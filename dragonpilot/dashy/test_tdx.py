"""Traffic extraction tests runnable without cereal, hardware, or UI dependencies."""
import ast
from datetime import datetime
import unittest
from pathlib import Path
from types import SimpleNamespace

from dragonpilot.selfdrive.controls.lib.tdx import expressway, freeway_lanes, urban_roads


class FakeSubMaster(dict):
    def __init__(self, *, started=True, alive=True, valid=True, speed=65):
        super().__init__(
            deviceState=SimpleNamespace(started=started),
            tdx=SimpleNamespace(
                sectionId='current', nextSectionId='ahead', speed=90,
                nextSpeed=speed, congestion='slow',
                events=[SimpleNamespace(eventType=1, description='事故', isCurrentSection=False)],
            ),
        )
        self.recv_frame = {'tdx': 10, 'carState': 10}
        self.alive = {'tdx': alive}
        self.valid = {'tdx': valid}


class TestTdxExtraction(unittest.TestCase):
    def setUp(self):
        # Load the actual pure extraction/formatting functions, excluding daemon imports.
        source = Path(__file__).with_name('dashyd.py').read_text(encoding='utf-8')
        tree = ast.parse(source)
        names = {'extract_tdx', 'format_speed', 'get_speed_unit'}
        functions = [node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name in names]
        self.namespace = {'_is_metric': True, 'Conversions': SimpleNamespace(MS_TO_KPH=3.6, MS_TO_MPH=2.236936292)}
        exec(compile(ast.Module(body=functions, type_ignores=[]), 'dashyd.py', 'exec'), self.namespace)
        self.extract = self.namespace['extract_tdx']

    def test_matches_c3x_without_valid_flag(self):
        self.assertIsNotNone(self.extract(FakeSubMaster(valid=False)))

    def test_previous_session_data_is_hidden(self):
        for topic in ('tdx', 'carState'):
            sm = FakeSubMaster()
            sm.recv_frame[topic] = 9
            with self.subTest(topic=topic):
                self.assertIsNone(self.extract(sm, started_frame=10))
        sm = FakeSubMaster()
        self.assertIsNotNone(self.extract(sm, started_frame=10))

    def test_traffic_and_events(self):
        result = self.extract(FakeSubMaster())
        self.assertEqual(result['displayNextSpeed'], '65')
        self.assertEqual(result['speedUnit'], 'km/h')
        self.assertEqual(result['events'], [{'eventType': 1, 'description': '事故', 'isCurrentSection': False}])

    def test_lane_speed_preserves_missing_samples_and_station_metadata(self):
        sm = FakeSubMaster()
        sm['tdx'].laneRoadLabel = '國一・汐五高架'
        sm['tdx'].lanePassedDistanceM = 80000
        sm['tdx'].laneCollectedAt = 1795618320
        sm['tdx'].laneSpeeds = [
            SimpleNamespace(laneId=0, speed=100), SimpleNamespace(laneId=1, speed=-1),
            SimpleNamespace(laneId=2, speed=0),
        ]
        self.namespace['_is_metric'] = False
        result = self.extract(sm)
        self.assertEqual(result['laneRoadLabel'], '國一・汐五高架')
        self.assertEqual(result['laneDistanceM'], 80000)
        self.assertTrue(result['laneStationPassed'])
        self.assertEqual(result['laneCollectedAt'], 1795618320)
        self.assertFalse(result['laneFromCache'])
        sm['tdx'].laneFromCache = True
        self.assertTrue(self.extract(sm)['laneFromCache'])
        self.assertEqual(result['laneSpeeds'], [
            {'laneId': 0, 'speed': '62'}, {'laneId': 1, 'speed': None}, {'laneId': 2, 'speed': '0'},
        ])

    def test_expired_and_offroad_clear_data(self):
        for settings in ({'started': False}, {'alive': False}):
            with self.subTest(settings=settings):
                self.assertIsNone(self.extract(FakeSubMaster(**settings)))

    def test_unknown_speed_keeps_event(self):
        for speed in (-1, 0, 151):
            with self.subTest(speed=speed):
                result = self.extract(FakeSubMaster(speed=speed))
                self.assertIsNone(result['displayNextSpeed'])
                self.assertEqual(len(result['events']), 1)

    def test_imperial_display_preserves_raw_kph(self):
        self.namespace['_is_metric'] = False
        result = self.extract(FakeSubMaster(speed=100))
        self.assertEqual(result['nextSpeed'], 100)
        self.assertEqual(result['displayNextSpeed'], '62')
        self.assertEqual(result['speedUnit'], 'mph')


