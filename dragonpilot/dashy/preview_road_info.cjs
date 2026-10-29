/* Local display test only: node dragonpilot/dashy/preview_road_info.cjs */
const http = require('node:http');
const crypto = require('node:crypto');
const fs = require('node:fs/promises');
const path = require('node:path');

function createPreviewServer() {
  const reports = [];
  const sockets = new Set();
  const assets = new Set(['/css/road-info-hud.css', '/js/road-info-hud.js',
    '/css/traffic-hud.css', '/js/traffic-hud.js']);
  const html = `<!doctype html><html lang="zh-Hant"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>離線 mapd HUD 模擬預覽</title>
<link rel="stylesheet" href="/css/road-info-hud.css">
<link rel="stylesheet" href="/css/traffic-hud.css">
<style>
html,body{margin:0;height:100%;background:#14171b;color:white;font:16px sans-serif}
#hud-page,#hud-page-content{position:relative;width:100%;height:100%;overflow:hidden}
.road{position:absolute;bottom:0;left:25%;width:50%;height:60%;background:#213831;clip-path:polygon(45% 0,55% 0,100% 100%,0 100%)}
.speed{position:absolute;top:4%;left:50%;transform:translateX(-50%);text-align:center;font-size:clamp(32px,8vh,90px)}
.speed small{display:block;font-size:18px;color:#aaa}.alka{position:absolute;top:24%;left:50%;transform:translateX(-50%);color:#5fbef0}
#controls{position:absolute;bottom:0;left:0;right:0;z-index:20;background:#111d;padding:8px;font-size:12px}
button{margin:3px;padding:6px}#status{white-space:pre-wrap;color:#bde4ff;margin-top:4px}
</style>
<div id="hud-page"><div id="hud-page-content"><div class="road"></div>
<div class="speed">100<small>km/h</small></div><div class="alka">ALKA</div></div></div>
<div id="controls">模擬資料：只驗證目前 HUD JS/CSS 與 WebSocket，不驗證 mapd 引擎或車機設定畫面。<br>
<button onclick="location.reload()">重新開始自動測試</button>
<button onclick="choose('highway')">國道 110</button><button onclick="choose('city')">市區 50</button>
<button onclick="choose('unknown')">未知限速</button><button onclick="choose('clear')">清除全部</button>
<button onclick="choose('disconnect')">模擬斷線</button><div id="status">準備連線…</div></div>
<script>
window.sm={recv_time:{},carState:{speedUnit:'km/h'}};
const status=document.getElementById('status');
const socket=new WebSocket('ws://'+location.host+'/api/ws');
socket.onmessage=event=>{const data=JSON.parse(event.data);for(const [key,value] of Object.entries(data)){
if(key==='ts')continue;sm[key]=value;sm.recv_time[key]=Date.now()/1000;}};
socket.onerror=()=>{status.textContent+='\\nWebSocket error';};
window.addEventListener('error',event=>{fetch('/report',{method:'POST',body:JSON.stringify({error:event.message})});});
function choose(name){fetch('/scenario/'+name,{method:'POST'});}
function check(label,condition){const road=document.getElementById('road-info-hud');
const result={label,ok:!!condition,roadText:road?.textContent,hidden:road?.hidden,socket:socket.readyState,
ageMs:Date.now()-(sm.recv_time.roadInfo||0)*1000,width:innerWidth,height:innerHeight};
status.textContent=label+': '+(result.ok?'PASS':'FAIL')+' (age '+Math.round(result.ageMs)+'ms, socket '+result.socket+')';
fetch('/report',{method:'POST',body:JSON.stringify(result)});}
setTimeout(()=>{const road=document.getElementById('road-info-hud'),r=road.getBoundingClientRect();
check('限速與道路名／畫面範圍',!road.hidden&&road.textContent.includes('110')&&road.textContent.includes('國道一號')&&r.left>=0&&r.right<=innerWidth);},1800);
setTimeout(()=>check('換道路與限速',document.getElementById('road-info-hud').textContent.includes('50')&&document.getElementById('road-info-hud').textContent.includes('中山路')),11800);
setTimeout(()=>check('未知限速只留道路名',document.querySelector('.road-info-limit').hidden&&!document.querySelector('.road-info-name').hidden),16800);
setTimeout(()=>check('取消資料清除畫面',document.getElementById('road-info-hud').hidden&&document.querySelector('.road-info-name').textContent===''),21800);
setTimeout(()=>check('恢復資料',!document.getElementById('road-info-hud').hidden),24800);
setTimeout(()=>check('斷線後清除限速與跑馬燈',document.getElementById('road-info-hud').hidden&&document.getElementById('traffic-hud').hidden),32000);
</script><script src="/js/traffic-hud.js"></script><script src="/js/road-info-hud.js"></script></html>`;
  let manual = null;
  const server = http.createServer(async (request, response) => {
    response.setHeader('Cache-Control', 'no-store');
    if (request.url === '/report' && request.method === 'POST') {
      let body = '';
      for await (const chunk of request) { body += chunk; if (body.length > 32768) break; }
      try { reports.push(JSON.parse(body)); response.end('ok'); }
      catch { response.writeHead(400).end(); }
      return;
    }
    if (request.url?.startsWith('/scenario/') && request.method === 'POST') {
      const scenario = request.url.slice('/scenario/'.length);
      if (!['highway', 'city', 'unknown', 'clear', 'disconnect'].includes(scenario)) {
        response.writeHead(400).end(); return;
      }
      manual = scenario;
      if (scenario === 'disconnect') for (const socket of sockets) socket.destroy();
      response.end('ok'); return;
    }
    if (request.url === '/') {
      manual = null;
      response.setHeader('Content-Type', 'text/html; charset=utf-8'); response.end(html); return;
    }
    if (assets.has(request.url)) {
      response.setHeader('Content-Type', request.url.endsWith('.css') ? 'text/css; charset=utf-8' : 'application/javascript; charset=utf-8');
      response.end(await fs.readFile(path.join(__dirname, 'web/dist', request.url))); return;
    }
    response.writeHead(404).end();
  });
  server.on('upgrade', (request, socket) => {
    if (request.url !== '/api/ws' || !request.headers['sec-websocket-key']) { socket.destroy(); return; }
    sockets.add(socket);
    socket.write('HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: '
      + crypto.createHash('sha1').update(request.headers['sec-websocket-key']+'258EAFA5-E914-47DA-95CA-C5AB0DC85B11').digest('base64')+'\r\n\r\n');
    const start = Date.now();
    const timer = setInterval(() => {
      const elapsed = Date.now() - start;
      if ((!manual && elapsed >= 26000) || manual === 'disconnect') { socket.destroy(); return; }
      const scenario = manual || (elapsed < 10000 ? 'highway' : elapsed < 15000 ? 'city' : elapsed < 20000 ? 'unknown' : elapsed < 23000 ? 'clear' : 'highway');
      const roadInfo = scenario === 'clear' ? null : {roadName:scenario==='highway'?'國道一號':'中山路',
        speedLimitKph:scenario==='unknown'?null:scenario==='highway'?110:50};
      const events = scenario === 'highway'
        ? [{description:'國道事故，占用外側車道，請減速並留意現場指揮。'.repeat(3),isCurrentSection:false}]
        : scenario === 'city' ? [{description:'中山路道路施工，請依現場指示通行。',isCurrentSection:true}] : [];
      const tdx = scenario === 'clear' ? null : {nextSpeed:65,displayNextSpeed:'65',speedUnit:'km/h',congestion:'slow',events};
      const payload = Buffer.from(JSON.stringify({ts:Date.now(),roadInfo,tdx}));
      const header = payload.length < 126 ? Buffer.from([129,payload.length]) : Buffer.from([129,126,payload.length>>8,payload.length&255]);
      socket.write(Buffer.concat([header,payload]));
    },250);
    socket.on('error',()=>clearInterval(timer));
    socket.on('close',()=>{clearInterval(timer);sockets.delete(socket);});
  });
  return {server,reports,sockets};
}

module.exports = {createPreviewServer};
if (require.main === module) {
  const {server} = createPreviewServer();
  server.listen(8765,'127.0.0.1',()=>console.log('Mock road HUD: http://127.0.0.1:8765 (32 second test)'));
}
