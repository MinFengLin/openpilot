"""Manage the SP mapd engine and feed it liveGPS. Never changes driving targets."""
import hashlib
import json
import math
import os
import platform
import shutil
import signal
import subprocess
import time

from dragonpilot.mapd import BINARY, ROOT, SHA256, URL, download_locations, region_key, progress_complete


def install(offroad):
    import requests
    if BINARY.exists() and hashlib.sha256(BINARY.read_bytes()).hexdigest() == SHA256:
        return
    if not offroad():
        raise RuntimeError('請先停車並進入 Offroad')
    BINARY.parent.mkdir(parents=True, exist_ok=True)
    temp = BINARY.with_suffix('.tmp')
    try:
        digest = hashlib.sha256()
        size = 0
        with requests.get(URL, stream=True, timeout=(10, 30)) as response:
            response.raise_for_status()
            with temp.open('wb') as target:
                for chunk in response.iter_content(256 * 1024):
                    if not offroad():
                        raise RuntimeError('已上路，停止安裝')
                    size += len(chunk)
                    if size > 20 * 1024 * 1024:
                        raise RuntimeError('mapd 檔案大小異常')
                    digest.update(chunk)
                    target.write(chunk)
                target.flush()
                os.fsync(target.fileno())
        if digest.hexdigest() != SHA256:
            raise RuntimeError('mapd SHA-256 校驗失敗')
        temp.chmod(0o755)
        temp.replace(BINARY)
    finally:
        temp.unlink(missing_ok=True)


def stop(child):
    if child is not None and child.poll() is None:
        child.terminate()
        try:
            child.wait(timeout=5)
        except subprocess.TimeoutExpired:
            child.kill()
            child.wait()


