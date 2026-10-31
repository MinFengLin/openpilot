import hashlib
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock, patch

from dragonpilot.dashy.road_info import format_road_info
from dragonpilot.mapd import download_locations, progress_complete, region_key, regions
from dragonpilot.mapd import manager


class TestOfflineRoadInfo(unittest.TestCase):
    def test_speed_unit_matches_sp_meters_per_second(self):
        self.assertAlmostEqual(format_road_info(110 / 3.6, '國道一號')['speedLimitKph'], 110)

    def test_unknown_limit_keeps_name_without_inventing_a_limit(self):
        for value in (None, 0, -1, float('nan'), 'invalid'):
            self.assertEqual(format_road_info(value, '道路'), {'roadName': '道路', 'speedLimitKph': None})
        self.assertIsNone(format_road_info(0, ''))

    def test_country_and_state_requests(self):
        self.assertEqual(download_locations('TW'), {'nations': ['TW'], 'states': []})
        self.assertEqual(download_locations('US', 'CA'), {'nations': [], 'states': ['CA']})
        self.assertEqual(download_locations('US'), {'nations': ['US'], 'states': []})
        self.assertEqual(region_key('TW', 'CA'), 'TW')
        self.assertEqual(region_key('US', 'CA'), 'US:CA')
        with self.assertRaises(ValueError):
            download_locations('../invalid')
        with self.assertRaises(ValueError):
            download_locations('US', 'invalid')

    def test_catalog_includes_requested_countries(self):
        for code in ('TW', 'JP', 'US'):
            self.assertIn(code, regions())

    def test_partial_or_empty_download_not_installed(self):
        for progress in ({}, {'total_files': 0, 'downloaded_files': 0},
                         {'total_files': 10, 'downloaded_files': 9}):
            self.assertFalse(progress_complete(progress))
        self.assertTrue(progress_complete({'total_files': 10, 'downloaded_files': 10}))

    def test_install_is_offroad_only_and_checks_sha256(self):
        data = b'test binary'
        class Response:
            def __enter__(self): return self
            def __exit__(self, *_): pass
            def raise_for_status(self): pass
            def iter_content(self, _): yield data
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'bin' / 'mapd'
            requests = SimpleNamespace(get=lambda *_, **__: Response())
            with patch.object(manager, 'BINARY', binary), patch.dict('sys.modules', {'requests': requests}):
                with self.assertRaises(RuntimeError):
                    manager.install(lambda: False)
                with self.assertRaises(RuntimeError):
                    manager.install(lambda: True)
                self.assertFalse(binary.exists())
                with patch.object(manager, 'SHA256', hashlib.sha256(data).hexdigest()):
                    manager.install(lambda: True)
                    self.assertEqual(binary.read_bytes(), data)
                binary.unlink()
                with patch.object(manager, 'SHA256', hashlib.sha256(data).hexdigest()):
                    with self.assertRaises(RuntimeError):
                        manager.install(Mock(side_effect=[True, True, False]))
                    self.assertFalse(binary.exists())
                    self.assertFalse(binary.with_suffix('.tmp').exists())


class TestMapdLifecycle(unittest.TestCase):
    def run_manager(self, *, offroad, request='', crashes=False):
        class EndLoop(BaseException):
            pass

        class FakeParams(dict):
            def get_bool(self, key): return bool(self.get(key))
            def put(self, key, value, **_): self[key] = value
            def put_bool(self, key, value, **_): self[key] = value
            def remove(self, key): self.pop(key, None)
            def get_param_path(self, key): return '/fake/' + key

        params = FakeParams(IsOffroad=offroad, dp_dashy_road_info=True,
                            dp_mapd_installed_regions={'TW': 1}, dp_mapd_request=request)
        mem = FakeParams()
        sm = {'deviceState': SimpleNamespace(started=not offroad)}
        class SubMaster(dict):
            seen = {'liveGPS': False}
            def update(self, _): pass
        sm = SubMaster(sm, liveGPS=SimpleNamespace())
        child = Mock()
        child.poll.side_effect = [None, 1, 1, 1] if crashes else None
        child.poll.return_value = None
        child.returncode = 1
        modules = {
            'cereal': SimpleNamespace(messaging=SimpleNamespace(SubMaster=lambda *a, **k: sm)),
            'openpilot.common.params': SimpleNamespace(Params=lambda path=None: mem if path else params),
            'openpilot.common.swaglog': SimpleNamespace(cloudlog=Mock()),
        }
        with patch.dict('sys.modules', modules), patch.object(manager.signal, 'signal'), \
             patch.object(manager, 'BINARY') as binary, patch.object(manager, 'ROOT'), \
             patch.object(manager, 'install'), patch.object(manager.platform, 'machine', return_value='aarch64'), \
             patch.object(manager.platform, 'system', return_value='Linux'), \
             patch.object(manager.shutil, 'disk_usage', return_value=SimpleNamespace(free=3 * 1024**3)), \
             patch.object(manager.os.path, 'exists', return_value=True), \
             patch.object(manager.subprocess, 'Popen', return_value=child) as spawn, \
             patch.object(manager.time, 'sleep', side_effect=[None, EndLoop()] if crashes else EndLoop()):
            binary.exists.return_value = True
            with self.assertRaises(EndLoop):
                manager.main()
        return params, mem, spawn

    def test_lookup_stays_stopped_offroad(self):
        _, mem, spawn = self.run_manager(offroad=True)
        spawn.assert_not_called()
        self.assertFalse(mem['dp_mapd_active'])

    def test_lookup_starts_onroad(self):
        _, _, spawn = self.run_manager(offroad=False)
        spawn.assert_called_once()

    def test_download_crash_clears_busy_instead_of_restarting_forever(self):
        params, _, spawn = self.run_manager(offroad=True, request='download', crashes=True)
        spawn.assert_called_once()
        self.assertFalse(params['dp_mapd_busy'])
        self.assertIn('下載程序已退出', params['dp_mapd_status'])
        self.assertNotIn('TW', params['dp_mapd_installed_regions'])


if __name__ == '__main__':
    unittest.main()
