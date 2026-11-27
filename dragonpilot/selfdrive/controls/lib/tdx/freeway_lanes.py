"""Highway Bureau mainline VD data keyed by road and direction."""
from collections import defaultdict
from datetime import datetime
import math
import re
import xml.etree.ElementTree as ET


MAX_AGE_S = 180
MAX_DISTANCE_M = 3000


def read_xml(content, expected_tag):
  root = ET.fromstring(content)
  for element in root.iter():
    element.tag = element.tag.rsplit('}', 1)[-1]
  if root.tag != expected_tag:
    raise ValueError(f'Expected {expected_tag}, got {root.tag}')
  return root


def mileage_m(text):
  match = re.fullmatch(r'(\d+)K\+(\d{1,3})', text or '')
  if match is None:
    raise ValueError('Invalid freeway mileage')
  return 1000 * int(match[1]) + int(match[2])


def parse_sections(root):
  sections = {}
  for entry in root.findall('Sections/Section'):
    section = entry.findtext('SectionID')
    route = (entry.findtext('RoadID'), entry.findtext('RoadDirection'))
    try:
      start = mileage_m(entry.findtext('SectionMile/StartKM'))
      end = mileage_m(entry.findtext('SectionMile/EndKM'))
    except ValueError:
      continue
    if section and all(route) and start != end:
      sections[section] = (route, start, end)
  return sections


def route_label(road_id):
  return {
    '000010': '國一・平面',
    '00001A': '國一・汐五高架',
  }.get(road_id, '')


def parse_stations(root, locate=None):
  routes = defaultdict(list)
  for entry in root.findall('VDs/VD'):
    if entry.findtext('DetectionType') != '1':  # Mainline, excluding ramps.
      continue
    station_id = entry.findtext('VDID')
    road = entry.findtext('RoadID')
    if not station_id or not road:
      continue
    for link in entry.findall('DetectionLinks/DetectionLink'):
      direction = link.findtext('RoadDirection')
      link_id = link.findtext('LinkID')
      if not direction or not link_id:
        continue
      route = (road, direction)
      try:
        mile = mileage_m(entry.findtext('LocationMile')) if locate is None else locate(entry, link, route)
      except (TypeError, ValueError):
        continue
      if mile is not None:
        routes[route].append((mile, station_id, link_id))
  return routes


def is_fresh_timestamp(value, now, max_age=MAX_AGE_S):
  try:
    timestamp = datetime.fromisoformat(value)
    return timestamp.tzinfo is not None and 0 <= now - timestamp.timestamp() <= max_age
  except (ValueError, TypeError):
    return False


def _text(record, key):
  value = record.get(key) if isinstance(record, dict) else record.findtext(key)
  return '' if value is None else str(value)


def _rows(record, group, item):
  if isinstance(record, dict):
    values = record.get(group)
    return (value for value in values if isinstance(value, dict)) if isinstance(values, list) else ()
  return record.findall(f'{group}/{item}')


def parse_live(root, now, max_age=MAX_AGE_S):
  readings = {}
  for entry in _rows(root, 'VDLives', 'VDLive'):
    station_id = _text(entry, 'VDID')
    if not station_id or _text(entry, 'Status') != '0':
      continue
    timestamp = _text(entry, 'DataCollectTime')
    if not is_fresh_timestamp(timestamp, now, max_age):
      continue
    collected = datetime.fromisoformat(timestamp).timestamp()
    for flow in _rows(entry, 'LinkFlows', 'LinkFlow'):
      link_id = _text(flow, 'LinkID')
      if not link_id:
        continue
      lanes = {}
      for lane in _rows(flow, 'Lanes', 'Lane'):
        if _text(lane, 'LaneType') == '8':  # Shoulder.
          continue
        try:
          lane_id = int(_text(lane, 'LaneID'))
        except ValueError:
          continue
        if not 0 <= lane_id <= 255:
          continue
        try:
          speed = float(_text(lane, 'Speed'))
        except ValueError:
          speed = -1
        if speed == 0 and not any(
          volume.isascii() and volume.isdigit() and int(volume) > 0
          for volume in (_text(vehicle, 'Volume') for vehicle in _rows(lane, 'Vehicles', 'Vehicle'))
        ):
          speed = -1  # The feed reports zero for lanes without vehicle samples.
        lanes[lane_id] = round(speed) if math.isfinite(speed) and 0 <= speed <= 150 else -1
      if any(speed >= 0 for speed in lanes.values()):
        readings[station_id, link_id] = (collected, tuple(sorted(lanes.items())))
  return readings


def select_lane_station(sections, routes, readings, section_id, fraction, now):
  """Prefer a fresh station ahead; otherwise use the nearest station behind."""
  section = sections.get(section_id)
  if section is None:
    return None
  route, start, end = section
  mile = start + fraction * (end - start)
  direction = 1 if end > start else -1
  nearest_ahead = nearest_passed = None
  ahead_limit = MAX_DISTANCE_M
  passed_limit = math.inf
  for station_mile, station_id, link_id in routes.get(route, ()):
    distance = (station_mile - mile) * direction
    if not -passed_limit <= distance <= ahead_limit:
      continue
    reading = readings.get((station_id, link_id))
    if reading is None or not 0 <= now - reading[0] <= MAX_AGE_S:
      continue
    collected, lanes = reading
    if distance >= 0:
      nearest_ahead = (round(distance), int(collected), lanes)
      ahead_limit = distance
    else:
      nearest_passed = (-max(1, round(-distance)), int(collected), lanes)
      passed_limit = -distance
  return nearest_ahead if nearest_ahead is not None else nearest_passed


def hold_lane_reading(previous, reading, section_key, now, wall_time, hold_seconds=3, max_age=MAX_AGE_S):
  """Keep a recent, still fresh measurement through a short loss on the same section."""
  if reading is not None:
    return section_key, reading, now
  if previous is not None:
    section, held, received = previous
    if (section == section_key and 0 <= now - received <= hold_seconds
        and 0 <= wall_time - held[1] <= max_age):
      return previous
  return None
