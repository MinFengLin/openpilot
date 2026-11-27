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
test('lane-only data shows distance and omits missing lanes', () => {
  const sm = state();
  Object.assign(sm.tdx, { nextSpeed: -1, events: [], laneDistanceM: 1800, laneCollectedAt: 1, laneRoadLabel: '國一・汐五高架',
    laneSpeeds: [{ laneId: 0, speed: '92' }, { laneId: 1, speed: null }, { laneId: 2, speed: '0' }] });
  const display = getDisplay(sm, now);
  assert.equal(display.laneCaption, '車速：2 道（左 → 右） · km/h');
  assert.equal(display.distance, '前方 1.8 km');
  Object.assign(sm.tdx, {laneStationPassed: true, laneDistanceM: 300});
  assert.equal(getDisplay(sm, now).distance, '已通過測站 300 m');
  sm.tdx.laneDistanceM = 80000;
  assert.equal(getDisplay(sm, now).distance, '已通過測站 80.0 km');
  assert.equal(display.laneRoadLabel, '國一・汐五高架');
  assert.deepEqual(display.lanes.map(lane => [lane.laneId, lane.speed]), [[0, '92'], [2, '0']]);
  assert.equal(getDisplay(sm, now + 4001), null);
  sm.tdx.laneSpeeds[0].speed = null;
  sm.tdx.laneSpeeds[2].speed = null;
  assert.equal(getDisplay(sm, now), null);
});

test('lane sample age advances and resets when the same station publishes again', () => {
  const sm = state();
  Object.assign(sm.tdx, { laneDistanceM: 1200, laneStationPassed: true,
    laneCollectedAt: 930, laneSpeeds: [{ laneId: 0, speed: '20' }] });
  sm.recv_time.tdx = 1000;
  assert.equal(getDisplay(sm, 1000000).laneAge, '收集於 1 分 10 秒前');
  assert.equal(getDisplay(sm, 1003000).laneAge, '收集於 1 分 13 秒前');
  Object.assign(sm.tdx, {laneDistanceM: 0, laneStationPassed: false});
  const atStation = getDisplay(sm, 1003000);
  assert.equal(atStation.distance, '');
  assert.equal(atStation.lanes.length, 1);
  assert.equal(atStation.laneAge, '收集於 1 分 13 秒前');
  sm.tdx.laneCollectedAt = 1001;
  assert.equal(getDisplay(sm, 1003000).laneAge, '收集於 2 秒前');
});

test('speed drop needs a distinct ahead section and a substantial drop in raw km/h', () => {
  const sm = state();
  Object.assign(sm.tdx, { sectionId: 'current', nextSectionId: 'ahead', speed: 90,
    nextSpeed: 60, displayCurrentSpeed: '56', displayNextSpeed: '37', speedUnit: 'mph' });
  assert.equal(getDisplay(sm, now).speedDrop, '前方車流驟降 56 → 37 mph');
  sm.tdx.nextSectionId = 'current';
  assert.equal(getDisplay(sm, now).speedDrop, '');
  sm.tdx.nextSectionId = 'ahead';
  sm.tdx.speed = 89;
  assert.equal(getDisplay(sm, now).speedDrop, '');
  sm.tdx.speed = 100;
  sm.tdx.nextSpeed = 61;
  assert.equal(getDisplay(sm, now).speedDrop, '');
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
    get scrollWidth() { return this.textContent.length * 40; }
    get offsetWidth() { this.reflows++; return this.scrollWidth; }
  }
  const host = new Element(); const page = new Element();
  const driveStatus = new Element();
  driveStatus.getBoundingClientRect = () => ({ bottom: 80 });
  driveStatus.querySelectorAll = () => [{ getBoundingClientRect: () => ({ bottom: 120 }) }];
  host.getBoundingClientRect = () => ({ top: 10 });
  const sm = state(); sm.recv_time.tdx = Date.now() / 1000;
  let tick;
  const context = {
    window: { sm, setInterval: callback => { tick = callback; }, addEventListener() {} },
    document: { hidden: false, getElementById: id => ({ 'hud-page': page, 'hud-page-content': host, 'drive-status': driveStatus })[id],
      createElement: () => new Element(), addEventListener() {} },
    getComputedStyle: () => ({ visibility: 'visible' }),
  };
  vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'web/dist/js/traffic-hud.js'), 'utf8'), context);
  const panel = host.children[0]; const eventLine = panel.children[1]; const text = eventLine.children[0];
  assert.equal(text.classList.contains('traffic-hud-scroll'), true);
  let offset;
  panel.style.setProperty = (key, value) => { if (key === '--traffic-hud-top') offset = value; };
  panel.style.removeProperty = key => { if (key === '--traffic-hud-top') offset = undefined; };
  tick();
  assert.equal(offset, '126px');
  driveStatus.hidden = true;
  tick();
  assert.equal(offset, undefined);
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
