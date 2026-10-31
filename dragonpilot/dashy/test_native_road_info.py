"""Exercise shared freshness checks and native HUD drawing without a GPU."""
import ast
import os
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock, patch

from dragonpilot.mapd.road_info import read_road_info


class TestNativeRoadInfo(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        class Params(dict):
            def get_bool(self, key): return bool(self.get(key))
            def get_param_path(inner, key): return str(Path(self.directory.name) / key)
        self.params = Params(dp_dashy_road_info=True, dp_mapd_installed_regions={'TW': 1})
        self.mem = Params(dp_mapd_active=True, MapSpeedLimit=110 / 3.6, RoadName='國道一號')
        for key in ('MapSpeedLimit', 'RoadName'):
            Path(self.mem.get_param_path(key)).touch()
        class SubMaster(dict):
            seen = {'liveGPS': True}
            valid = {'liveGPS': True}
            recv_time = {'liveGPS': 100}
            recv_frame = {'liveGPS': 10, 'carState': 10}
        self.sm = SubMaster(liveGPS=SimpleNamespace(gpsOK=True), deviceState=SimpleNamespace(started=True))
        self.clock = patch('time.monotonic', return_value=100).start()
        self.addCleanup(patch.stopall)

    def test_both_displays_clear_when_prerequisites_expire(self):
        self.assertEqual(read_road_info(self.params, self.mem, self.sm)['roadName'], '國道一號')
        for store, key, value in ((self.params, 'dp_dashy_road_info', False),
                                  (self.params, 'dp_mapd_country', 'JP'),
                                  (self.mem, 'dp_mapd_active', False)):
            with patch.dict(store, {key: value}):
                self.assertIsNone(read_road_info(self.params, self.mem, self.sm))
        with patch.dict(self.sm.valid, {'liveGPS': False}):
            self.assertIsNone(read_road_info(self.params, self.mem, self.sm))
        self.sm['deviceState'].started = False
        self.assertIsNone(read_road_info(self.params, self.mem, self.sm))
        self.sm['deviceState'].started = True
        self.clock.return_value = 104
        self.assertIsNone(read_road_info(self.params, self.mem, self.sm))
        self.clock.return_value = 100
        os.utime(self.mem.get_param_path('RoadName'), (1, 1))
        self.assertIsNone(read_road_info(self.params, self.mem, self.sm))

    def test_native_layout_units_and_session_clear(self):
        # Load the real renderer methods, replacing only hardware imports.
        source = Path(__file__).parents[1] / 'selfdrive/ui/road_info.py'
        tree = ast.parse(source.read_text())
        class RemoveImports(ast.NodeTransformer):
            def visit_ImportFrom(self, node): return None
        classes = [RemoveImports().visit(n) for n in tree.body if isinstance(n, ast.ClassDef)]
        def rectangle(x, y, width, height):
            return SimpleNamespace(x=x, y=y, width=width, height=height)
        ray = SimpleNamespace(Rectangle=rectangle, Vector2=lambda x, y: SimpleNamespace(x=x, y=y),
                              Color=lambda *args: args, WHITE=(255, 255, 255, 255), BLACK=(0, 0, 0, 255),
                              color_alpha=lambda color, alpha: color)
        for name in ('draw_circle_v', 'draw_ring', 'draw_text_ex', 'draw_rectangle_rounded', 'draw_rectangle_rounded_lines_ex'):
            setattr(ray, name, Mock())
        ui = SimpleNamespace(sm=self.sm, params=self.params, started=True, started_frame=10, is_metric=True)
        namespace = dict(rl=ray, Widget=object, Params=lambda _: self.mem, ui_state=ui,
                         time=SimpleNamespace(monotonic=self.clock), read_road_info=read_road_info,
                         gui_app=SimpleNamespace(font=lambda _: 'font'), FontWeight=SimpleNamespace(BOLD=1, SEMI_BOLD=2),
                         measure_text_cached=lambda font, text, size: SimpleNamespace(x=len(text) * size / 2, y=size),
                         UI_CONFIG=SimpleNamespace(set_speed_width_metric=200, set_speed_width_imperial=172, set_speed_height=204))
        exec(compile(ast.Module(body=classes, type_ignores=[]), str(source), 'exec'), namespace)
        renderer = namespace['OfflineRoadInfoRenderer']()
        renderer.update()
        self.assertEqual(renderer.limit, '110')
        self.assertEqual(renderer.road_name, '國道一號')
        rect = rectangle(30, 30, 2100, 1000)
        renderer._render(rect)
        self.assertEqual(ray.draw_circle_v.call_count, 1)
        self.assertGreater(ray.draw_circle_v.call_args.args[0].x, rect.x + 260)
        ui.is_metric = False
        self.clock.return_value = 101
        renderer.update()
        self.assertEqual(renderer.limit, '68')
        ray.draw_circle_v.reset_mock()
        renderer._render(rect)
        ray.draw_circle_v.assert_not_called()
        drawn = [call.args[1] for call in ray.draw_text_ex.call_args_list]
        self.assertIn('SPEED', drawn)
        self.assertIn('LIMIT', drawn)
        self.sm.recv_frame = {'liveGPS': 9, 'carState': 10}
        renderer.update()
        self.assertEqual((renderer.limit, renderer.road_name), ('', ''))
