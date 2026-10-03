// crosscheck_live_parity.js - viewport_layout.js ciktisi (603/0) ile canli PDB
// kanit dosyasini alan alan karsilastirir. docs/27'deki capraz kontrol tablosunun
// otomatik dogrulanisi.
const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const here = __dirname;
const tool = path.join(here, 'viewport_layout.js');
const pdbFile = path.join(here, 'live_pdb_viewport_layout.txt');

// 1) arac ciktisi. Arac 603/0 icin BILEREK exit=1 doner (istemci tarafi EX803
// biciminde oldugu icin "kayma" beklenir); burada yalnizca SUNUCU tarafi
// dogrulanacagindan cikti okunur. Cocuk kodu yutulmez, ayri olarak yazilir.
const run = spawnSync(process.execPath, [tool, '603', '0'], { encoding: 'latin1' });
const out = run.stdout || '';
console.log(`# viewport_layout.js 603 0 -> cocuk exit=${run.status} (beklenen: 1, istemci EX803 biciminde)\n`);

// 2) canli PDB kanit dosyasi -> { struct: {size, {field: off}} }
const pdb = {};
let cur = null;
for (const line of fs.readFileSync(pdbFile, 'latin1').split(/\r?\n/)) {
  let m = line.match(/^(PMSG_\w+)\s+size=(\d+)/);
  if (m) { cur = m[1]; pdb[cur] = { size: +m[2], f: {} }; continue; }
  m = line.match(/^\s+\+(\d+)\s+(\w+)\s/);
  if (m && cur) pdb[cur].f[m[2]] = +m[1];
}

// 3) arac ciktisi -> ayni sekil
const toolL = {};
let curT = null;
for (const line of out.split(/\r?\n/)) {
  let m = line.match(/^== SUNUCU (PMSG_\w+) ==/);
  if (m) { curT = m[1]; toolL[curT] = { size: -1, f: {} }; continue; }
  if (/^== /.test(line)) { curT = null; continue; }   // ISTEMCI bolumune gecildi
  if (!curT) continue;
  m = line.match(/^\s+(\d+)\s+(\d+)\s+\S+(?:\[\d+\])?\s+(\w+)\s*$/);
  if (m) { toolL[curT].f[m[3]] = +m[1]; continue; }
  m = line.match(/^\s+sizeof = (\d+)/);
  if (m) toolL[curT].size = +m[1];
}

let bad = 0;
console.log('capraz kontrol: viewport_layout.js (603/0)  vs  canli GameServer.pdb\n');
// viewport_layout.js yalnizca dort GAMESERVER->ISTEMCI paketini yazdirir;
// ayni dordunu karsilastiriyoruz (pdb tarafi ek yapilar icin ek basliklarda).
for (const name of ['PMSG_VIEWPORT_PLAYER', 'PMSG_VIEWPORT_CHANGE', 'PMSG_VIEWPORT_MONSTER', 'PMSG_VIEWPORT_SUMMON']) {
  const a = toolL[name], b = pdb[name];
  if (!a || !b) { console.log(`  ${name}: ARAC=${!!a} PDB=${!!b}  HATA`); bad++; continue; }
  const sizeOk = a.size === b.size;
  const keys = new Set([...Object.keys(a.f), ...Object.keys(b.f)]);
  const diffs = [];
  for (const k of keys) if (a.f[k] !== b.f[k]) diffs.push(`${k}: arac=${a.f[k]} pdb=${b.f[k]}`);
  const ok = sizeOk && diffs.length === 0;
  if (!ok) bad++;
  console.log(`  ${ok ? 'ESIT  ' : 'FARKLI'} ${name}  sizeof ${a.size}/${b.size}  alan ${Object.keys(a.f).length}/${Object.keys(b.f)}`);
  for (const d of diffs) console.log(`        - ${d}`);
}
console.log(bad === 0 ? '\n### CANLI PARITE DOGRULANDI (tum yapilar bayt-bayt ayni)' : `\n### ${bad} YAPI FARKLI`);
process.exit(bad === 0 ? 0 : 1);