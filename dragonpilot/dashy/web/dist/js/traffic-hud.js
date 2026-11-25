/* National freeway information. Display only; does not change driving targets. */
(() => {
  'use strict';
  const STALE_MS = 4000;
  const congestionLabels = { smooth: '順暢', slow: '緩慢', congested: '壅塞' };

  function getDisplay(sm, now = Date.now()) {
    const traffic = sm?.tdx;
    const received = Number(sm?.recv_time?.tdx) * 1000;
    if (!traffic || !Number.isFinite(received) || received > now || now - received > STALE_MS) return null;

    const hasSpeed = Number.isFinite(traffic.nextSpeed) && traffic.nextSpeed > 0 && traffic.nextSpeed <= 150
      && traffic.displayNextSpeed != null;
    const speed = hasSpeed ? `前方車流 ${traffic.displayNextSpeed} ${traffic.speedUnit}` : '';
    const congestion = hasSpeed ? congestionLabels[traffic.congestion] || '' : '';
    const events = Array.isArray(traffic.events) ? traffic.events : [];
    const descriptions = [];
    for (const event of events) {
      const description = String(event.description || '');
      if (!description) continue;
      const text = `${event.isCurrentSection ? '目前路段' : '前方'}：${description}`;
      descriptions.push(text);
    }
    const lanes = Array.isArray(traffic.laneSpeeds) ? traffic.laneSpeeds : [];
    const hasLanes = Number.isFinite(traffic.laneDistanceM) && traffic.laneDistanceM >= 0
      && traffic.laneCollectedAt > 0 && lanes.some(lane => lane.speed != null);
    if (!speed && !descriptions.length && !hasLanes) return null;
    return {
      speed: [speed, congestion].filter(Boolean).join(' · '),
      event: descriptions.join(' ／ '),
      lanes: hasLanes ? lanes : [],
      laneCaption: hasLanes ? `車道（左 → 右） · ${traffic.speedUnit}` : '',
      distance: hasLanes ? (traffic.laneDistanceM < 1000
        ? `前方 ${traffic.laneDistanceM} m` : `前方 ${(traffic.laneDistanceM / 1000).toFixed(1)} km`) : '',
      congestion: hasSpeed && congestion ? traffic.congestion : 'unknown',
    };
  }

  function getScrollMetrics(textWidth, lineWidth) {
    const overflow = Math.max(0, textWidth - lineWidth);
    return { overflow, duration: Math.max(2, overflow / 35) };
  }

  // Expose pure formatting for diagnostics and tests without the vehicle/UI runtime.
  window.DashyTraffic = { getDisplay, getScrollMetrics };
  if (typeof document === 'undefined') return;

  const host = document.getElementById('hud-page-content');
  if (!host) return;
  const panel = document.createElement('div');
  panel.id = 'traffic-hud';
  panel.hidden = true;
  const speedLine = document.createElement('div');
  speedLine.className = 'traffic-hud-speed';
  const eventLine = document.createElement('div');
  eventLine.className = 'traffic-hud-event';
  const eventText = document.createElement('span');
  eventLine.appendChild(eventText);
  const lanePanel = document.createElement('div');
  lanePanel.className = 'traffic-hud-lanes';
  const laneHeader = document.createElement('div');
  laneHeader.className = 'traffic-hud-lane-header';
  const laneTitle = document.createElement('span');
  const laneDistance = document.createElement('span');
  laneHeader.append(laneTitle, laneDistance);
  const laneList = document.createElement('ol');
  laneList.className = 'traffic-hud-lane-list';
  lanePanel.append(laneHeader, laneList);
  panel.append(speedLine, eventLine, lanePanel);
  host.appendChild(panel);

  function clearEvent() {
    eventLine.hidden = true;
    eventText.classList.remove('traffic-hud-scroll');
    eventText.textContent = '';
    eventText.style.removeProperty('--traffic-scroll');
    eventText.style.removeProperty('--traffic-duration');
  }

  function fitEvent() {
    if (panel.hidden || eventLine.hidden || !eventText.textContent) {
      eventText.classList.remove('traffic-hud-scroll');
      return;
    }
    const { overflow, duration } = getScrollMetrics(eventText.scrollWidth, eventLine.clientWidth);
    eventText.style.setProperty('--traffic-scroll', `${-overflow}px`);
    eventText.style.setProperty('--traffic-duration', `${duration}s`);
    eventText.classList.toggle('traffic-hud-scroll', overflow > 1);
  }

  let renderedLanes = '';
  function updateLanes(display) {
    lanePanel.hidden = !display?.lanes.length;
    if (lanePanel.hidden) return;
    laneTitle.textContent = display.laneCaption;
    laneDistance.textContent = display.distance;
    const key = JSON.stringify(display.lanes);
    if (key === renderedLanes) return;
    renderedLanes = key;
    laneList.textContent = '';
    for (const lane of display.lanes) {
      const item = document.createElement('li');
      const label = document.createElement('span');
      label.textContent = `第 ${lane.laneId + 1} 道`;
      const speed = document.createElement('strong');
      speed.textContent = lane.speed ?? '—';
      item.append(label, speed);
      laneList.appendChild(item);
    }
  }

  function update() {
    const page = document.getElementById('hud-page');
    const alertSize = window.sm?.selfdriveState?.alertSize;
    const alertVisible = Boolean(alertSize && alertSize !== 'none');
    const display = !alertVisible && !document.hidden && page && page.getClientRects().length
      && getComputedStyle(page).visibility !== 'hidden' ? getDisplay(window.sm) : null;
    panel.hidden = !display;
    updateLanes(display);
    if (!display) {
      speedLine.textContent = '';
      clearEvent();
      return;
    }
    const driveStatus = document.getElementById('drive-status');
    if (driveStatus && !driveStatus.hidden && driveStatus.getClientRects().length) {
      const statusBottom = Math.max(driveStatus.getBoundingClientRect().bottom,
        ...Array.from(driveStatus.querySelectorAll('.drive-status-detail:not([hidden])'),
          detail => detail.getBoundingClientRect().bottom));
      panel.style.setProperty('--traffic-hud-top', `${statusBottom - host.getBoundingClientRect().top + 16}px`);
    } else {
      panel.style.removeProperty('--traffic-hud-top');
    }
    panel.dataset.congestion = display.congestion;
    speedLine.textContent = display.speed;
    speedLine.hidden = !display.speed;
    if (!display.event) {
      clearEvent();
      return;
    }
    eventLine.hidden = false;
    if (eventText.textContent !== display.event) {
      eventText.classList.remove('traffic-hud-scroll');
      eventText.textContent = display.event;
      // Commit removal before reapplying the class so a new event starts at the beginning.
      void eventText.offsetWidth;
    }
    fitEvent();
  }

  const timer = window.setInterval(update, 250);
  const observer = typeof ResizeObserver === 'function' ? new ResizeObserver(fitEvent) : null;
  observer?.observe(panel);
  document.addEventListener('visibilitychange', update);
  window.addEventListener('pagehide', event => {
    if (event.persisted) return;
    window.clearInterval(timer);
    observer?.disconnect();
    document.removeEventListener('visibilitychange', update);
  }, { once: true });
  update();
})();