class TestFreewayLaneSelection(unittest.TestCase):
    def test_road_labels_distinguish_parallel_routes(self):
        self.assertEqual(freeway_lanes.route_label('000010'), '國一・平面')
        self.assertEqual(freeway_lanes.route_label('00001A'), '國一・汐五高架')
        self.assertEqual(freeway_lanes.route_label('00001B'), '')

    def test_uses_nearest_passed_station_until_ahead_is_available(self):
        road = ('road', 'N')
        sections = {'section': (road, 3000, 2000)}
        routes = {road: [(2800, 'passed', 'n'), (2100, 'ahead', 'n')],
                  ('road', 'S'): [(2400, 'opposite', 's')]}
        passed = (1000, ((0, 79),))
        ahead = (1005, ((0, 65),))
        speeds = {('passed', 'n'): passed, ('opposite', 's'): ahead}
        lookup = freeway_lanes.select_lane_station
        self.assertEqual(lookup(sections, routes, speeds, 'section', .5, 1030), (-300, *passed))
        self.assertEqual(lookup(sections, routes, speeds, 'section', .8, 1030), (-600, *passed))
        self.assertIsNone(lookup(sections, routes, speeds, 'section', .5, 1181))
        speeds['ahead', 'n'] = ahead
        self.assertEqual(lookup(sections, routes, speeds, 'section', .5, 1030), (400, *ahead))

    def test_holds_last_lane_values_while_same_direction_has_average(self):
        reading = (250, 1000, ((0, 85), (1, 72)), '國一・平面')
        key = ('freeway', ('000010', 'N'))
        select = freeway_lanes.lane_display_for_average
        stored, cached = select(None, reading, key, True)
        self.assertFalse(cached)
        self.assertEqual(select(stored, None, key, True), (stored, True))
        self.assertEqual(select(stored, None, ('freeway', ('000010', 'S')), True), (None, False))
        self.assertEqual(select(stored, None, ('freeway', ('00001A', 'N')), True), (None, False))
        self.assertEqual(select(stored, None, key, False), (None, False))
        updated = (300, 1300, ((0, 64),), '國一・平面')
        self.assertEqual(select(stored, updated, key, True), ((key, updated), False))

    def test_city_station_walks_directional_sections_and_prefers_ahead(self):
        matcher = SimpleNamespace(sections={sid: {'total_length': 1000} for sid in ('a', 'b', 'c')},
                                  topology_next={'a': 'b', 'b': 'c'})
        stations = {'a': [(900, 'behind', 'a')], 'c': [(100, 'ahead', 'c')]}
        previous = {'b': 'a', 'c': 'b'}
        records = {('behind', 'a'): (1000, ((0, 60),))}
        lookup = urban_roads.walk_stations
        self.assertEqual(lookup(matcher, stations, previous, 'b', 300, records, 1100), (-800, 1000, ((0, 60),)))
        records['ahead', 'c'] = (1000, ((0, 75),))
        self.assertEqual(lookup(matcher, stations, previous, 'b', 300, records, 1100), (400, 1000, ((0, 75),)))
        self.assertIsNone(lookup(matcher, stations, previous, 'b', 300, records, 1301))

    def test_expressway_index_excludes_regular_provincial_roads(self):
        root = freeway_lanes.read_xml(b"""<SectionList><Sections>
          <Section><SectionID>express</SectionID><RoadClass>1</RoadClass></Section>
          <Section><SectionID>regular</SectionID><RoadClass>3</RoadClass></Section>
        </Sections></SectionList>""", 'SectionList')
        self.assertEqual(list(expressway.static_sections(root)), ['express'])
        self.assertTrue(expressway.same_road('台64線', '臺64'))
        self.assertFalse(expressway.same_road('台64線', '台65線'))

    def test_tdx_lane_json_obeys_same_freshness_and_status_rules(self):
        reading = {'VDID': 'station', 'Status': 0, 'DataCollectTime': '2026-11-26T12:00:00+08:00',
                   'LinkFlows': [{'LinkID': 'link', 'Lanes': [
                       {'LaneID': 2, 'LaneType': 1, 'Speed': '30.4'},
                       {'LaneID': 0, 'LaneType': 1, 'Speed': 0, 'Vehicles': [{'Volume': 1}]},
                       {'LaneID': 1, 'LaneType': 8, 'Speed': 22},
                   ]}]}
        collected = datetime.fromisoformat(reading['DataCollectTime']).timestamp()
        feed = {'VDLives': [reading]}
        self.assertEqual(freeway_lanes.parse_live(feed, collected + 120),
                         {('station', 'link'): (collected, ((0, 0), (2, 30)))})
        self.assertEqual(freeway_lanes.parse_live(feed, collected + 181), {})
        reading['Status'] = 1
        self.assertEqual(freeway_lanes.parse_live(feed, collected + 120), {})

    def test_selects_fresh_station_ahead_on_same_road_and_direction(self):
        sections = {'north': (('N1', 'N'), 35000, 33000)}
        xml = freeway_lanes.read_xml(b"""
            <VDList><VDs>
              <VD><VDID>ahead</VDID><RoadID>N1</RoadID><DetectionType>1</DetectionType>
                <LocationMile>33K+800</LocationMile><DetectionLinks><DetectionLink>
                  <RoadDirection>N</RoadDirection><LinkID>north-link</LinkID>
                </DetectionLink></DetectionLinks>
              </VD>
              <VD><VDID>ramp</VDID><RoadID>N1</RoadID><DetectionType>2</DetectionType>
                <LocationMile>33K+990</LocationMile><DetectionLinks><DetectionLink>
                  <RoadDirection>N</RoadDirection><LinkID>ramp-link</LinkID>
                </DetectionLink></DetectionLinks>
              </VD>
            </VDs></VDList>
        """, 'VDList')
        routes = freeway_lanes.parse_stations(xml)
        routes['N1', 'N'].append((34100, 'behind', 'behind-link'))
        routes['N1', 'S'].append((33990, 'opposite', 'opposite-link'))
        routes['N1-elevated', 'N'].append((33995, 'elevated', 'elevated-link'))
        reading = (1000, ((0, 91), (1, -1)))
        readings = dict.fromkeys([
            ('ahead', 'north-link'), ('ramp', 'ramp-link'), ('behind', 'behind-link'),
            ('opposite', 'opposite-link'), ('elevated', 'elevated-link'),
        ], reading)
        self.assertEqual(freeway_lanes.select_lane_station(sections, routes, readings, 'north', 0.5, 1050),
                         (200, *reading))
        self.assertIsNone(freeway_lanes.select_lane_station(sections, routes, readings, 'north', 0.5, 1181))


if __name__ == '__main__':
    unittest.main()
