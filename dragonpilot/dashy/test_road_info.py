import hashlib
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

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


if __name__ == '__main__':
    unittest.main()
