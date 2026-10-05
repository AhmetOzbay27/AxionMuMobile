// crosscheck_runtime_spec.js - H-018: RUNTIME yakalanan paketler <-> layout spec (603/0)
//
// Amac: gercekten telden gelen paketlerin ALAN OFSETLERINI, kaynaktan uretilen
// layout spec dosyasiyla (layout_spec_603_0.txt) bayt bayt karsilastirmak.
//
// Girdiler:
//   1) BuildLog/h018/client/H018DRV_recv.bin - istemcinin TranslateProtocol'a
//      verdigi duz metin paket akisi (H018PktDump kancasi, enc=0).
//   2) BuildLog/h018/capture/s2c.bin         - proxy tel kaydi (GS -> istemci);
//      K1/K2 GameServerInfo - Common.ini anahtarlarindan turetilir.
//   3) BuildLog/h018/capture/layout_spec_603_0.txt - spec
//      (node BuildLog/2e7/viewport_layout.js 603 0 ciktisi).
//
// kullanim: node BuildLog/h018/crosscheck_runtime_spec.js
// exit 0 = TUM ALANLAR SPEC ILE AYNI, 1 = uyusmazlik var.
const fs = require('fs');
const LF = String.fromCharCode(10);
const CR = String.fromCharCode(13);

const SPEC = 'BuildLog/h018/capture/layout_spec_603_0.txt';
const RECV = 'BuildLog/h018/client/H018DRV_recv.bin';
const S2C = 'BuildLog/h018/capture/s2c.bin';
const CFG = 'BuildLog/h018/deploy/4.GameServer/Sub 1/GameServer/Data/GameServerInfo - Common.ini';
const OUT = 'BuildLog/h018/capture/runtime_spec_crosscheck.txt';

const lines = [];
function out(s) { lines.push(s); console.log(s); }

function parseSpec(title) {
  const text = fs.readFileSync(SPEC, 'latin1');
  const start = text.indexOf('== ' + title + ' ==');
  if (start < 0) throw new Error('spec blok yok: ' + title);
  let end = text.indexOf(LF + '== ', start + 4);
  if (end < 0) end = text.length;
  const rows = text.slice(start, end).split(LF);
  const fields = {};
  let size = -1;
  for (const row of rows) {
    const t = row.replace(/^ +| +$/g, '');
    if (t.indexOf('sizeof') === 0) {
      const eq = t.indexOf('=');
      if (eq >= 0) size = parseInt(t.slice(eq + 1), 10);
      continue;
    }
    if (!/^[0-9]+ /.test(t)) continue;
    const parts = t.split(/ +/);
    if (parts.length < 3) continue;
    const off = parseInt(parts[0], 10);
    const bytes = parseInt(parts[1], 10);
    const name = parts[parts.length - 1];
    if (isNaN(off) || isNaN(bytes)) continue;
    fields[name] = { off: off, bytes: bytes };
  }
  const list = [];
  for (const nm in fields) { if (Object.prototype.hasOwnProperty.call(fields, nm)) list.push({ name: nm, off: fields[nm].off, bytes: fields[nm].bytes }); }
  list.sort(function (a, b) { return a.off - b.off; });
  return { fields: list, byName: fields, size: size, title: title };
}

// --- cerceve ayristirici (tam cerceve akisi: C1/C2/C3/C4) ---
function parseStream(buf, label) {
  const pks = [];
  let off = 0, stray = 0;
  while (off + 3 <= buf.length) {
    const t = buf[off];
    let size = -1, head = -1, hdr = 0;
    if (t === 0xC1) { size = buf[off + 1]; head = buf[off + 2]; hdr = 3; }
    else if (t === 0xC2) { if (off + 4 <= buf.length) { size = (buf[off + 1] << 8) | buf[off + 2]; head = buf[off + 3]; hdr = 4; } }
    else if (t === 0xC3) { size = buf[off + 1]; head = -2; hdr = 3; }
    else if (t === 0xC4) { if (off + 4 <= buf.length) { size = (buf[off + 1] << 8) | buf[off + 2]; head = -2; hdr = 4; } }
    const min = (t === 0xC2 || t === 0xC4) ? 4 : 3;
    if (size < min || off + size > buf.length) { stray++; off++; continue; }
    pks.push({ off: off, type: t, size: size, head: head, hdr: hdr });
    off += size;
  }
  return { buf: buf, pks: pks, stray: stray, bytes: buf.length, label: label };
}

