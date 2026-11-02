const {test} = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
test('preview front-car controls update data and illustration while retaining distance bars', () => {
  const html = fs.readFileSync(path.join(__dirname, 'preview-drive-status.html'),'utf8');
  const script = /<script>([\s\S]*?)<\/script>/.exec(html)[1];
  const nodes = Object.fromEntries(['connected','lane','lead','long','leftLane','rightLane','leftBlind','rightBlind',
    'controls','preview-car','preview-lead-state','mode'].map(key=>[key,{checked:true,hidden:false,textContent:'',value:'standard',addEventListener(){}}]));
  const context = {window:{},document:{getElementById:id=>nodes[id]},setInterval(){},Date};
  context.window = context;
  vm.runInNewContext(script,context);
  context.setLeadDetected(false);
  assert.equal(context.window.sm.driveStatus.assist.leadDetected,false);
  assert.equal(context.window.sm.driveStatus.assist.followBars,2);
  assert.equal(nodes['preview-car'].hidden,true);
  assert.ok(nodes['preview-lead-state'].textContent.includes('未偵測前車'));
  context.setLeadDetected(true);
  assert.equal(context.window.sm.driveStatus.assist.leadDetected,true);
  assert.equal(nodes['preview-car'].hidden,false);
  nodes.lane.checked=false;context.publish();
  assert.ok(nodes['preview-lead-state'].textContent.includes('ALKA 關閉'));
});
