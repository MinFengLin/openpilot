"""Provincial Highway Bureau data for signed expressways (RoadClass 1)."""

BASE_URL = 'https://thbapp.thb.gov.tw/opendata'
ENDPOINTS = {
  'Section': 'section/sectioninfo/SectionList',
  'SectionShape': 'section/sectionshapeinfo/SectionShapeList',
  'VD': 'vd/info/VDList',
  'VDLive': 'vd/one/VDLiveList',
  'LiveTraffic': 'section/livetrafficdata/LiveTrafficList',
}
BEARINGS = {name: 45 * i for i, name in enumerate(('N', 'NE', 'E', 'SE', 'S', 'SW', 'W', 'NW'))}
TRAFFIC_MAX_AGE_S = 600


def static_sections(root):
  return {entry.findtext('SectionID'): entry for entry in root.findall('Sections/Section')
          if entry.findtext('RoadClass') == '1' and entry.findtext('SectionID')}


def static_shapes(root, sections, records):
  return [{'SectionID': section_id, 'Geometry': shape.findtext('Geometry'), 'route': sections[section_id],
           'road_name': records[section_id].findtext('RoadName', '')}
          for shape in root.findall('SectionShapes/SectionShape')
          if (section_id := shape.findtext('SectionID')) in sections and shape.findtext('Geometry')]


def station_mile(matcher, sections, route, latitude, longitude, bearing):
  match = matcher.find_current_section(latitude, longitude, threshold_meters=55, bearing_deg=bearing)
  if match not in sections or sections[match][0] != route:
    return None
  _, start, end = sections[match]
  length = matcher.sections[match]['total_length']
  if length <= 0:
    return None
  fraction = 1 - matcher.distance_to_section_end(match, latitude, longitude) / length
  return round(start + fraction * (end - start))


def same_road(first, second):
  return bool(first and second and first.replace('臺', '台').removesuffix('線') ==
              second.replace('臺', '台').removesuffix('線'))