// --- ini anahtar cozucu (split tabanli; regex kacis dizisi yok) ---
function iniVal(text, key) {
  const L = text.split(LF);
  for (let i = 0; i < L.length; i++) {
    const t = L[i].split(CR).join('').trim();
    if (t.indexOf(key) === 0) {
      const eq = t.indexOf('=');
      if (eq >= 0) return t.slice(eq + 1).trim();
    }
  }
  return null;
}

function decryptS2C() {
  const cfg = fs.readFileSync(CFG, 'latin1');
  const customer = iniVal(cfg, 'CustomerName');
  const serial = iniVal(cfg, 'ServerSerial');
  if (!customer || !serial) throw new Error('CustomerName/ServerSerial okunamadi');
  const cb = Buffer.alloc(32);
  Buffer.from(customer, 'latin1').copy(cb);
  const sb = Buffer.alloc(17);
  Buffer.from(serial, 'latin1').copy(sb);
  let sum = 0;
  for (let n = 0; n < 32; n++) sum = (sum + (cb[n] ^ sb[n % 17])) & 0xFFFF;
  const K1 = (0xF1 + (sum & 0xFF)) & 0xFF;
  const K2 = (0x1A + ((sum >> 8) & 0xFF)) & 0xFF;
  const raw = fs.readFileSync(S2C);
  let start = 0;
  const mk = fs.readFileSync('BuildLog/h018/capture/marks.jsonl', 'utf8').split(LF);
  for (const line of mk) {
    if (!line.trim()) continue;
    let o = null;
    try { o = JSON.parse(line); } catch (e) { continue; }
    if (o.ev === 'conn_open' && typeof o.s2c_off === 'number') start = o.s2c_off;
  }
  const seg = raw.slice(start);
  const p = Buffer.alloc(seg.length);
  for (let i = 0; i < seg.length; i++) p[i] = ((seg[i] ^ K1) - K2) & 0xFF;
  return { buf: p, K1: K1, K2: K2, start: start, total: raw.length };
}

function fieldOf(sp, name) {
  return sp.byName[name] || null;
}

function readField(buf, b, f) {
  if (f.bytes === 1) return buf[b + f.off];
  if (f.bytes === 2) return buf[b + f.off] | (buf[b + f.off + 1] << 8);
  if (f.bytes === 4) return (buf[b + f.off] | (buf[b + f.off + 1] << 8) | (buf[b + f.off + 2] << 16) | (buf[b + f.off + 3] << 24)) >>> 0;
  return -1;
}

function readStr(buf, b, f) {
  let s = '';
  for (let k = 0; k < f.bytes; k++) { const c = buf[b + f.off + k]; if (c === 0) break; s += String.fromCharCode(c); }
  return s;
}

function decodeRecord(buf, b, sp) {
  const parts = [];
  for (const f of sp.fields) {
    let v;
    if (f.name === 'name') v = JSON.stringify(readStr(buf, b, f));
    else if (f.bytes <= 4) v = String(readField(buf, b, f));
    else v = '0x' + Buffer.from(buf.slice(b + f.off, b + f.off + f.bytes)).toString('hex').toUpperCase();
    parts.push('+' + f.off + ' ' + f.name + '=' + v);
  }
  return parts.join('  ');
}

// --------------------------------------------------------------------------
out('# H-018 runtime paketleri <-> layout spec capraz kontrolu');
out('');

