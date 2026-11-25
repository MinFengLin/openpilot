"""Traffic extraction tests runnable without cereal, hardware, or UI dependencies."""
import ast
import unittest
from pathlib import Path
from types import SimpleNamespace

from dragonpilot.selfdrive.controls.lib.tdx import freeway_lanes


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
        sm['tdx'].laneDistanceM = 1800
        sm['tdx'].laneCollectedAt = 1795618320
        sm['tdx'].laneSpeeds = [
            SimpleNamespace(laneId=0, speed=100), SimpleNamespace(laneId=1, speed=-1),
            SimpleNamespace(laneId=2, speed=0),
        ]
        self.namespace['_is_metric'] = False
        result = self.extract(sm)
        self.assertEqual(result['laneDistanceM'], 1800)
        self.assertEqual(result['laneCollectedAt'], 1795618320)
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
        self.assertEqual(freeway_lanes.select_ahead(sections, routes, readings, 'north', 0.5, 1050),
                         (200, *reading))
        self.assertIsNone(freeway_lanes.select_ahead(sections, routes, readings, 'north', 0.5, 1181))


if __name__ == '__main__':
    unittest.main()
