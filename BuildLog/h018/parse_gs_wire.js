// parse_gs_wire.js - H-018: GS->istemci tel kaydini cozer; cerceve + viewport paketlerini dogrular.
const fs = require('fs');
function arg(name, def) { const k = '--' + name + '=', p = process.argv.slice(2), m = p.find(a => a.startsWith(k)); return m ? m.slice(k.length) : def; }
const S2C = arg('s2c', 'BuildLog/h018/capture/s2c.bin');
const CFG = arg('cfg', 'BuildLog/h018/deploy/4.GameServer/Sub 1/GameServer/Data/GameServerInfo - Common.ini');
const EXP_CHAR = arg('char', 'H018Test');
const OUT_TXT = arg('out', 'BuildLog/h018/capture/wire_summary.txt');
const OUT_JSON = arg('json', 'BuildLog/h018/capture/wire_summary.json');
const cfg = fs.readFileSync(CFG, 'latin1');
function iniVal(key) { const L = cfg.split(String.fromCharCode(10)); for (let i = 0; i < L.length; i++) { const t = L[i].split(String.fromCharCode(13)).join('').trim(); if (t.indexOf(key) === 0) { const eq = t.indexOf('='); if (eq >= 0) return t.slice(eq + 1).trim(); } } return null; }
const customer = iniVal('CustomerName'), serial = iniVal('ServerSerial');
if (!customer || !serial) { console.error('HATA: CustomerName/ServerSerial okunamadi'); process.exit(2); }
const cb = Buffer.alloc(32); Buffer.from(customer, 'latin1').copy(cb);
const sb = Buffer.alloc(17); Buffer.from(serial, 'latin1').copy(sb);
let sum = 0;
for (let n = 0; n < 32; n++) sum = (sum + (cb[n] ^ sb[n % 17])) & 0xFFFF;
const K1 = (0xF1 + (sum & 0xFF)) & 0xFF;
const K2 = (0x1A + ((sum >> 8) & 0xFF)) & 0xFF;
const raw = fs.readFileSync(S2C);
const p = Buffer.alloc(raw.length);
for (let i = 0; i < raw.length; i++) p[i] = ((raw[i] ^ K1) - K2) & 0xFF;
const lines = [];
function out(s) { lines.push(s); console.log(s); }
out('# H-018 tel dogrulama');
out('kayit   : ' + S2C + ' (' + raw.length + ' ham bayt)');
out("anahtar : CustomerName='" + customer + "' ServerSerial='" + serial + "' -> K1=0x" + K1.toString(16).toUpperCase() + ' K2=0x' + K2.toString(16).toUpperCase() + ' (sum=0x' + sum.toString(16) + ')');
const packets = [];
let i = 0, stray = 0;
const hist = {};
while (i < p.length) {
  const t = p[i];
  let size = -1, head = -1;
  if (t === 0xC1) { if (i + 3 <= p.length) { size = p[i + 1]; head = p[i + 2]; } }
  else if (t === 0xC2) { if (i + 4 <= p.length) { size = (p[i + 1] << 8) | p[i + 2]; head = p[i + 3]; } }
  else if (t === 0xC3) { if (i + 3 <= p.length) { size = p[i + 1]; head = -2; } }
  else if (t === 0xC4) { if (i + 4 <= p.length) { size = (p[i + 1] << 8) | p[i + 2]; head = -2; } }
  const minSize = (t === 0xC2 || t === 0xC4) ? 4 : 3;
  if (size < minSize || i + size > p.length) { stray++; i++; continue; }
  packets.push({ off: i, type: t, size: size, head: head });
  const hk = head === -2 ? 'C3C4_sifreli' : '0x' + head.toString(16).toUpperCase().padStart(2, '0');
  hist[hk] = (hist[hk] || 0) + 1;
  i += size;
}
out('paket   : ' + packets.length + ' cerceve, sapan bayt=' + stray);
out('opcode  : ' + Object.keys(hist).sort().map(k => k + 'x' + hist[k]).join(' '));
function u16(b, o) { return b[o] | (b[o + 1] << 8); }
function u32(b, o) { return (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0; }
function str10(b, o) { let s = ''; for (let k = 0; k < 10; k++) { const c = b[o + k]; if (!c) break; s += String.fromCharCode(c); } return s; }
const vp = { players: [], monsters: [], summons: [], changes: [] };
const checks = [];
function chk(name, ok, detail) { checks.push({ name: name, ok: !!ok, detail: detail || '' }); }
for (const pk of packets) {
  if (pk.type !== 0xC2) continue;
  if (pk.head === 0x12) {
    const count = p[pk.off + 4];
    chk('0x12 PLAYER boyut = 5+36*n', pk.size === 5 + 36 * count, 'size=' + pk.size + ' n=' + count + ' beklenen=' + (5 + 36 * count));
    for (let j = 0; j < count; j++) {
      const b = pk.off + 5 + 36 * j;
      vp.players.push({ off: pk.off, index: u16(p, b), x: p[b + 2], y: p[b + 3], name: str10(p, b + 22), tx: p[b + 32], ty: p[b + 33], dirPk: p[b + 34], kuyruk: p[b + 35] });
    }
  } else if (pk.head === 0x13) {
    const count = p[pk.off + 4];
    chk('0x13 MONSTER boyut = 5+20*n', pk.size === 5 + 20 * count, 'size=' + pk.size + ' n=' + count + ' beklenen=' + (5 + 20 * count));
    for (let j = 0; j < count; j++) {
      const b = pk.off + 5 + 20 * j;
      vp.monsters.push({ off: pk.off, index: u16(p, b), type: u16(p, b + 2), x: p[b + 4], y: p[b + 5], curHp: p[b + 9], level: u16(p, b + 10), life: u32(p, b + 12) });
    }
  } else if (pk.head === 0x1F) {
    const count = p[pk.off + 4];
    chk('0x1F SUMMON boyut = 5+20*n', pk.size === 5 + 20 * count, 'size=' + pk.size + ' n=' + count);
    for (let j = 0; j < count; j++) {
      const b = pk.off + 5 + 20 * j;
      vp.summons.push({ off: pk.off, index: u16(p, b), type: u16(p, b + 2), x: p[b + 4], y: p[b + 5], name: str10(p, b + 9) });
    }
  } else if (pk.head === 0x45) {
    const count = p[pk.off + 4];
    chk('0x45 CHANGE boyut = 5+38*n', pk.size === 5 + 38 * count, 'size=' + pk.size + ' n=' + count);
    for (let j = 0; j < count; j++) {
      const b = pk.off + 5 + 38 * j;
      vp.changes.push({ off: pk.off, index: u16(p, b), x: p[b + 2], y: p[b + 3], skin: u16(p, b + 4), name: str10(p, b + 6) });
    }
  }
}
const self = vp.players.find((e) => e.name === EXP_CHAR);
chk('cerceveleme saglam (sapan bayt yok)', stray === 0, 'sapan=' + stray);
chk('0x12 oyuncu paketi geldi', vp.players.length > 0, 'kayit=' + vp.players.length);
chk("kendi karakter viewport'ta (" + EXP_CHAR + ')', !!self, self ? 'index=' + self.index + ' x=' + self.x + ' y=' + self.y : 'isimler: ' + Array.from(new Set(vp.players.map((e) => e.name))).join(','));
out('');
out('## Viewport ozeti');
out('0x12 PLAYER : kayit=' + vp.players.length + ' | ' + JSON.stringify(vp.players.slice(0, 4)));
out('0x13 MONSTER: kayit=' + vp.monsters.length + ' | ' + JSON.stringify(vp.monsters.slice(0, 4)));
out('0x1F SUMMON : kayit=' + vp.summons.length + ' | ' + JSON.stringify(vp.summons.slice(0, 4)));
out('0x45 CHANGE : kayit=' + vp.changes.length + ' | ' + JSON.stringify(vp.changes.slice(0, 4)));
if (vp.monsters.length === 0) out('NOT: 0x13 MONSTER kaydi gelmedi (spawn cevresinde canli yok olabilir).');
out('');
out('## Kontroller');
for (const c of checks) out((c.ok ? 'PASS  ' : 'FAIL  ') + c.name + (c.detail ? '  [' + c.detail + ']' : ''));
const fails = checks.filter((c) => !c.ok).length;
const verdict = fails === 0 ? 'PASS' : 'FAIL';
out('');
out('SONUC=' + verdict + '  (FAIL=' + fails + ', kontrol=' + checks.length + ')');
fs.writeFileSync(OUT_TXT, lines.join('\n') + '\n', 'utf8');
fs.writeFileSync(OUT_JSON, JSON.stringify({ s2c: S2C, rawBytes: raw.length, packets: packets.length, stray: stray, hist: hist, keys: { customer: customer, serial: serial, K1: K1, K2: K2, sum: sum }, vp: vp, checks: checks, verdict: verdict }, null, 2), 'utf8');
process.exit(verdict === 'PASS' ? 0 : 1);