// 1) SPEC yukle
const spPlayer  = parseSpec('SUNUCU PMSG_VIEWPORT_PLAYER');
const spChange  = parseSpec('SUNUCU PMSG_VIEWPORT_CHANGE');
const spMonster = parseSpec('SUNUCU PMSG_VIEWPORT_MONSTER');
const spSummon  = parseSpec('SUNUCU PMSG_VIEWPORT_SUMMON');
out('spec    : ' + SPEC);
out('  PLAYER  sizeof=' + spPlayer.size + ' alan=' + spPlayer.fields.length);
out('  CHANGE  sizeof=' + spChange.size + ' alan=' + spChange.fields.length);
out('  MONSTER sizeof=' + spMonster.size + ' alan=' + spMonster.fields.length);
out('  SUMMON  sizeof=' + spSummon.size + ' alan=' + spSummon.fields.length);
out('');

// 2) Kaynaklar
const recv = fs.readFileSync(RECV);
const recvStream = parseStream(recv, 'H018DRV_recv.bin (istemci duz metin)');
const dec = decryptS2C();
const wireStream = parseStream(dec.buf, 's2c.bin (proxy tel kaydi, K1/K2 cozulmus)');
out('istemci : ' + RECV + ' (' + recvStream.bytes + ' bayt) -> paket=' + recvStream.pks.length + ' sapan=' + recvStream.stray);
out('tel     : ' + S2C + ' (' + dec.total + ' bayt, K1=0x' + dec.K1.toString(16).toUpperCase() + ' K2=0x' + dec.K2.toString(16).toUpperCase() + ', baglanti basi=' + dec.start + ') -> paket=' + wireStream.pks.length + ' sapan=' + wireStream.stray);
out('');

// 3) Kayitlari spec'e gore coz + alan alan dogrula
const checks = [];
function chk(name, ok, detail) { checks.push({ name: name, ok: !!ok, detail: detail || '' }); }

function recordsOf(stream, head, bodySize) {
  const recs = [];
  for (const pk of stream.pks) {
    if (pk.head !== head) continue;
    const n = stream.buf[pk.off + pk.hdr];
    const body = pk.size - pk.hdr - 1;
    chk('0x' + head.toString(16).toUpperCase() + ' cerceve boyu = ' + pk.hdr + '+1+' + bodySize + '*n',
        body === bodySize * n, 'size=' + pk.size + ' n=' + n + ' govde=' + body + ' beklenen=' + (bodySize * n));
    for (let j = 0; j < n; j++) {
      recs.push({ stream: stream, base: pk.off + pk.hdr + 1 + bodySize * j });
    }
  }
  return recs;
}

const playerRecs  = recordsOf(wireStream, 0x12, spPlayer.size);
const monsterRecs = recordsOf(wireStream, 0x13, spMonster.size);
const changeRecs  = recordsOf(wireStream, 0x45, spChange.size);
const summonRecs  = recordsOf(wireStream, 0x1F, spSummon.size);

out('## Tel kaydindan cozulen kayitlar');
out('0x12 PLAYER  : ' + playerRecs.length + ' kayit');
out('0x13 MONSTER : ' + monsterRecs.length + ' kayit');
out('0x45 CHANGE  : ' + changeRecs.length + ' kayit');
out('0x1F SUMMON  : ' + summonRecs.length + ' kayit');
out('');
for (const r of playerRecs) out('  PLAYER  ' + decodeRecord(r.stream.buf, r.base, spPlayer));
for (const r of monsterRecs) out('  MONSTER ' + decodeRecord(r.stream.buf, r.base, spMonster));
for (const r of changeRecs) out('  CHANGE  ' + decodeRecord(r.stream.buf, r.base, spChange));
for (const r of summonRecs) out('  SUMMON  ' + decodeRecord(r.stream.buf, r.base, spSummon));
out('');

