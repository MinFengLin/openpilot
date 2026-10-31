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
    const aheadKph = info.aheadSpeedLimitKph;
    const distance = info.aheadDistanceM;
    let ahead = '';
    if (limit && Number.isFinite(aheadKph) && aheadKph > 0 && aheadKph <= 200 && aheadKph !== kph
        && Number.isFinite(distance) && distance >= 0) {
      const nextLimit = Math.round(imperial ? aheadKph / 1.609344 : aheadKph);
      let dist;
      if (!imperial) {
        dist = distance < 50 ? 'Near' : distance >= 1000 ? `${(distance / 1000).toFixed(1)} km`
          : `${Math.round(distance / (distance < 200 ? 10 : 100)) * (distance < 200 ? 10 : 100)} m`;
      } else {
        const feet = distance * 3.28084;
        dist = feet < 100 ? 'Near' : feet >= 900 ? `${(distance * 0.000621371).toFixed(1)} mi`
          : `${Math.round(feet / (feet < 500 ? 50 : 100)) * (feet < 500 ? 50 : 100)} ft`;
      }
      ahead = `AHEAD · ${nextLimit} ${imperial ? 'mph' : 'km/h'} · ${dist}`;
    }
    return limit || name ? { limit, name, ahead, unit: imperial ? 'mph' : 'km/h' } : null;
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
  const ahead = document.createElement('div');
  ahead.className = 'road-info-ahead';
  panel.append(sign, ahead, name);
  host.appendChild(panel);
  function update() {
    const page = document.getElementById('hud-page');
    const data = !document.hidden && page?.getClientRects().length
      && getComputedStyle(page).visibility !== 'hidden' ? getRoadDisplay(window.sm) : null;
    panel.hidden = !data;
    sign.textContent = data?.limit || '';
    sign.hidden = !data?.limit;
    sign.title = data ? `限速 ${data.limit} ${data.unit}` : '';
    ahead.textContent = data?.ahead || '';
    ahead.hidden = !data?.ahead;
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
