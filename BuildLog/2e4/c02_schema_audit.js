// C-02 — DB sema uyumu denetimi (v2)
// Yalnizca SQL dize literal'lerinden tablo adi cikarir (gQueryManager.ExecQuery("SELECT ...") kalibi).
const fs = require('fs');
const path = require('path');

const ROOT = process.argv[2] || '.';
const dbTables = new Set(
  fs.readFileSync(path.join(ROOT, 'BuildLog/2e4/db_tables.txt'), 'utf8')
    .split(/\r?\n/).map(s => s.trim()).filter(Boolean).map(s => s.toLowerCase())
);

const SRC_DIRS = [
  'Source/1.ConnectServer', 'Source/2.DataServer', 'Source/3.JoinServer',
  'Source/4.GameServer/GameServer'
];

function walk(dir, out = []) {
  let ents;
  try { ents = fs.readdirSync(dir, { withFileTypes: true }); } catch { return out; }
  for (const e of ents) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) walk(p, out);
    else if (/\.(cpp|h)$/i.test(e.name) && !/pugixml/i.test(e.name)) out.push(p);
  }
  return out;
}

// Cift tirnakli literal icinde SQL mi?  Tirnak atlayarak tarariz.
const LIT = /"((?:[^"\\]|\\.)*)"/g;
const SQLSTART = /^\s*(select|insert\s+into|update|delete\s+from)\b/i;
// literal icindeki tablo referanslari
// NOT: "(?:\[?dbo\]?\.)?" -> "INSERT INTO dbo.MEMB_INFO(...)" yaziminda
// tablo adi "dbo" degil "MEMB_INFO"dir. Sema oneki atilir (yanlis pozitif onlemi).
const TBL = /\b(?:from|into|update|join)\s+(?:\[?dbo\]?\s*\.\s*)?\[?([A-Za-z_][A-Za-z0-9_]*)\]?/gi;
const STOP = new Set([
  'select','insert','update','delete','where','values','set','from','and','or','not',
  'null','is','in','like','between','order','group','by','having','join','inner',
  'left','right','outer','on','as','distinct','top','case','when','then','else','end',
  'exists','union','all','into','default','exec','table','if','begin','declare','proc',
  'database','use','update','create','alter','drop','column','with','convert','substring',
  'getdate','newid','col_length','object_id','openquery','opendatasource'
]);

const used = new Map();
const cols  = new Map(); // tablo -> Set(sutun)
let scanned = 0, literals = 0;

for (const d of SRC_DIRS) {
  for (const f of walk(path.join(ROOT, d))) {
    scanned++;
    const txt = fs.readFileSync(f, 'utf8');
    const rel = path.relative(ROOT, f).replace(/\\/g, '/');
    LIT.lastIndex = 0;
    let m;
    while ((m = LIT.exec(txt)) !== null) {
      const lit = m[1];
      if (!SQLSTART.test(lit)) continue;   // SQL dizesi degil -> atla
      literals++;
      // INSERT INTO X (a,b,c)  -> sutunlar
      const ins = lit.match(/insert\s+into\s+(?:\[?dbo\]?\s*\.\s*)?\[?([A-Za-z_][A-Za-z0-9_]*)\]?\s*\(([^)]*)\)/i);
      TBL.lastIndex = 0;
      let t;
      while ((t = TBL.exec(lit)) !== null) {
        const name = t[1];
        if (STOP.has(name.toLowerCase())) continue;
        const key = name.toLowerCase();
        if (!used.has(key)) used.set(key, new Set());
        used.get(key).add(rel);
      }
      if (ins && !STOP.has(ins[1].toLowerCase())) {
        const key = ins[1].toLowerCase();
        if (!cols.has(key)) cols.set(key, new Set());
        ins[2].split(',').forEach(c => {
          const cc = c.trim().replace(/^\[|\]$/g, '').trim();
          if (cc && !/^@/i.test(cc)) cols.get(key).add(cc.toLowerCase());
        });
      }
      // UPDATE X SET a=.. -> sutunlar
      const upd = lit.match(/update\s+(?:\[?dbo\]?\s*\.\s*)?\[?([A-Za-z_][A-Za-z0-9_]*)\]?\s+set\s+(.+?)(?:\bwhere\b|$)/i);
      if (upd && !STOP.has(upd[1].toLowerCase())) {
        const key = upd[1].toLowerCase();
        if (!cols.has(key)) cols.set(key, new Set());
        upd[2].split(',').forEach(p => {
          const cc = p.split('=')[0].trim().replace(/^\[|\]$/g, '').trim();
          if (cc && !/^@/i.test(cc) && /^[A-Za-z_][A-Za-z0-9_]*$/.test(cc)) cols.get(key).add(cc.toLowerCase());
        });
      }
    }
  }
}

const eksik = [], tam = [], colSorun = [];
for (const [t, files] of [...used.entries()].sort()) {
  if (dbTables.has(t)) {
    tam.push([t, files.size, cols.get(t) || new Set()]);
  } else {
    eksik.push([t, [...files]]);
  }
}
const fazla = [...dbTables].filter(t => !used.has(t)).sort();

const L = [];
L.push('C-02 — DB SEMA UYUMU DENETIMI (kaynak kodu vs canli MuOnlineS6)');
L.push('Tarih: 03.10.2026 · Bekleyen gorev: docs/03 C-02 "tam sema uyumu kontrolu Faz 3.1\'de"');
L.push('');
L.push('Taranan kaynak dosyasi : ' + scanned);
L.push('SQL dize literal sayisi : ' + literals);
L.push('Canli DB tablo sayisi    : ' + dbTables.size);
L.push('Kaynakta kullanilan tablo: ' + used.size);
L.push('');
L.push('=== A) KULLANILAN VE DB\'DE OLAN (' + tam.length + ') ===');
for (const [t, n] of tam) L.push('  OK   ' + t + '  (' + n + ' dosya)');
L.push('');
L.push('=== B) KULLANILAN AMA DB\'DE OLMAYAN — EKSIK TABLO (' + eksik.length + ') ===');
if (!eksik.length) L.push('  (yok)');
for (const [t, files] of eksik)
  L.push('  EKSIK  ' + t + '\n         -> ' + files.join('\n         -> '));
L.push('');
L.push('=== C) DB\'DE OLUP KAYNAKTA KULLANILMAYAN (' + fazla.length + ') ===');
L.push('  ' + fazla.join(', '));
L.push('');
L.push('=== D) INSERT/UPDATE ile yazilan sutunlar (kapsam dogrulamasi icin) ===');
for (const [t, , cs] of tam) if (cs.size) L.push('  ' + t + ': ' + [...cs].join(', '));

fs.writeFileSync(path.join(ROOT, 'BuildLog/2e4/c02_schema_report.txt'), L.join('\n'), 'utf8');
console.log(L.slice(0, 60).join('\n'));
console.log('\n>>> taranan=' + scanned + ' literal=' + literals + ' db=' + dbTables.size +
            ' kullanilan=' + used.size + ' eksik=' + eksik.length + ' tam=' + tam.length +
            ' fazla=' + fazla.length);
console.log('\n=== EKSIK TABLOLAR ===');
for (const [t, f] of eksik) console.log('  ' + t + '  [' + f[0].replace('Source/', '') + ']');
console.log('\n=== FAZLA (kullanilmayan) ===');
console.log('  ' + fazla.join(', '));