// 4) Kritik alan ofset dogrulamalari (spec <-> runtime deger)
// PLAYER: name @+22 okunabiliyor ve karakter adiyla ayni; CharSet @+4 18 bayt.
let selfRec = null;
for (const r of playerRecs) {
  const nm = readStr(r.stream.buf, r.base, fieldOf(spPlayer, 'name'));
  if (nm === 'H018Test') selfRec = r;
}
chk('PLAYER name@+22 = H018Test (kendi karakter)', selfRec !== null, 'PLAYER kayitlari=' + playerRecs.length);
chk('PLAYER CharSet@+4 (18B) canli', playerRecs.length > 0 && fieldOf(spPlayer, 'CharSet').bytes === 18, '');
chk('PLAYER count@+35 = 0 (buff yok)', playerRecs.length > 0 && readField(playerRecs[0].stream.buf, playerRecs[0].base, fieldOf(spPlayer, 'count')) === 0, '');
chk('MONSTER CurHp@+9 = 100', monsterRecs.length > 0 && readField(monsterRecs[0].stream.buf, monsterRecs[0].base, fieldOf(spMonster, 'CurHp')) === 100, '');
chk('MONSTER count@+16 = 0', monsterRecs.length > 0 && readField(monsterRecs[0].stream.buf, monsterRecs[0].base, fieldOf(spMonster, 'count')) === 0, '');

// 5) Iki bagimsiz kaynak karsilastirmasi: istemci duz metni <-> proxy tel kaydi
const cliPlayerRecs = recordsOf(recvStream, 0x12, spPlayer.size);
const cliMonsterRecs = recordsOf(recvStream, 0x13, spMonster.size);
let dualOk = cliPlayerRecs.length === playerRecs.length && cliMonsterRecs.length === monsterRecs.length;
for (let i = 0; i < cliPlayerRecs.length && dualOk; i++) {
  const a = cliPlayerRecs[i], b = playerRecs[i];
  if (a.stream.buf.length <= a.base + 36 || b.stream.buf.length <= b.base + 36) { dualOk = false; break; }
  for (let k = 0; k < 36; k++) if (a.stream.buf[a.base + k] !== b.stream.buf[b.base + k]) { dualOk = false; break; }
}
chk('istemci duz metni = proxy tel kaydi (PLAYER 36B birebir)', dualOk, 'istemci=' + cliPlayerRecs.length + ' tel=' + playerRecs.length);
let dualMon = cliMonsterRecs.length === monsterRecs.length;
for (let i = 0; i < cliMonsterRecs.length && dualMon; i++) {
  const a = cliMonsterRecs[i], b = monsterRecs[i];
  if (a.stream.buf.length <= a.base + 20 || b.stream.buf.length <= b.base + 20) { dualMon = false; break; }
  for (let k = 0; k < 20; k++) if (a.stream.buf[a.base + k] !== b.stream.buf[b.base + k]) { dualMon = false; break; }
}
chk('istemci duz metni = proxy tel kaydi (MONSTER 20B birebir)', dualMon, 'istemci=' + cliMonsterRecs.length + ' tel=' + monsterRecs.length);
out('');
out('## Kontroller');
for (const c of checks) out((c.ok ? 'PASS  ' : 'FAIL  ') + c.name + (c.detail ? '  [' + c.detail + ']' : ''));
const fails = checks.filter(function (c) { return !c.ok; }).length;
const verdict = fails === 0 ? 'PASS' : 'FAIL';
out('');
out('SONUC=' + verdict + '  (FAIL=' + fails + ', kontrol=' + checks.length + ')');
fs.writeFileSync(OUT, lines.join(LF) + LF, 'utf8');
fs.writeFileSync(OUT.replace('.txt', '.json'), JSON.stringify({
  spec: SPEC, specSizes: { player: spPlayer.size, change: spChange.size, monster: spMonster.size, summon: spSummon.size },
  sources: { istemci: { file: RECV, bytes: recvStream.bytes, packets: recvStream.pks.length, stray: recvStream.stray },
             tel: { file: S2C, bytes: dec.total, packets: wireStream.pks.length, stray: wireStream.stray, K1: dec.K1, K2: dec.K2, connStart: dec.start } },
  records: { player: playerRecs.length, monster: monsterRecs.length, change: changeRecs.length, summon: summonRecs.length },
  checks: checks, verdict: verdict, fails: fails
}, null, 2), 'utf8');
process.exit(fails === 0 ? 0 : 1);
