/* Boot elapsed time and read-only vehicle assist graphic. */
(() => {
  'use strict';
  function getDisplay(sm, now = Date.now()) {
    const status = sm?.driveStatus;
    const received = Number(sm?.recv_time?.driveStatus) * 1000;
    if (!status || !Number.isFinite(received) || received > now || now - received > 4000
        || !Number.isFinite(status.uptimeSeconds) || status.uptimeSeconds < 0) return null;
    const elapsed = Math.floor(status.uptimeSeconds + (now - received) / 1000);
    const pad = value => String(value).padStart(2, '0');
    const timer = `${pad(Math.floor(elapsed / 3600))}:${pad(Math.floor(elapsed / 60) % 60)}:${pad(elapsed % 60)}`;
    const assist = status.assist;
    const badges = [];
    if (assist && (assist.laneCenteringEnabled === true || assist.longitudinalActive === true)) {
      const centered = assist.laneCenteringEnabled === true;
      badges.push({ icon: 'lane', leftGlow: assist.leftGlow || 'none', rightGlow: assist.rightGlow || 'none', state: centered ? 'enabled' : 'off',
        text: `置中${centered ? '開' : '關'}`, title: '兩側車道線依車道置中開關亮起' });
      const controlling = assist.longitudinalActive === true;
      const bars = controlling && Number.isInteger(assist.followBars) && assist.followBars >= 1 && assist.followBars <= 3
        ? assist.followBars : 0;
      const mode = ['', '積極', '標準', '輕鬆'][bars];
      badges.push({ icon: 'follow', bars, leadDetected: controlling && assist.leadDetected === true,
        state: controlling ? 'enabled' : 'off', text: mode || '縱向未控制或距離未知',
        title: bars ? `縱向控制中；實際距離指令 ${bars} 條（${mode}）；前車位置隨距離模式示意，非量測距離`
          : '縱向控制未啟用或距離訊號未知，不顯示距離線' });
    }
    let currentTime = '—';
    if (Number.isFinite(status.currentTimeSeconds) && Number.isFinite(status.utcOffsetSeconds)) {
      const clock = new Date((status.currentTimeSeconds + status.utcOffsetSeconds) * 1000 + now - received);
      currentTime = `${pad(clock.getUTCHours())}:${pad(clock.getUTCMinutes())}:${pad(clock.getUTCSeconds())}`;
    }
    return { timer, currentTime, badges };
  }
  window.DashyDriveStatus = { getDisplay };
  if (typeof document === 'undefined') return;
  const host = document.getElementById('hud-page-content');
  if (!host) return;
  const panel = document.createElement('div');
  panel.id = 'drive-status'; panel.hidden = true;
  const clock = document.createElement('div'); clock.className = 'drive-status-clock';
  clock.setAttribute('aria-label', '車機目前時間');
  const timer = document.createElement('div'); timer.className = 'drive-status-time';
  timer.setAttribute('aria-label', '行駛時間，從本次車機開機起算');
  const row = document.createElement('div'); row.className = 'drive-status-badges';
  const times = document.createElement('div'); times.className = 'drive-status-times';
  times.append(clock, timer);
  panel.append(times, row); host.appendChild(panel);
  const svgNS = 'http://www.w3.org/2000/svg';
  function makeCluster(badges) {
    const [lane, follow] = badges;
    const item = document.createElement('button');
    item.type = 'button'; item.className = 'drive-status-cluster';
    const description = badges.map(b => `${b.text}。${b.title}`).join('；');
    item.title = description; item.setAttribute('aria-label', description);
    item.setAttribute('aria-expanded', 'false');
    const icon = document.createElementNS(svgNS, 'svg');
    icon.setAttribute('viewBox', '0 0 120 82'); icon.setAttribute('aria-hidden', 'true');
    const draw = (d, className, state, opacity = '1', transform = '') => {
      const path = document.createElementNS(svgNS, 'path');
      path.setAttribute('d', d); path.setAttribute('class', className);
      if (transform) path.setAttribute('transform', transform);
      path.setAttribute('data-state', state); path.setAttribute('opacity', opacity);
      icon.appendChild(path);
    };
    // Lane lines follow the centering switch; distance display requires live longitudinal control.
    draw('M42 24 12 76 22 76 47 24Z M73 24 98 76 108 76 78 24Z',
      'assist-lanes', lane.state);
    for (let i = 0; i < 3; i++) {
      const y = 40 + i * 12; const inset = 39 - i * 7;
      draw(`M${inset} ${y}H${120 - inset}L${124 - inset} ${y + 6}H${inset - 4}Z`,
        'assist-distance', follow.state, i >= 3 - follow.bars ? '1' : '0');
    }
    const steps = follow.bars ? 3 - follow.bars : 0;
    const carTransform = `translate(60 ${steps * 12}) scale(${1 + steps * 0.06}) translate(-60 0)`;
    const carOpacity = follow.leadDetected && follow.bars ? '1' : '0';
    draw('M48 15 51 6H69L72 15 76 18V29H44V18Z', 'assist-car', follow.state, carOpacity, carTransform);
    draw('M51 15 54 9H66L69 15Z', 'assist-window', follow.state, carOpacity, carTransform);
    draw('M48 28V33M72 28V33', 'assist-wheels', follow.state, carOpacity, carTransform);
    const defs = document.createElementNS(svgNS, 'defs');
    for (const side of ['left', 'right']) {
      const state = lane[`${side}Glow`];
      const color = state === 'green' ? '#40f478' : '#29cfff';
      const gradient = document.createElementNS(svgNS, 'linearGradient');
      gradient.setAttribute('id', `assist-glow-${side}`);
      gradient.setAttribute('x1', side === 'left' ? '0%' : '100%');
      gradient.setAttribute('x2', side === 'left' ? '100%' : '0%');
      for (const [offset, opacity] of [['0%', '0'], ['100%', '0.85']]) {
        const stop = document.createElementNS(svgNS, 'stop');
        stop.setAttribute('offset', offset); stop.setAttribute('stop-color', color);
        stop.setAttribute('stop-opacity', opacity); gradient.appendChild(stop);
      }
      defs.appendChild(gradient);
    }
    icon.appendChild(defs);
    for (const side of ['left', 'right']) {
      const glow = document.createElementNS(svgNS, 'path');
      glow.setAttribute('d', side === 'left' ? 'M36 24 0 76H12L42 24Z' : 'M78 24 108 76H120L84 24Z');
      glow.setAttribute('fill', `url(#assist-glow-${side})`);
      glow.setAttribute('data-side', side);
      glow.setAttribute('data-glow', lane[`${side}Glow`]);
      glow.setAttribute('opacity', ['blue', 'green'].includes(lane[`${side}Glow`]) ? '1' : '0');
      icon.appendChild(glow);
    }
    item.appendChild(icon);
    const detail = document.createElement('div'); detail.className = 'drive-status-detail';
    detail.hidden = true; detail.textContent = description;
    item.addEventListener('click', event => {
      event.stopPropagation(); detail.hidden = !detail.hidden;
      item.setAttribute('aria-expanded', String(!detail.hidden));
    });
    const group = document.createElement('div'); group.append(item, detail);
    return group;
  }
  let lastBadges = '';
  function update() {
    const page = document.getElementById('hud-page');
    const display = !document.hidden && page?.getClientRects().length
      && getComputedStyle(page).visibility !== 'hidden' ? getDisplay(window.sm) : null;
    panel.hidden = !display;
    clock.textContent = display ? display.currentTime : '';
    timer.textContent = display ? display.timer : '';
    const badges = display?.badges || [];
    const key = JSON.stringify(badges);
    if (key !== lastBadges) { row.replaceChildren(...(badges.length ? [makeCluster(badges)] : [])); lastBadges = key; }
    row.hidden = badges.length === 0;
  }
  const interval = window.setInterval(update, 250);
  document.addEventListener('visibilitychange', update);
  window.addEventListener('pagehide', event => {
    if (event.persisted) return;
    window.clearInterval(interval); document.removeEventListener('visibilitychange', update);
  }, { once: true });
  update();
})();
