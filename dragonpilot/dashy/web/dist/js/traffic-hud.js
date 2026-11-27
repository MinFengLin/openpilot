/* National freeway information. Display only; does not change driving targets. */
(() => {
  'use strict';
  const STALE_MS = 4000;
  const MIN_DROP_KPH = 30;
  const MAX_DROP_AHEAD_KPH = 60;
  const congestionLabels = { smooth: '順暢', slow: '緩慢', congested: '壅塞' };

  function formatLaneAge(collectedAt, now) {
    const seconds = Math.floor(now / 1000 - collectedAt);
    if (!Number.isFinite(seconds) || seconds < 0) return '';
    const minutes = Math.floor(seconds / 60);
    const detail = minutes ? `${minutes} 分 ${seconds % 60} 秒` : `${seconds} 秒`;
    return `收集於 ${detail}前`;
  }

  function getDisplay(sm, now = Date.now()) {
    const traffic = sm?.tdx;
    const received = Number(sm?.recv_time?.tdx) * 1000;
    if (!traffic || !Number.isFinite(received) || received > now || now - received > STALE_MS) return null;

    const hasSpeed = Number.isFinite(traffic.nextSpeed) && traffic.nextSpeed > 0 && traffic.nextSpeed <= 150
      && traffic.displayNextSpeed != null;
    const speed = hasSpeed ? `前方車流 ${traffic.displayNextSpeed} ${traffic.speedUnit}` : '';
    const congestion = hasSpeed ? congestionLabels[traffic.congestion] || '' : '';
    const hasSpeedDrop = hasSpeed && traffic.nextSpeed <= MAX_DROP_AHEAD_KPH
      && Number.isFinite(traffic.speed) && traffic.speed <= 150
      && traffic.speed - traffic.nextSpeed >= MIN_DROP_KPH && traffic.displayCurrentSpeed != null
      && traffic.sectionId && traffic.nextSectionId && traffic.sectionId !== traffic.nextSectionId;
    const events = Array.isArray(traffic.events) ? traffic.events : [];
    const descriptions = [];
    let hasAheadEvent = false;
    for (const event of events) {
      const description = String(event.description || '');
      if (!description) continue;
      const text = `${event.isCurrentSection ? '目前路段' : '前方'}：${description}`;
      descriptions.push(text);
      hasAheadEvent ||= !event.isCurrentSection;
    }
    const lanes = Array.isArray(traffic.laneSpeeds)
      ? traffic.laneSpeeds.filter(lane => lane.speed != null) : [];
    const stationPosition = traffic.laneStationPassed ? '已通過測站' : '前方';
    const hasLanes = Number.isFinite(traffic.laneDistanceM) && traffic.laneDistanceM >= 0
      && traffic.laneCollectedAt > 0 && lanes.length > 0;
    if (!speed && !descriptions.length && !hasLanes) return null;
    return {
      speed: [speed, congestion].filter(Boolean).join(' · '),
      speedValue: hasSpeed ? traffic.displayNextSpeed : '',
      speedUnit: traffic.speedUnit,
      speedStatus: congestion,
      event: descriptions.join(' ／ '),
      hasAheadEvent,
      speedDrop: hasSpeedDrop ? `前方車流驟降 ${traffic.displayCurrentSpeed} → ${traffic.displayNextSpeed} ${traffic.speedUnit}` : '',
      lanes: hasLanes ? lanes : [],
      laneRoadLabel: traffic.laneRoadLabel || '',
      laneAge: hasLanes ? formatLaneAge(traffic.laneCollectedAt, now) : '',
      laneCaption: hasLanes ? `車道（左 → 右） · ${traffic.speedUnit}` : '',
      distance: hasLanes && traffic.laneDistanceM > 0 ? (traffic.laneDistanceM < 1000
        ? `${stationPosition} ${traffic.laneDistanceM} m` : `${stationPosition} ${(traffic.laneDistanceM / 1000).toFixed(1)} km`) : '',
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
  const speedLabel = document.createElement('span');
  speedLabel.textContent = '前方車流';
  const speedValue = document.createElement('strong');
  const speedUnit = document.createElement('span');
  const speedStatus = document.createElement('span');
  speedStatus.className = 'traffic-hud-status';
  speedLine.append(speedLabel, speedValue, speedUnit, speedStatus);
  const eventLine = document.createElement('div');
  eventLine.className = 'traffic-hud-event';
  const eventText = document.createElement('span');
  eventLine.appendChild(eventText);
  const dropLine = document.createElement('div');
  dropLine.className = 'traffic-hud-warning';
  const lanePanel = document.createElement('div');
  lanePanel.className = 'traffic-hud-lanes';
  const laneHeader = document.createElement('div');
  laneHeader.className = 'traffic-hud-lane-header';
  const laneTitle = document.createElement('span');
  const laneDistance = document.createElement('span');
  laneHeader.append(laneTitle, laneDistance);
  const laneList = document.createElement('ol');
  laneList.className = 'traffic-hud-lane-list';
  const laneRoad = document.createElement('div');
  laneRoad.className = 'traffic-hud-lane-road';
  const laneAge = document.createElement('div');
  laneAge.className = 'traffic-hud-lane-age';
  lanePanel.append(laneHeader, laneList, laneAge);
  panel.append(speedLine, eventLine, dropLine, laneRoad, lanePanel);
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
    const style = getComputedStyle(eventLine);
    const padding = (parseFloat(style.paddingLeft) || 0) + (parseFloat(style.paddingRight) || 0);
    const { overflow, duration } = getScrollMetrics(eventText.scrollWidth, eventLine.clientWidth - padding);
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
    laneDistance.hidden = !display.distance;
    laneAge.textContent = display.laneAge;
    laneAge.hidden = !display.laneAge;
    const key = JSON.stringify(display.lanes);
    if (key === renderedLanes) return;
    renderedLanes = key;
    laneList.textContent = '';
    for (const lane of display.lanes) {
      const item = document.createElement('li');
      const speed = document.createElement('strong');
      speed.textContent = lane.speed;
      item.appendChild(speed);
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
    dropLine.textContent = display?.speedDrop || '';
    dropLine.hidden = !display?.speedDrop;
    laneRoad.textContent = display?.laneRoadLabel || '';
    laneRoad.hidden = !display?.laneRoadLabel;
    updateLanes(display);
    if (!display) {
      clearEvent();
      return;
    }
    panel.dataset.congestion = display.congestion;
    speedValue.textContent = display.speedValue;
    speedUnit.textContent = display.speedUnit;
    speedStatus.textContent = display.speedStatus;
    speedStatus.hidden = !display.speedStatus;
    speedLine.hidden = !display.speed;
    if (!display.event) {
      clearEvent();
      return;
    }
    eventLine.hidden = false;
    eventLine.classList.toggle('traffic-hud-warning', display.hasAheadEvent);
    if (eventText.textContent !== display.event) {
      eventText.classList.remove('traffic-hud-scroll');
      eventText.textContent = display.event;
      // Commit removal before reapplying the class so a new event starts at the beginning.
      void eventText.offsetWidth;
    }
    fitEvent();
  }

  const timer = window.setInterval(update, 250);
  function onPanelResize() {
    const height = panel.offsetHeight;
    if (height > 0) {
      host.style.setProperty('--drive-status-offset', `${height + 16}px`);
    } else {
      host.style.removeProperty('--drive-status-offset');
    }
    fitEvent();
  }

  const observer = typeof ResizeObserver === 'function' ? new ResizeObserver(onPanelResize) : null;
  observer?.observe(panel);
  document.addEventListener('visibilitychange', update);
  window.addEventListener('pagehide', event => {
    if (event.persisted) return;
    window.clearInterval(timer);
    observer?.disconnect();
    host.style.removeProperty('--drive-status-offset');
    document.removeEventListener('visibilitychange', update);
  }, { once: true });
  update();
})();