def main():
    from cereal import messaging
    from openpilot.common.params import Params
    from openpilot.common.swaglog import cloudlog
    params = Params()
    mem = Params('/dev/shm/params')
    sm = messaging.SubMaster(['liveGPS', 'deviceState'], ignore_alive=['liveGPS'])
    child = None
    pending = None
    params.put_bool('dp_mapd_busy', False, block=True)
    started_at = 0.0
    active_region = None
    shutdown = False

    def on_signal(*_):
        nonlocal shutdown
        shutdown = True
    signal.signal(signal.SIGTERM, on_signal)
    signal.signal(signal.SIGINT, on_signal)

    def status(message):
        if params.get('dp_mapd_status') != message:
            params.put('dp_mapd_status', message, block=True)

    def offroad():
        return params.get_bool('IsOffroad')

    try:
        while not shutdown:
            try:
                sm.update(0)
                country = params.get('dp_mapd_country') or 'TW'
                state = params.get('dp_mapd_state') or ''
                selection = region_key(country, state)
                installed = params.get('dp_mapd_installed_regions') or {}
                request = params.get('dp_mapd_request') or ''
                if request:
                    params.remove('dp_mapd_request')
                    if not offroad():
                        status('請先停車並進入 Offroad')
                    elif pending:
                        status('正在下載，請稍候')
                    elif request == 'delete':
                        stop(child)
                        child = None
                        mem.put_bool('dp_mapd_active', False, block=True)
                        target = (ROOT / 'offline').resolve()
                        if target != ROOT.resolve() / 'offline' or target.is_symlink():
                            raise RuntimeError('無效的地圖資料路徑')
                        if target.exists():
                            shutil.rmtree(target)
                        params.put('dp_mapd_installed_regions', {}, block=True)
                        status('已刪除下載圖資')
                    elif request == 'download':
                        params.put_bool('dp_mapd_busy', True, block=True)
                        locations = download_locations(country, state)
                        if platform.system() != 'Linux' or platform.machine() != 'aarch64':
                            raise RuntimeError('SP mapd 二進位僅支援 Linux ARM64 車機')
                        ROOT.mkdir(parents=True, exist_ok=True)
                        if shutil.disk_usage(ROOT).free < 2 * 1024**3:
                            raise RuntimeError('可用空間不足，請至少保留 2 GB')
                        status('安裝及驗證 mapd…')
                        install(offroad)
                        stop(child)
                        child = None
                        mem.put_bool('dp_mapd_active', False, block=True)
                        for name in ('MapSpeedLimit', 'RoadName', 'OSMDownloadLocations', 'MapTargetVelocities'):
                            mem.remove(name)
                        params.remove('OSMDownloadProgress')
                        installed.pop(selection, None)
                        params.put('dp_mapd_installed_regions', installed, block=True)
                        pending = {'key': selection, 'locations': locations, 'triggered': False}
                        status('啟動下載…')

                if pending and not offroad():
                    stop(child)
                    child = None
                    pending = None
                    mem.remove('OSMDownloadLocations')
                    status('已上路，下載中止；停車後可重新下載')

                enabled = params.get_bool('dp_dashy_road_info') and selection in installed
                if not pending and active_region != selection:
                    stop(child)
                    child = None
                    active_region = selection
                if not pending and not enabled:
                    stop(child)
                    child = None
                if (pending or enabled) and (child is None or child.poll() is not None):
                    if BINARY.exists() and platform.machine() == 'aarch64':
                        mem.remove('MapSpeedLimit')
                        mem.remove('RoadName')
                        mem.remove('MapTargetVelocities')
                        child = subprocess.Popen([str(BINARY)], cwd=ROOT,
                                                 env={**os.environ, 'GOMAXPROCS': '2'})
                        started_at = time.monotonic()
                        if pending:
                            pending['triggered'] = False
                    elif not pending:
                        status('請先下載／更新圖資以安裝 mapd')

                alive = child is not None and child.poll() is None
                mem.put_bool('dp_mapd_active', alive and not pending and enabled)
                if pending and alive:
                    # ResetParams runs at startup: wait for its output marker before sending the request.
                    if not pending['triggered'] and os.path.exists(mem.get_param_path('MapTargetVelocities')):
                        mem.put('OSMDownloadLocations', pending['locations'], block=True)
                        pending['triggered'] = True
                    progress = params.get('OSMDownloadProgress') or {}
                    total = progress.get('total_files', 0)
                    done = progress.get('downloaded_files', 0)
                    if pending['triggered'] and not mem.get('OSMDownloadLocations'):
                        if progress_complete(progress):
                            installed[pending['key']] = int(time.time())
                            params.put('dp_mapd_installed_regions', installed, block=True)
                            status(f'下載完成：{pending["key"]}（{done}/{total}）')
                        else:
                            status(f'下載未完成（{done}/{total}），請重新下載')
                        pending = None
                    else:
                        status(f'下載中：{done}/{total}' if total else '下載準備中…')
                    if pending and not pending['triggered'] and time.monotonic() - started_at > 15:
                        raise RuntimeError('mapd 啟動逾時')

                if alive and not pending:
                    gps = sm['liveGPS']
                    if (sm.seen['liveGPS'] and sm.valid['liveGPS'] and gps.gpsOK
                            and sm['deviceState'].started and time.monotonic() - sm.recv_time['liveGPS'] < 3
                            and all(math.isfinite(value) for value in (gps.latitude, gps.longitude, gps.bearingDeg))
                            and -90 <= gps.latitude <= 90 and -180 <= gps.longitude <= 180):
                        mem.put('LastGPSPosition', json.dumps({'latitude': gps.latitude, 'longitude': gps.longitude,
                                                             'bearing': gps.bearingDeg}), block=True)
                    else:
                        mem.put('LastGPSPosition', '{}', block=True)
                if request == 'delete':
                    active_region = selection
                params.put_bool('dp_mapd_busy', bool(pending))
            except Exception as error:
                stop(child)
                child = None
                pending = None
                mem.put_bool('dp_mapd_active', False)
                params.put_bool('dp_mapd_busy', False)
                status(f'離線地圖錯誤：{type(error).__name__}：{error}')
                cloudlog.warning(f'offline mapd: {type(error).__name__}')
            time.sleep(1)
    finally:
        stop(child)
        mem.put_bool('dp_mapd_active', False, block=True)
        params.put_bool('dp_mapd_busy', False, block=True)


if __name__ == '__main__':
    main()
