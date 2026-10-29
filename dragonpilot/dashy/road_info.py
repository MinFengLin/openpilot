"""Forward SP mapd's offline road outputs to Dashy; no network or control targets."""
import asyncio
import json
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


async def road_info_loop(app):
    from cereal import messaging
    from openpilot.common.params import Params

    params = Params()
    mem = Params('/dev/shm/params')
    sm = None
    while True:
        result = None
        try:
            installed = params.get('dp_mapd_installed_regions') or {}
            selection = region_key(params.get('dp_mapd_country') or 'TW', params.get('dp_mapd_state') or '')
            if (params.get_bool('dp_dashy_road_info') and selection in installed
                    and mem.get_bool('dp_mapd_active') and app['ws_clients']):
                if sm is None:
                    sm = messaging.SubMaster(['liveGPS', 'deviceState'], ignore_alive=['liveGPS'])
                sm.update(0)
                gps = sm['liveGPS']
                if (sm.seen['liveGPS'] and sm.valid['liveGPS'] and gps.gpsOK
                        and sm['deviceState'].started and time.monotonic() - sm.recv_time['liveGPS'] < 3):
                    paths = [mem.get_param_path(key) for key in ('MapSpeedLimit', 'RoadName')]
                    if all(0 <= time.time() - os.stat(path).st_mtime < 5 for path in paths):
                        result = format_road_info(mem.get('MapSpeedLimit'), mem.get('RoadName'))
        except asyncio.CancelledError:
            raise
        except (OSError, ValueError, TypeError):
            result = None
        if time.monotonic() - app.get('dashy_last_received', 0) < 3:
            message = json.dumps({'ts': time.monotonic_ns(), 'roadInfo': result})
            for ws in list(app['ws_clients']):
                try:
                    await asyncio.wait_for(ws.send_str(message), timeout=1)
                except Exception:
                    app['ws_clients'].discard(ws)
        await asyncio.sleep(1)
