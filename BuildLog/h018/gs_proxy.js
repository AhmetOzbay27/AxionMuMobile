// gs_proxy.js - H-018 tel yakalayici: 55920 ve 55901 -> 127.0.0.1:55902 (GS).
// c2s.bin / s2c.bin: ham baytlar; marks.jsonl: baglanti sinirlari + ofsetler; gs_proxy.log: olaylar.
const net = require('net');
const fs = require('fs');
const path = require('path');
const CAP = path.join(__dirname, 'capture');
fs.mkdirSync(CAP, { recursive: true });
const TARGET = { host: '127.0.0.1', port: 55902 };
const PORTS = [55920, 55901];
const fC2S = path.join(CAP, 'c2s.bin');
const fS2C = path.join(CAP, 's2c.bin');
const fLog = path.join(CAP, 'gs_proxy.log');
const fMark = path.join(CAP, 'marks.jsonl');
let offC2S = 0, offS2C = 0, conn = 0;
const wsC2S = fs.createWriteStream(fC2S);
const wsS2C = fs.createWriteStream(fS2C);
const lg = fs.createWriteStream(fLog, { flags: 'a' });
fs.writeFileSync(fMark, '');
function t() { return new Date().toISOString(); }
function mark(o) { fs.appendFileSync(fMark, JSON.stringify(o) + '\n'); }
lg.write(`[${t()}] baslat: ${PORTS.join(',')} -> ${TARGET.host}:${TARGET.port}\n`);
mark({ ev: 'proxy_start', ts: t(), ports: PORTS, target: TARGET });
function handle(cl, lport) {
  const id = ++conn;
  const from = `${cl.remoteAddress}:${cl.remotePort} -> :${lport}`;
  lg.write(`[${t()}] CONN#${id} ${from}\n`);
  mark({ ev: 'conn_open', id, from, ts: t(), s2c_off: offS2C, c2s_off: offC2S });
  const up = net.connect(TARGET.port, TARGET.host);
  cl.on('data', (b) => { offC2S += b.length; wsC2S.write(b); });
  up.on('data', (b) => { offS2C += b.length; wsS2C.write(b); });
  cl.pipe(up); up.pipe(cl);
  cl.on('error', (e) => lg.write(`[${t()}] CONN#${id} istemci_hata ${e.code}\n`));
  up.on('error', (e) => lg.write(`[${t()}] CONN#${id} gs_hata ${e.code}\n`));
  cl.on('close', () => { lg.write(`[${t()}] CONN#${id} istemci_kapandi\n`); mark({ ev: 'conn_close', id, side: 'client', ts: t(), s2c_off: offS2C, c2s_off: offC2S }); up.destroy(); });
  up.on('close', () => { lg.write(`[${t()}] CONN#${id} gs_kapandi\n`); mark({ ev: 'conn_close', id, side: 'gs', ts: t(), s2c_off: offS2C, c2s_off: offC2S }); cl.destroy(); });
}
for (const p of PORTS) {
  const srv = net.createServer((c) => handle(c, p));
  srv.on('error', (e) => lg.write(`[${t()}] LISTEN_ERR :${p} ${e.message}\n`));
  srv.listen(p, '0.0.0.0', () => lg.write(`[${t()}] LISTEN :${p}\n`));
}
function stop() { lg.write(`[${t()}] STOP conns=${conn} c2s=${offC2S} s2c=${offS2C}\n`); mark({ ev: 'proxy_stop', ts: t(), conns: conn, c2s: offC2S, s2c: offS2C }); process.exit(0); }
process.on('SIGINT', stop); process.on('SIGTERM', stop);
