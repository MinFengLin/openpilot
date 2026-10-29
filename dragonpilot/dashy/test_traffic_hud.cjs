const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const { test } = require('node:test');
const sandbox = { window: {} };
vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'web/dist/js/traffic-hud.js'), 'utf8'), sandbox);
const { getDisplay } = sandbox.window.DashyTraffic;
const { getScrollMetrics } = sandbox.window.DashyTraffic;
const now = 10000;
function state() {
  return { recv_time: { tdx: now / 1000 }, selfdriveState: { alertSize: 'none' }, tdx: {
    nextSpeed: 65, displayNextSpeed: '65', speedUnit: 'km/h', congestion: 'slow',
    events: [{ description: '事故', isCurrentSection: false }],
  } };
}
test('speed and event formatting', () => {
  const display = getDisplay(state(), now);
  assert.equal(display.speed, '前方車流 65 km/h · 緩慢');
  assert.equal(display.event, '前方：事故');
});
test('missing, expired and cleared data hide immediately', () => {
  assert.equal(getDisplay({}, now), null);
  assert.equal(getDisplay(state(), now + 4001), null);
  const sm = state(); sm.tdx = null;
  assert.equal(getDisplay(sm, now), null);
});
test('empty events clear the event line while keeping published speed', () => {
  const sm = state(); sm.tdx.events = [];
  assert.equal(getDisplay(sm, now).event, '');
  assert.equal(getDisplay(sm, now).speed, '前方車流 65 km/h · 緩慢');
});
test('unknown speed keeps events; empty traffic hides', () => {
  const sm = state(); sm.tdx.nextSpeed = -1; sm.tdx.displayNextSpeed = null;
  assert.equal(getDisplay(sm, now).speed, '');
  assert.equal(getDisplay(sm, now).event, '前方：事故');
  sm.tdx.events = [];
  assert.equal(getDisplay(sm, now), null);
});
test('preserves event order and duplicates from tdxd', () => {
  const sm = state(); sm.tdx.events = [
    { description: '<img onerror=alert(1)>', isCurrentSection: true },
    { description: '<img onerror=alert(1)>', isCurrentSection: true },
  ];
  assert.equal(getDisplay(sm, now).event, '目前路段：<img onerror=alert(1)> ／ 目前路段：<img onerror=alert(1)>');
});
test('overflow scrolls the full hidden width at a fixed speed', () => {
  const metrics = getScrollMetrics(1000, 300);
  assert.equal(metrics.overflow, 700);
  assert.equal(metrics.duration, 20);
});
test('fitting and short text do not produce negative scroll', () => {
  assert.equal(getScrollMetrics(100, 300).overflow, 0);
  assert.equal(getScrollMetrics(300, 300).overflow, 0);
  assert.equal(getScrollMetrics(320, 300).duration, 2);
});

test('DOM clears cancelled events and restarts only when the event changes', () => {
  class Element {
    constructor() {
      this.children = []; this.textContent = ''; this.hidden = false; this.dataset = {};
      this.clientWidth = 100; this.reflows = 0;
      const classes = new Set();
      this.classList = {
        remove: name => classes.delete(name),
        toggle: (name, enabled) => enabled ? classes.add(name) : classes.delete(name),
        contains: name => classes.has(name),
      };
      this.style = { setProperty() {}, removeProperty() {} };
    }
    append(...children) { this.children.push(...children); }
    appendChild(child) { this.children.push(child); }
    getClientRects() { return [{}]; }
    // Some inline/layout states report zero scrollWidth even though text is rendered wider.
    get scrollWidth() { return 0; }
    getBoundingClientRect() { return {width: this.textContent.length * 40}; }
    get offsetWidth() { this.reflows++; return this.getBoundingClientRect().width; }
  }
  const host = new Element(); const page = new Element();
  const sm = state(); sm.recv_time.tdx = Date.now() / 1000;
  let tick;
  const context = {
    window: { sm, setInterval: callback => { tick = callback; }, addEventListener() {} },
    document: { hidden: false, getElementById: id => id === 'hud-page' ? page : host,
      createElement: () => new Element(), addEventListener() {} },
    getComputedStyle: () => ({ visibility: 'visible' }),
  };
  vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'web/dist/js/traffic-hud.js'), 'utf8'), context);
  const panel = host.children[0]; const eventLine = panel.children[1]; const text = eventLine.children[0];
  assert.equal(text.classList.contains('traffic-hud-scroll'), true);
  const initialReflows = text.reflows;
  tick(); assert.equal(text.reflows, initialReflows);
  sm.tdx.events = [{ description: '施工，請注意', isCurrentSection: true }];
  tick(); assert.equal(text.textContent, '目前路段：施工，請注意');
  assert.equal(text.reflows, initialReflows + 1);
  sm.tdx.events = []; tick();
  assert.equal(eventLine.hidden, true);
  assert.equal(text.textContent, '');
  assert.equal(text.classList.contains('traffic-hud-scroll'), false);
  assert.equal(panel.hidden, false);
  sm.tdx = null; tick();
  assert.equal(panel.hidden, true);
  assert.equal(text.textContent, '');
  // A disconnected socket leaves the last payload in memory; its receive time must expire it.
  sm.tdx = state().tdx;
  sm.recv_time.tdx = Date.now() / 1000;
  tick();
  assert.equal(panel.hidden, false);
  sm.recv_time.tdx = (Date.now() - 5000) / 1000;
  tick();
  assert.equal(panel.hidden, true);
  assert.equal(text.textContent, '');
  assert.equal(text.classList.contains('traffic-hud-scroll'), false);
});
