const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const {test} = require('node:test');
const source = fs.readFileSync(path.join(__dirname, 'web/dist/js/drive-status.js'), 'utf8');
const context = {window: {}};
vm.runInNewContext(source, context);
const {getDisplay} = context.window.DashyDriveStatus;
const state = () => ({recv_time: {driveStatus: 10}, driveStatus: {uptimeSeconds: 3661, assist: {
  laneCenteringEnabled: true, lateralActive: true, stopAndGoAvailable: true, longitudinalActive: true, followMode: 'standard', followBars: 2, leadDetected: true,
}}});
test('device uptime persists across browser initialization and formats large hours', () => {
  assert.equal(getDisplay(state(), 10000).timer, '01:01:01');
  assert.equal(getDisplay(state(), 12000).timer, '01:01:03');
  const sm = state(); sm.driveStatus.uptimeSeconds = 360000;
  assert.equal(getDisplay(sm, 10000).timer, '100:00:00');
});
test('centering switch lights sides independently of lateral control', () => {
  const sm = state(); sm.driveStatus.assist.lateralActive = false;
  assert.equal(getDisplay(sm, 10000).badges[0].state, 'enabled');
  sm.driveStatus.assist.laneCenteringEnabled = false;
  assert.equal(getDisplay(sm, 10000).badges[0].state, 'off');
});
test('distance bars require actual longitudinal control; inactive cluster hides', () => {
  const sm = state(); const t = sm.driveStatus.assist;
  for (let bars = 1; bars <= 3; bars++) {
    t.followBars = bars;
    assert.equal(getDisplay(sm, 10000).badges[1].bars, bars);
  }
  t.longitudinalActive = false;
  assert.equal(getDisplay(sm, 10000).badges[1].bars, 0);
  t.laneCenteringEnabled = false;
  assert.equal(getDisplay(sm, 10000).badges.length, 0);
  t.longitudinalActive = null; t.lateralActive = true;
  assert.equal(getDisplay(sm, 10000).badges.length, 0);
});
test('missing vehicle metadata keeps only the clock; stale, missing and malformed payloads hide', () => {
  const sm = state(); sm.driveStatus.assist = null;
  assert.equal(getDisplay(sm, 10000).badges.length, 0);
  assert.equal(getDisplay(sm, 14001), null);
  assert.equal(getDisplay(sm, 9999), null);
  assert.equal(getDisplay({}, 10000), null);
  sm.driveStatus.uptimeSeconds = -1;
  assert.equal(getDisplay(sm, 10000), null);
});
test('DOM creates SVG badges and clears on a disconnected stream', () => {
  class Element {
    constructor() { this.children = []; this.hidden = false; this.dataset = {}; this.textContent = ''; this.attributes = {}; }
    append(...items) { this.children.push(...items); }
    appendChild(item) { this.children.push(item); }
    replaceChildren(...items) { this.children = items; }
    setAttribute(key, value) { this.attributes[key] = value; }
    addEventListener() {}
    getClientRects() { return [{}]; }
  }
  const host = new Element(); const page = new Element(); const sm = state();
  sm.recv_time.driveStatus = Date.now() / 1000;
  let tick;
  const dom = {window: {sm, setInterval: fn => {tick = fn;}, addEventListener() {}},
    document: {hidden: false, getElementById: id => id === 'hud-page' ? page : host,
      createElement: () => new Element(), createElementNS: () => new Element(), addEventListener() {}},
    getComputedStyle: () => ({visibility: 'visible'})};
  vm.runInNewContext(source, dom);
  const panel = host.children[0]; const row = panel.children[1];
  assert.equal(panel.children[0].className, 'drive-status-times');
  assert.equal(panel.children[0].children.length, 2);
  assert.equal(panel.hidden, false); assert.equal(row.children.length, 1);
  assert.equal(row.children[0].children[0].children[0].children.length, 10);
  assert.equal(row.children[0].children[0].children.length, 1); // SVG only; no visible status labels
  sm.driveStatus.assist.followMode = 'relaxed'; sm.driveStatus.assist.followBars = 3; tick();
  assert.ok(row.children[0].children[1].textContent.includes('輕鬆'));
  assert.equal(row.children[0].children[1].hidden, true);
  const paths = () => row.children[0].children[0].children[0].children;
  assert.deepEqual(paths().slice(1, 4).map(p => p.attributes.opacity), ['1', '1', '1']);
  const far = paths()[4].attributes.transform;
  sm.driveStatus.assist.followBars = 2; tick();
  assert.deepEqual(paths().slice(1, 4).map(p => p.attributes.opacity), ['0', '1', '1']);
  sm.driveStatus.assist.followBars = 1; tick();
  assert.deepEqual(paths().slice(1, 4).map(p => p.attributes.opacity), ['0', '0', '1']);
  assert.notEqual(paths()[4].attributes.transform, far);
  assert.ok(paths()[4].attributes.transform.includes('translate(60 24)'));
  sm.driveStatus.assist.leadDetected = false; tick();
  assert.equal(row.children[0].children[0].children[0].children[4].attributes.opacity, '0');
  sm.driveStatus.assist.leftGlow = 'blue'; sm.driveStatus.assist.rightGlow = 'amber'; tick();
  assert.equal(paths()[8].attributes['data-glow'], 'blue');
  assert.equal(paths()[9].attributes['data-glow'], 'amber');
  assert.equal(paths()[8].attributes.opacity, '1');
  sm.driveStatus.assist.leftGlow = 'none'; tick();
  assert.equal(paths()[8].attributes.opacity, '0');
  sm.driveStatus.assist.laneCenteringEnabled = false;
  sm.driveStatus.assist.longitudinalActive = false; tick();
  assert.equal(row.hidden, true); assert.equal(panel.hidden, false);
  assert.equal(row.children.length, 0);
  sm.recv_time.driveStatus = (Date.now() - 5000) / 1000; tick();
  assert.equal(panel.hidden, true); assert.equal(row.children.length, 0);
});

test('current time follows vehicle clock and offset, independently of phone timezone', () => {
  const sm = state();
  sm.driveStatus.currentTimeSeconds = Date.UTC(2026, 10, 1, 15, 59, 59) / 1000;
  sm.driveStatus.utcOffsetSeconds = 8 * 3600;
  assert.equal(getDisplay(sm, 10000).currentTime, '23:59:59');
  assert.equal(getDisplay(sm, 11000).currentTime, '00:00:00');
  assert.equal(getDisplay(sm, 11000).timer, '01:01:02');
});
