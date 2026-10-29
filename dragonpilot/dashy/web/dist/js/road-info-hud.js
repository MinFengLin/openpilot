/* Display-only road information supplied by Dashy's shared server worker. */
(() => {
  'use strict';
  function getRoadDisplay(sm, now = Date.now()) {
    const info = sm?.roadInfo;
    const received = Number(sm?.recv_time?.roadInfo) * 1000;
    if (!info || !Number.isFinite(received) || received > now || now - received > 4000) return null;
    const kph = info.speedLimitKph;
    const imperial = sm?.carState?.speedUnit === 'mph';
    const limit = Number.isFinite(kph) && kph > 0 && kph <= 200
      ? String(Math.round(imperial ? kph / 1.609344 : kph)) : '';
    const name = String(info.roadName || '');
    return limit || name ? { limit, name, unit: imperial ? 'mph' : 'km/h' } : null;
  }
  window.DashyRoadInfo = { getRoadDisplay };
  if (typeof document === 'undefined') return;
  const host = document.getElementById('hud-page-content');
  if (!host) return;
  const panel = document.createElement('div');
  panel.id = 'road-info-hud';
  panel.hidden = true;
  const sign = document.createElement('div');
  sign.className = 'road-info-limit';
  const name = document.createElement('div');
  name.className = 'road-info-name';
  panel.append(sign, name);
  host.appendChild(panel);
  function update() {
    const page = document.getElementById('hud-page');
    const data = !document.hidden && page?.getClientRects().length
      && getComputedStyle(page).visibility !== 'hidden' ? getRoadDisplay(window.sm) : null;
    panel.hidden = !data;
    sign.textContent = data?.limit || '';
    sign.hidden = !data?.limit;
    sign.title = data ? `限速 ${data.limit} ${data.unit}` : '';
    name.textContent = data?.name || '';
    name.hidden = !data?.name;
    name.title = data?.name || '';
  }
  const timer = window.setInterval(update, 250);
  document.addEventListener('visibilitychange', update);
  window.addEventListener('pagehide', event => {
    if (event.persisted) return;
    window.clearInterval(timer);
    document.removeEventListener('visibilitychange', update);
  }, { once: true });
  update();
})();
