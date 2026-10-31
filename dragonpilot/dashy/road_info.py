"""Forward SP mapd's offline road outputs to Dashy; no network or control targets."""
import asyncio
import json
import time

from dragonpilot.mapd.road_info import format_road_info, read_road_info  # format_road_info: backwards-compatible export




async def road_info_loop(app):
    from cereal import messaging
    from openpilot.common.params import Params

    params = Params()
    mem = Params('/dev/shm/params')
    sm = None
    while True:
        result = None
        try:
            if app['ws_clients']:
                if sm is None:
                    sm = messaging.SubMaster(['liveGPS', 'deviceState'], ignore_alive=['liveGPS'])
                sm.update(0)
                result = read_road_info(params, mem, sm)
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
