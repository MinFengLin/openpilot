"""Loopback preview of the production Dashy assets with fixture telemetry."""
import asyncio
from base64 import b64encode
from pathlib import Path
import time

from aiohttp import web

ROOT = Path(__file__).resolve().parents[1] / 'dragonpilot/dashy/web/dist'
HOST = '127.0.0.1'
PORT = 5088

PREVIEW_CONTROLS = """
<style>
.preview-controls { position: fixed; left: env(safe-area-inset-left, 0px); bottom: env(safe-area-inset-bottom, 0px);
  z-index: 50; margin: 8px; padding: 8px 10px; border-radius: 8px; color: #e7f0f5; background: #14232d;
  font: 14px system-ui, sans-serif; }
.preview-controls select { color: inherit; background: #223644; padding: 4px; }
</style>
<label class="preview-controls">資料回放
  <select id="preview-scenario">
    <option value="taipei">市民大道高架</option>
    <option value="freeway">國一・平面</option>
    <option value="elevated">國一・汐五高架</option>
    <option value="expressway">台64線</option>
    <option value="combined-warning">前方事件＋車流驟降</option>
    <option value="lane-cache">車道資料暫未更新</option>
    <option value="long-event">長事件</option>
    <option value="speed-drop">車流驟降</option>
    <option value="no-traffic">無即時路況</option>
    <option value="no-lead">無前車</option>
    <option value="alert">Alert</option>
  </select>
</label>
<script>
const scenario = document.getElementById('preview-scenario');
scenario.value = '__SCENARIO__';
scenario.addEventListener('change', () => fetch('/preview/scenario/' + scenario.value, {method: 'POST'}));
</script>
"""

SCENARIOS = {
  'taipei': ('市民大道高架道路', '市民大道高架道路', 70, 42, 38, 1200, [52, 41, 28]),
  'freeway': ('國一・平面', '國道1號', 100, 95, 86, 914, [103, 92, 84]),
  'elevated': ('國一・汐五高架', '汐五高架', 90, 89, 87, 420, [102, 105]),
  'expressway': ('台64線', '台64線', 80, 76, 65, 155, [62, 70]),
}
EXTRA_SCENARIOS = {'no-traffic', 'no-lead', 'alert', 'speed-drop', 'combined-warning', 'long-event', 'lane-cache'}
PREVIEW_START = time.time()
X = list(range(2, 162, 5))


def trajectory(offset):
  return {'x': X, 'y': [offset + 0.00015 * x * x for x in X], 'z': [0] * len(X)}


