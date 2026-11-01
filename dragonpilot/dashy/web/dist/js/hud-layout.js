/* Keep corner information and TDX in separate rows on small HUD viewports. */
(() => {
  'use strict';
  function getLayout(width, height, cornerHeight, trafficHeight, topInset = 8) {
    const compact = width < 760 || height < 420;
    if (!compact) return { compact, scale: 1, top: height * 0.3 };
    const available = Math.max(0, height - topInset - trafficHeight - 16);
    const scale = trafficHeight && cornerHeight ? Math.min(1, available / cornerHeight) : 1;
    const top = trafficHeight ? Math.max(height * 0.3, topInset + cornerHeight * scale + 8) : height * 0.3;
    return { compact, scale, top };
  }
  window.DashyHudLayout = { getLayout };
  if (typeof document === 'undefined') return;
  const host = document.getElementById('hud-page-content');
  if (!host) return;
  function update() {
    if (document.hidden || !host.clientWidth || !host.clientHeight) return;
    const compact = host.clientWidth < 760 || host.clientHeight < 420;
    host.classList.toggle('hud-compact-layout', compact);
    const corners = ['road-info-hud', 'drive-status'].map(id => document.getElementById(id))
      .filter(node => node && !node.hidden && node.getClientRects().length);
    const cornerHeight = Math.max(0, ...corners.map(node => node.offsetHeight));
    const topInset = Math.max(8, ...corners.map(node => node.offsetTop));
    const traffic = document.getElementById('traffic-hud');
    const trafficHeight = traffic && !traffic.hidden ? traffic.offsetHeight : 0;
    const layout = getLayout(host.clientWidth, host.clientHeight, cornerHeight, trafficHeight, topInset);
    host.style.setProperty('--hud-corner-scale', String(layout.scale));
    if (traffic) {
      if (compact) traffic.style.top = `${layout.top}px`;
      else traffic.style.removeProperty('top');
    }
  }
  const timer = window.setInterval(update, 250);
  window.addEventListener('resize', update);
  document.addEventListener('visibilitychange', update);
  window.addEventListener('pagehide', event => {
    if (event.persisted) return;
    window.clearInterval(timer); window.removeEventListener('resize', update);
    document.removeEventListener('visibilitychange', update);
  }, { once: true });
  update();
})();
