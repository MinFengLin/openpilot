"""Highway Bureau mainline VD data keyed by road and direction."""
from collections import defaultdict
from datetime import datetime
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


def parse_stations(root):
  routes = defaultdict(list)
  for entry in root.findall('VDs/VD'):
    if entry.findtext('DetectionType') != '1':  # Mainline, excluding ramps.
      continue
    station_id = entry.findtext('VDID')
    road = entry.findtext('RoadID')
    if not station_id or not road:
      continue
    try:
      mile = mileage_m(entry.findtext('LocationMile'))
    except ValueError:
      continue
    for link in entry.findall('DetectionLinks/DetectionLink'):
      direction = link.findtext('RoadDirection')
      link_id = link.findtext('LinkID')
      if direction and link_id:
        routes[road, direction].append((mile, station_id, link_id))
  return routes


def parse_live(root, now):
  readings = {}
  for entry in root.findall('VDLives/VDLive'):
    station_id = entry.findtext('VDID')
    if not station_id or entry.findtext('Status') != '0':
      continue
    try:
      timestamp = datetime.fromisoformat(entry.findtext('DataCollectTime', ''))
      if timestamp.tzinfo is None:
        continue
      collected = timestamp.timestamp()
    except ValueError:
      continue
    if not 0 <= now - collected <= MAX_AGE_S:
      continue
    for flow in entry.findall('LinkFlows/LinkFlow'):
      link_id = flow.findtext('LinkID')
      if not link_id:
        continue
      lanes = {}
      for lane in flow.findall('Lanes/Lane'):
        if lane.findtext('LaneType') == '8':  # Shoulder.
          continue
        try:
          lane_id = int(lane.findtext('LaneID', ''))
        except ValueError:
          continue
        if not 0 <= lane_id <= 255:
          continue
        try:
          speed = int(lane.findtext('Speed', ''))
        except ValueError:
          speed = -1
        if speed == 0 and not any(
          vehicle.findtext('Volume', '').isdigit() and int(vehicle.findtext('Volume')) > 0
          for vehicle in lane.findall('Vehicles/Vehicle')
        ):
          speed = -1  # The feed reports zero for lanes without vehicle samples.
        lanes[lane_id] = speed if 0 <= speed <= 150 else -1
      if any(speed >= 0 for speed in lanes.values()):
        readings[station_id, link_id] = (collected, tuple(sorted(lanes.items())))
  return readings


def select_ahead(sections, routes, readings, section_id, fraction, now):
  section = sections.get(section_id)
  if section is None:
    return None
  route, start, end = section
  mile = start + fraction * (end - start)
  direction = 1 if end > start else -1
  nearest = None
  limit = MAX_DISTANCE_M
  for station_mile, station_id, link_id in routes.get(route, ()):
    distance = (station_mile - mile) * direction
    if not 0 < distance <= limit:
      continue
    reading = readings.get((station_id, link_id))
    if reading is None or not 0 <= now - reading[0] <= MAX_AGE_S:
      continue
    collected, lanes = reading
    nearest = (round(distance), int(collected), lanes)
    limit = distance
  return nearest
