"""Traffic extraction tests runnable without cereal, hardware, or UI dependencies."""
import ast
import unittest
from pathlib import Path
from types import SimpleNamespace


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

    def test_traffic_and_events(self):
        result = self.extract(FakeSubMaster())
        self.assertEqual(result['displayNextSpeed'], '65')
        self.assertEqual(result['speedUnit'], 'km/h')
        self.assertEqual(result['events'], [{'eventType': 1, 'description': '事故', 'isCurrentSection': False}])

    def test_expired_invalid_and_offroad_clear_data(self):
        for settings in ({'started': False}, {'alive': False}, {'valid': False}):
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


if __name__ == '__main__':
    unittest.main()
