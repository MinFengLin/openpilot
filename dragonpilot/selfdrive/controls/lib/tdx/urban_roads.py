"""Taipei's municipal expressways and traffic feeds."""
from collections import defaultdict
import json
import math
from pathlib import Path
import time

import requests

from dragonpilot.selfdrive.controls.lib.tdx import freeway_lanes

PUBLIC_URL = 'https://tcgbusfs.blob.core.windows.net/blobtisv'
TDX_TRAFFIC = 'v2/Road/Traffic/Live/City/Taipei'
TDX_VD = 'v2/Road/Traffic/Live/VD/City/Taipei'
CITY_SHAPES_PATH = Path(__file__).with_name('taipei_expressway_shapes.json')
TIMEOUT_S = 5
MAX_AGE_S = 300


def walk_stations(matcher, stations, previous, section_id, remaining, readings, now):
  """Return the next station on this route, or the nearest fresh station behind."""
  def walk_links(forward):
    candidates = []
    visited = set()
    current = section_id
    offset = remaining - matcher.sections[current]['total_length']
    while current and current not in visited:
      visited.add(current)
      length = matcher.sections[current]['total_length']
      if forward and offset > freeway_lanes.MAX_DISTANCE_M:
        break
      for mile, station, link in stations.get(current, ()):
        distance = offset + mile
        if not (0 <= distance <= freeway_lanes.MAX_DISTANCE_M if forward else distance < 0):
          continue
        reading = readings.get((station, link))
        if reading is not None and 0 <= now - reading[0] <= MAX_AGE_S:
          candidates.append((abs(distance), distance, reading))
      if candidates:
        _, distance, (collected, speeds) = min(candidates)
        meters = round(distance) if forward else -max(1, round(-distance))
        return meters, int(collected), speeds
      current = matcher.topology_next.get(current) if forward else previous.get(current)
      if current:
        offset += length if forward else -matcher.sections[current]['total_length']
    return None

  return walk_links(True) or walk_links(False)


class TaipeiTraffic:
  def __init__(self, matcher_factory):
    try:
      metadata = json.loads(CITY_SHAPES_PATH.read_text(encoding='utf-8'))
    except (OSError, ValueError):
      metadata = {}
    self.version = metadata.get('LinkVersion')
    self.matcher = matcher_factory(metadata.get('SectionShapes', []))
    self.stations = defaultdict(list)
    for section, station, link, lon, lat in metadata.get('Stations', []):
      if section in self.matcher.sections:
        length = self.matcher.sections[section]['total_length']
        remaining = self.matcher.distance_to_section_end(section, lat, lon)
        self.stations[section].append((length - remaining, station, link))

    predecessors = defaultdict(list)
    for section, following in self.matcher.topology_next.items():
      predecessors[following].append(section)
    self.previous = {section: choices[0] for section, choices in predecessors.items() if len(choices) == 1}
    self.cached_speeds = {}
    self.cached_events = []
    self._lanes = {}

  def _public(self, filename, root_tag):
    response = requests.get(f'{PUBLIC_URL}/{filename}.xml', timeout=TIMEOUT_S)
    response.raise_for_status()
    data = freeway_lanes.read_xml(response.content, root_tag)
    if data.findtext('LinkVersion') != self.version:
      raise ValueError('Taipei feed link version differs from map')
    return data

  def public_speeds(self):
    data = self._public('LiveTraffic', 'LiveTrafficList')
    rows = ((e.findtext('SectionID'), e.findtext('TravelSpeed'), e.findtext('DataCollectTime'))
            for e in data.findall('LiveTraffics/LiveTraffic'))
    return self._speeds(rows)

  def tdx_speeds(self, source):
    now = time.time()  # noqa: TID251 - feed timestamps use Unix time
    data = source.fetch(TDX_TRAFFIC, 'LiveTraffics', now, max_age=MAX_AGE_S)
    if data.get('LinkVersion') != self.version:
      raise ValueError('Taipei TDX feed link version differs from map')
    return self._speeds((e.get('SectionID'), e.get('TravelSpeed'), e.get('DataCollectTime'))
                        for e in data['LiveTraffics'] if isinstance(e, dict))

  def _speeds(self, rows):
    now = time.time()  # noqa: TID251 - feed timestamps use Unix time
    speeds = {}
    for section, value, collected in rows:
      if not isinstance(section, str) or section not in self.matcher.sections or not freeway_lanes.is_fresh_timestamp(collected, now, MAX_AGE_S):
        continue
      try:
        speed = float(value)
      except (TypeError, ValueError):
        continue
      if math.isfinite(speed) and 1 <= speed <= 150:
        speeds[section] = round(speed)
    if not speeds:
      raise ValueError('no fresh Taipei expressway speeds')
    return speeds

  def public_lanes(self):
    return self._parse_lanes(self._public('VDLive', 'VDLiveList'))

  def tdx_lanes(self, source):
    now = time.time()  # noqa: TID251 - feed timestamps use Unix time
    data = source.fetch(TDX_VD, 'VDLives', now, max_age=MAX_AGE_S)
    if data.get('LinkVersion') != self.version:
      raise ValueError('Taipei TDX feed link version differs from map')
    return self._parse_lanes(data)

  def _parse_lanes(self, data):
    now = time.time()  # noqa: TID251 - feed timestamps use Unix time
    readings = freeway_lanes.parse_live(data, now, MAX_AGE_S)
    lanes = {(vd, link): readings[vd, link] for stations in self.stations.values() for _, vd, link in stations
             if (vd, link) in readings}
    if not lanes:
      raise ValueError('no fresh Taipei expressway lane speeds')
    return lanes

  def find_ahead(self, matcher, section_id, lat, lon, bearing):
    if section_id is None or not self._lanes:
      return None
    if matcher.find_current_section(lat, lon, threshold_meters=35, bearing_deg=bearing) != section_id:
      return None
    remaining = matcher.distance_to_section_end(section_id, lat, lon)
    now = time.time()  # noqa: TID251 - feed timestamps use Unix time
    reading = walk_stations(matcher, self.stations, self.previous, section_id, remaining, self._lanes, now)
    if reading is not None:
      return (*reading, matcher.sections[section_id]['road_name'])
    return None

  def store(self, fetch, get_tdx):
    self.cached_speeds = fetch('Taipei road speeds', self.public_speeds, self.tdx_speeds, get_tdx, {})
    self._lanes = fetch('Taipei lane speeds', self.public_lanes, self.tdx_lanes, get_tdx, self._lanes)
