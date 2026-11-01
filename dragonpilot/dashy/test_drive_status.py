import time
import unittest
from types import SimpleNamespace
from unittest.mock import patch

from dragonpilot.dashy.drive_status import boot_seconds, extract_drive_status


class SubMaster(dict):
    def __init__(self, brand='toyota', longitudinal=True, flags=0, min_speed=-1):
        super().__init__(carParams=SimpleNamespace(brand=brand, openpilotLongitudinalControl=longitudinal,
                                                 flags=flags, minEnableSpeed=min_speed),
                         carControl=SimpleNamespace(latActive=True, longActive=True, hudControl=SimpleNamespace(leadDistanceBars=2)),
                         modelV2=SimpleNamespace(laneLineProbs=[0.9, 0.8, 0.1, 0.9]),
                         carState=SimpleNamespace(standstill=False, leftBlindspot=False, rightBlindspot=False), radarState=SimpleNamespace(leadOne=SimpleNamespace(status=True, dRel=25.0)),
                         selfdriveState=SimpleNamespace(personality='standard'))
        self.alive = {key: True for key in self}
        self.valid = {key: True for key in self}


class TestDriveStatus(unittest.TestCase):
    def setUp(self):
        self.params = SimpleNamespace(get_bool=lambda key: key == 'dp_lane_centering')

    def extract(self, sm):
        with patch('dragonpilot.dashy.drive_status.boot_seconds', return_value=3661):
            return extract_drive_status(sm, self.params)

    def test_timer_uses_device_boot_clock(self):
        with patch('time.clock_gettime', return_value=3661.9) as clock:
            self.assertEqual(boot_seconds(), 3661)
            clock.assert_called_once_with(time.CLOCK_BOOTTIME)

    def test_vehicle_modes_and_active_control(self):
        result = self.extract(SubMaster())
        self.assertEqual(result['uptimeSeconds'], 3661)
        self.assertTrue(result['assist']['lateralActive'])
        self.assertTrue(result['assist']['leadDetected'])
        self.assertEqual(result['assist']['leadDistanceMeters'], 25.0)
        for bars, mode in enumerate(('aggressive', 'standard', 'relaxed'), 1):
            sm = SubMaster(); sm['selfdriveState'].personality = 'standard'; sm['carControl'].hudControl.leadDistanceBars = bars
            self.assertEqual(self.extract(sm)['assist']['followMode'], mode)

    def test_all_brands_use_shared_vehicle_signals(self):
        for brand in ('toyota', 'honda', 'hyundai', 'volkswagen', 'mock'):
            with self.subTest(brand=brand):
                state = self.extract(SubMaster(brand=brand))['assist']
                self.assertTrue(state['longitudinalActive'])
                self.assertEqual(state['followBars'], 2)
                self.assertEqual(state['leftGlow'], 'blue')

    def test_stock_longitudinal_does_not_claim_openpilot_modes(self):
        toyota = self.extract(SubMaster(longitudinal=False))['assist']
        self.assertIsNone(toyota['longitudinalActive'])
        self.assertIsNone(toyota['followMode'])

    def test_missing_or_stale_topics_clear_states(self):
        for field in ('alive', 'valid'):
            for topic in ('carParams', 'carControl', 'radarState'):
                with self.subTest(field=field, topic=topic):
                    sm = SubMaster(); getattr(sm, field)[topic] = False
                    state = self.extract(sm)
                    if topic == 'carParams': self.assertIsNone(state['assist'])
                    elif topic == 'carControl': self.assertIsNone(state['assist']['lateralActive'])
                    else: self.assertIsNone(state['assist']['leadDetected'])

    def test_live_changes_not_settings_drive_icons(self):
        sm = SubMaster()
        sm['carControl'].latActive = False
        sm['carState'].standstill = True
        sm['radarState'].leadOne.status = False
        state = self.extract(sm)['assist']
        self.assertTrue(state['laneCenteringEnabled'])
        self.assertFalse(state['lateralActive'])
        self.assertFalse(state['leadDetected'])
        sm.alive['carControl'] = False
        state = self.extract(sm)['assist']
        self.assertEqual(state['followBars'], 0)

    def test_glow_is_per_side_and_blindspot_takes_priority(self):
        sm = SubMaster()
        state = self.extract(sm)['assist']
        self.assertEqual((state['leftGlow'], state['rightGlow']), ('blue', 'none'))
        sm['carState'].rightBlindspot = True
        sm['carState'].leftBlindspot = True
        state = self.extract(sm)['assist']
        self.assertEqual((state['leftGlow'], state['rightGlow']), ('green', 'green'))
        sm.valid['carState'] = False
        self.assertEqual(self.extract(sm)['assist']['rightGlow'], 'none')
        sm.alive['modelV2'] = False
        state = self.extract(sm)['assist']
        self.assertEqual((state['leftGlow'], state['rightGlow']), ('none', 'none'))
        sm.valid['carState'] = True
        self.assertEqual(self.extract(sm)['assist']['leftGlow'], 'green')

    def test_invalid_lane_confidence_does_not_glow(self):
        sm = SubMaster()
        for value in (float('nan'), float('inf'), -1, 1.1, 0.34):
            sm['modelV2'].laneLineProbs = [0.9, value, value, 0.9]
            state = self.extract(sm)['assist']
            self.assertEqual((state['leftGlow'], state['rightGlow']), ('none', 'none'))

    def test_current_time_comes_from_vehicle(self):
        with patch('time.time', return_value=1793520000), patch('time.localtime', return_value=SimpleNamespace(tm_gmtoff=28800)):
            result = self.extract(SubMaster())
        self.assertEqual(result['currentTimeSeconds'], 1793520000)
        self.assertEqual(result['utcOffsetSeconds'], 28800)