def telemetry(scenario, elapsed):
  label, road, limit, speed, traffic, distance, lanes = SCENARIOS.get(scenario, SCENARIOS['freeway'])
  now = time.time()
  collected = PREVIEW_START - 15 + 60 * int((now - PREVIEW_START) // 60)
  lane_from_cache = scenario == 'lane-cache' and elapsed >= 5
  if scenario == 'lane-cache':
    collected = PREVIEW_START - 15
    if lane_from_cache:
      distance = 0
  if scenario == 'taipei':
    distance = max(0, 125 - int(elapsed * speed / 3.6))
  lead_detected = scenario != 'no-lead'
  alert = scenario == 'alert'
  if scenario in ('speed-drop', 'combined-warning'):
    traffic, lanes = 45, [52, 43, 38]
  events = []
  if scenario in ('freeway', 'combined-warning'):
    events.append({'description': '施工，路肩封閉（示意）', 'isCurrentSection': False})
  elif scenario == 'long-event':
    events.append({'description': '國道一號北向內湖交流道至汐止系統交流道路段，因夜間道路養護施工封閉外側兩個車道，請依現場標誌提前變換車道並減速慢行（示意）',
                   'isCurrentSection': False})
  return {
    'ts': time.time(),
    'carState': {'vEgo': speed / 3.6, 'vEgoCluster': speed / 3.6, 'gearShifter': 'drive',
                 'speedDisplay': str(speed), 'speedClusterDisplay': str(speed), 'setSpeed': limit,
                 'speedUnit': 'km/h', 'cruiseEnabled': True},
    'selfdriveState': {'enabled': True, 'activeOverride': 0, 'experimentalMode': False,
                       'alertSize': 'mid' if alert else 'none', 'alertStatus': 'userPrompt' if alert else 'normal',
                       'alertText1': 'Take Control' if alert else '', 'alertText2': 'Preview' if alert else ''},
    'controlsStateExt': {'alkaActive': True},
    'deviceState': {'started': True, 'deviceType': 'tici', 'thermalStatus': 'green', 'tempDisplay': '53°',
                    'cpuUsagePercent': [12, 17], 'gpuUsagePercent': 8, 'memoryUsagePercent': 32},
    'carParams': {'openpilotLongitudinalControl': True},
    'modelV2': {'position': trajectory(0), 'laneLines': [trajectory(y) for y in (-5.4, -1.8, 1.8, 5.4)],
                'laneLineProbs': [.8, .95, .95, .8], 'roadEdges': [trajectory(-7), trajectory(7)],
                'roadEdgeStds': [.1, .1]},
    'radarState': {'leadOne': {'status': lead_detected, 'dRel': 62, 'yRel': 0.1, 'vRel': -1.5,
                              'distValue': '62.0', 'distUnit': 'm', 'speedValue': str(speed - 5),
                              'speedUnit': 'km/h', 'ttcValue': '2.4', 'ttcUnit': 's'},
                   'leadTwo': {'status': False}},
    'longitudinalPlan': {'allowThrottle': True},
    'liveCalibration': {'rpyCalib': [0, 0, 0], 'calStatus': 'calibrated', 'height': [1.22]},
    'driveStatus': {'assist': {'alkaActive': True, 'longitudinalActive': True, 'followBars': 2,
                               'leadDetected': lead_detected, 'leftGlow': 'blue', 'rightGlow': 'blue'}},
    'roadInfo': {'roadName': road, 'speedLimitKph': limit},
    'tdx': None if scenario == 'no-traffic' else {
      'sectionId': 'preview-current', 'nextSectionId': 'preview-ahead',
      'speed': speed, 'displayCurrentSpeed': str(speed), 'nextSpeed': traffic, 'displayNextSpeed': str(traffic), 'speedUnit': 'km/h',
      'congestion': 'smooth' if traffic >= 80 else 'slow',
      'laneRoadLabel': label, 'laneDistanceM': distance, 'laneCollectedAt': int(collected),
      'laneFromCache': lane_from_cache,
      'laneSpeeds': [{'laneId': i, 'speed': str(value)} for i, value in enumerate(lanes)],
      'events': events,
    },
  }


async def index(request):
  text = (ROOT / 'index.html').read_text()
  scenario = request.query.get('scenario')
  if scenario in SCENARIOS or scenario in EXTRA_SCENARIOS:
    request.app['state']['scenario'] = scenario
  controls = PREVIEW_CONTROLS.replace('__SCENARIO__', request.app['state']['scenario'])
  view = request.query.get('view')
  if view in ('settings', 'map-update'):
    controls += """
<script>
window.addEventListener('load', async () => {
  await window.PanelOverlay.show('settings');
  if (new URLSearchParams(location.search).get('view') === 'map-update') {
    const tab = [...document.querySelectorAll('button')].find(button => button.textContent === '離線道路資料');
    tab?.click();
  }
  document.addEventListener('submit', event => {
    const status = event.target.querySelector('[role="status"]');
    if (!event.target.matches('form') || !event.target.querySelector('button[value="check"]') || !status) return;
    event.preventDefault();
    event.stopImmediatePropagation();
    status.hidden = false;
    status.textContent = '本機預覽，請在車機上儲存或驗證金鑰。';
  }, true);
});
</script>"""

  font = Path(__file__).with_name('dashy-live-preview.ttf')
  if font.is_file():
    encoded = b64encode(font.read_bytes()).decode('ascii')
    controls += (f"<style>@font-face {{font-family: PreviewTC; src:url(data:font/ttf;base64,{encoded});}}"
                 "#traffic-hud, #road-info-hud, .preview-controls, .hud-mono, [role=status] {font-family: 'Geist Mono', PreviewTC, sans-serif;}"
                 "</style>")
  return web.Response(text=text.replace('</body>', controls + '</body>'), content_type='text/html',
                      headers={'Cache-Control': 'no-store'})


async def change_scenario(request):
  scenario = request.match_info['scenario']
  if scenario not in SCENARIOS and scenario not in EXTRA_SCENARIOS:
    raise web.HTTPNotFound()
  request.app['state']['scenario'] = scenario
  return web.json_response({'scenario': scenario})


async def socket(request):
  ws = web.WebSocketResponse()
  await ws.prepare(request)
  previous_scenario = None
  try:
    while not ws.closed:
      scenario = request.app['state']['scenario']
      if scenario != previous_scenario:
        scenario_start = time.monotonic()
        previous_scenario = scenario
      await ws.send_json(telemetry(scenario, time.monotonic() - scenario_start))
      await asyncio.sleep(0.10)
  except (ConnectionError, asyncio.CancelledError):
    pass
  return ws


async def init(_request):
  return web.json_response({'dp_dev_dashy': True, 'isOffroad': False})


async def settings(_request):
  return web.json_response({'settings': []})


async def preview_map_update(request):
  state = request.app['state'].setdefault('map', {
    'country': 'TW', 'state': '', 'enabled': True, 'installed': {'TW'}, 'download': '下載完成：TW（3/3）',
  })
  job = state.get('job')
  if job and time.monotonic() - job[1] >= 1.5:
    action, _, key = state.pop('job')
    if action == 'check_update':
      state['update'] = key
    elif action == 'download':
      state['installed'].add(key)
      state['update'] = None
      state['download'] = f'下載完成：{key}（模擬）'
    else:
      state['installed'].clear()
      state['update'] = None
      state['download'] = '已刪除下載圖資（模擬）'
  if request.method == 'POST' and 'job' not in state:
    payload = await request.json()
    action, value = payload.get('action'), payload.get('value')
    if action in ('country', 'state', 'enabled'):
      if action == 'country':
        state['state'] = ''
      state[action] = value
    elif action in ('check_update', 'download', 'delete'):
      state['job'] = (action, time.monotonic(), payload.get('key'))
  country, province = state['country'], state['state']
  key = f'US:{province}' if country == 'US' and province else country
  installed = key in state['installed']
  active = state.get('job')
  status = ('not_installed' if not installed else 'checking' if active and active[0] == 'check_update'
            else 'available' if state.get('update') == key else 'unchecked')
  download = '下載中…（模擬）' if active and active[0] == 'download' else state['download']
  return web.json_response({
    'key': key, 'country': country, 'state': province, 'enabled': state['enabled'],
    'countries': {'TW': '臺灣（模擬）', 'US': '美國', 'JP': '日本'}, 'states': {'CA': 'California', 'NY': 'New York'},
    'installed': installed, 'downloadStatus': download,
    'status': status, 'offroad': True, 'busy': bool(active),
  })


if __name__ == '__main__':
  app = web.Application()
  app['state'] = {'scenario': 'combined-warning'}
  app.router.add_get('/', index)
  app.router.add_get('/api/init', init)
  app.router.add_get('/api/settings', settings)
  app.router.add_get('/api/models', lambda _: web.json_response({'model_list': []}))
  app.router.add_get('/api/settings/map-update', preview_map_update)
  app.router.add_post('/api/settings/map-update', preview_map_update)
  app.router.add_get('/api/ws', socket)
  app.router.add_post('/preview/scenario/{scenario}', change_scenario)
  for folder in ('css', 'fonts', 'icons', 'js', 'lib', 'pages'):
    app.router.add_static(f'/{folder}/', ROOT / folder)
  web.run_app(app, host=HOST, port=PORT, access_log=None)
