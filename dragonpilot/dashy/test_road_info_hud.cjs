const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const { test } = require('node:test');
const sandbox = { window: {} };
vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'web/dist/js/road-info-hud.js'), 'utf8'), sandbox);
const { getRoadDisplay } = sandbox.window.DashyRoadInfo;
const state = () => ({ roadInfo: { speedLimitKph: 110, roadName: '國道一號' }, recv_time: { roadInfo: 10 } });
test('shows limit and road name', () => {
  const result = getRoadDisplay(state(), 10000);
  assert.equal(result.limit, '110'); assert.equal(result.name, '國道一號');
});
test('unknown limit still shows known road name', () => {
  const sm = state(); sm.roadInfo.speedLimitKph = null;
  assert.equal(getRoadDisplay(sm, 10000).limit, '');
});
test('cleared or expired payload hides display', () => {
  assert.equal(getRoadDisplay(state(), 14001), null);
  assert.equal(getRoadDisplay({ roadInfo: null }, 10000), null);
});
test('uses mph when HUD speed unit is mph', () => {
  const sm = state(); sm.carState = { speedUnit: 'mph' };
  assert.equal(getRoadDisplay(sm, 10000).limit, '68');
});

test('ahead limit shows route distance in metric and imperial', () => {
  const sm = state();
  sm.roadInfo.aheadSpeedLimitKph = 80; sm.roadInfo.aheadDistanceM = 300;
  assert.equal(getRoadDisplay(sm, 10000).ahead, 'AHEAD · 80 km/h · 300 m');
  sm.carState = { speedUnit: 'mph' };
  assert.equal(getRoadDisplay(sm, 10000).ahead, 'AHEAD · 50 mph · 0.2 mi');
});
test('cancelled, same, invalid and expired previews disappear', () => {
  const sm = state();
  for (const [limit, distance] of [[110, 300], [80, -1], [NaN, 300], [80, Infinity], [undefined, undefined]]) {
    sm.roadInfo.aheadSpeedLimitKph = limit; sm.roadInfo.aheadDistanceM = distance;
    assert.equal(getRoadDisplay(sm, 10000).ahead, '');
  }
  sm.roadInfo.aheadSpeedLimitKph = 80; sm.roadInfo.aheadDistanceM = 300;
  assert.equal(getRoadDisplay(sm, 14001), null);
});
