"""Shared display-only mapd reader for the device HUD and Dashy."""
import math
import os
import time

from dragonpilot.mapd import region_key


def format_road_info(speed_ms, name):
    try:
        speed = float(speed_ms or 0)
    except (ValueError, TypeError):
        speed = 0
    kph = speed * 3.6 if math.isfinite(speed) and 0 < speed * 3.6 <= 200 else None
    return {'roadName': str(name or ''), 'speedLimitKph': kph} if name or kph else None



def read_road_info(params, mem, sm):
    """Return current road data, or None when any display prerequisite expires."""
    try:
        installed = params.get('dp_mapd_installed_regions') or {}
        selection = region_key(params.get('dp_mapd_country') or 'TW', params.get('dp_mapd_state') or '')
        if (not params.get_bool('dp_dashy_road_info') or selection not in installed
                or not mem.get_bool('dp_mapd_active')):
            return None
        gps = sm['liveGPS']
        if (not sm.seen['liveGPS'] or not sm.valid['liveGPS'] or not gps.gpsOK
                or not sm['deviceState'].started or not 0 <= time.monotonic() - sm.recv_time['liveGPS'] < 3):
            return None
        if not all(0 <= time.time() - os.stat(mem.get_param_path(key)).st_mtime < 5
                   for key in ('MapSpeedLimit', 'RoadName')):
            return None
        result = format_road_info(mem.get('MapSpeedLimit'), mem.get('RoadName'))
        # mapd supplies the distance along the upcoming ways, in meters.
        # An absent/stale preview must not hide the current road information.
        if result and result['speedLimitKph']:
            try:
                age = time.time() - os.stat(mem.get_param_path('NextMapSpeedLimit')).st_mtime
                ahead = mem.get('NextMapSpeedLimit') or {}
                speed = float(ahead.get('speedlimit', 0)) * 3.6
                distance = float(ahead.get('distance', -1))
                if (0 <= age < 5 and math.isfinite(speed) and 0 < speed <= 200
                        and math.isfinite(distance) and distance >= 0
                        and not math.isclose(speed, result['speedLimitKph'])):
                    result['aheadSpeedLimitKph'] = speed
                    result['aheadDistanceM'] = distance
            except (OSError, ValueError, TypeError, AttributeError):
                pass
        return result
    except (OSError, ValueError, TypeError):
        return None
