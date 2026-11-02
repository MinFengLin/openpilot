const {test} = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const context = {window: {}};
vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'web/dist/js/hud-layout.js'), 'utf8'), context);
const {getLayout} = context.window.DashyHudLayout;
test('portrait, landscape and short screens keep TDX below both corner cards', () => {
  for (const [w,h,corner,traffic] of [[320,568,180,58],[375,667,190,50],[667,300,165,44],[568,240,180,44],[320,180,180,44]]) {
    const result = getLayout(w,h,corner,traffic);
    assert.equal(result.compact, true);
    assert.ok(result.top >= 8 + corner * result.scale + 8 - 0.01);
    assert.equal(result.scale, 1);
  }
});
test('large screens keep original layout; hidden traffic does not shrink corners', () => {
  assert.equal(getLayout(1280,720,200,50).compact, false);
  assert.equal(getLayout(1280,720,200,50).top, 216);
  assert.equal(getLayout(320,180,200,0).scale, 1);
});
test('layout worker reacts to card height changes and clears inline top on large screens', () => {
  let tick;
  const styles = {};
  const host = {clientWidth: 568, clientHeight: 240, classList: {toggle() {}},
    style: {removeProperty: key => {delete styles[key];}}};
  const road = {hidden: false, offsetHeight: 180, offsetTop: 8, getClientRects: () => [{}]};
  const drive = {hidden: false, offsetHeight: 120, offsetTop: 8, getClientRects: () => [{}]};
  const traffic = {hidden: false, offsetHeight: 44, style: {removeProperty(key) {delete this[key];}}};
  const nodes = {'hud-page-content':host, 'road-info-hud':road,'drive-status':drive,'traffic-hud':traffic};
  const dom = {window: {setInterval: fn => {tick=fn;},addEventListener() {}},
    document: {hidden:false,getElementById:id=>nodes[id],addEventListener() {}}};
  vm.runInNewContext(fs.readFileSync(path.join(__dirname,'web/dist/js/hud-layout.js'),'utf8'),dom);
  assert.equal(traffic.style.top,'196px');
  assert.equal(styles['--hud-corner-scale'], undefined);
  road.hidden=true;tick();assert.equal(traffic.style.top,'136px');
  host.clientWidth=1280;host.clientHeight=720;tick();
  assert.equal(traffic.style.top,undefined);
  assert.equal(styles['--hud-corner-scale'],undefined);
});
