// C-02 — eksik tablolar icin gereken sutun kumesi (tablo adi hem koseli hem cplak kabul eder)
const fs = require('fs');
const path = require('path');
const ROOT = process.argv[2] || '.';

const MISSING = [
  'cardphone', 'customitembank', 'customnpcquest', 'datanapgame', 'equipinventory',
  'eventinventory', 'itemmarketdata', 'murummycard', 'murummydata', 'muuninventory',
  'pcpointdata', 'pentagramjewel', 'pshopitemvalue', 'snsdata'
];

function walk(dir, out = []) {
  let ents; try { ents = fs.readdirSync(dir, { withFileTypes: true }); } catch { return out; }
  for (const e of ents) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) walk(p, out);
    else if (/\.cpp$/i.test(e.name) && !/pugixml/i.test(e.name)) out.push(p);
  }
  return out;
}

const LIT = /"((?:[^"\\]|\\.)*)"/g;
const SQLSTART = /^\s*(select|insert\s+into|update|delete\s+from)\b/i;
const ID = '[A-Za-z_][A-Za-z0-9_]*';
const clean = s => s.replace(/[\[\]]/g, '').trim();

const files = [
  ...walk(path.join(ROOT, 'Source/2.DataServer')),
  ...walk(path.join(ROOT, 'Source/3.JoinServer')),
  ...walk(path.join(ROOT, 'Source/4.GameServer/GameServer'))
];

const need = new Map(MISSING.map(t => [t, {
  ins: new Set(), upd: new Set(), sel: new Set(), where: new Set(), files: new Set(), stmts: []
}]));

for (const f of files) {
  const txt = fs.readFileSync(f, 'utf8');
  const rel = path.relative(ROOT, f).replace(/\\/g, '/');
  LIT.lastIndex = 0; let m;
  while ((m = LIT.exec(txt)) !== null) {
    const lit = m[1];
    if (!SQLSTART.test(lit)) continue;
    for (const t of MISSING) {
      if (lit.toLowerCase().indexOf(t) === -1) continue;
      const rec = need.get(t);
      rec.files.add(rel);
      if (rec.stmts.length < 3) rec.stmts.push(lit.replace(/\s+/g, ' ').trim().slice(0, 150));
      const T = '(?:\\[?dbo\\]?\\.)?\\[?' + t + '\\]?';

      let mm = lit.match(new RegExp('insert\\s+into\\s+' + T + '\\s*\\(([^)]*)\\)', 'i'));
      if (mm) mm[1].split(',').forEach(c => { const x = clean(c); if (new RegExp('^' + ID + '$').test(x)) rec.ins.add(x.toLowerCase()); });

      mm = lit.match(new RegExp('update\\s+' + T + '\\s+set\\s+(.+?)(?:\\bwhere\\b|$)', 'i'));
      if (mm) mm[1].split(',').forEach(p => { const x = clean(p.split('=')[0]); if (new RegExp('^' + ID + '$').test(x)) rec.upd.add(x.toLowerCase()); });

      mm = lit.match(new RegExp('select\\s+(.+?)\\s+from\\s+' + T + '\\b', 'i'));
      if (mm && !/^\*$/i.test(mm[1].trim()))
        mm[1].split(',').forEach(c => { const x = clean(c.split(/\s+as\s+/i)[0]); if (new RegExp('^' + ID + '$').test(x)) rec.sel.add(x.toLowerCase()); });

      mm = lit.match(/\bwhere\s+(.+)$/i);
      if (mm) mm[1].split(/\s+and\s+/i).forEach(c => {
        const x = clean(c.replace(/^\(\s*/, '').split(/\s*(?:=|<>|>=|<=|>|<|like\b|in\s*\()/i)[0]);
        if (new RegExp('^' + ID + '$').test(x)) rec.where.add(x.toLowerCase());
      });
    }
  }
}

const L = ['C-02 — EKSİK TABLOLARIN GEREKEN SÜTUN KÜMELERİ (kaynak koddan çıkarıldı)', ''];
for (const t of MISSING) {
  const r = need.get(t);
  const all = new Set([...r.ins, ...r.upd, ...r.sel, ...r.where]);
  L.push('### ' + t.toUpperCase() + '  — ' + all.size + ' sütun');
  L.push('  INSERT : ' + (r.ins.size ? [...r.ins].join(', ') : '-'));
  L.push('  UPDATE : ' + (r.upd.size ? [...r.upd].join(', ') : '-'));
  L.push('  SELECT : ' + (r.sel.size ? [...r.sel].join(', ') : '-'));
  L.push('  WHERE  : ' + (r.where.size ? [...r.where].join(', ') : '-'));
  L.push('  TÜMÜ   : ' + [...all].sort().join(', '));
  L.push('  DOSYA  : ' + [...r.files].join(', '));
  for (const s of r.stmts) L.push('    > ' + s);
  L.push('');
}
fs.writeFileSync(path.join(ROOT, 'BuildLog/2e4/c02_missing_columns.txt'), L.join('\n'), 'utf8');
console.log(L.join('\n'));