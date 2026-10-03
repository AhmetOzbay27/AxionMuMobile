// C-02 — eksik tablolar icin BindCol ile kesin veri tipi/boyut cikarimi
const fs = require('fs');
const path = require('path');
const ROOT = process.argv[2] || '.';

const MISSING = ['cardphone','customitembank','customnpcquest','datanapgame','equipinventory',
  'eventinventory','itemmarketdata','murummycard','murummydata','muuninventory',
  'pcpointdata','pentagramjewel','pshopitemvalue','snsdata'];

const TYPES = {
  SQL_CHAR:1, SQL_NUMERIC:2, SQL_DECIMAL:3, SQL_INTEGER:4, SQL_SMALLINT:5, SQL_FLOAT:6,
  SQL_REAL:7, SQL_DOUBLE:8, SQL_DATETIME:9, SQL_VARCHAR:12, SQL_TYPE_TIMESTAMP:11,
  SQL_LONGVARCHAR:-1, SQL_BINARY:-2, SQL_VARBINARY:-3, SQL_BIGINT:-5,
  SQL_BIT:-7, SQL_WCHAR:-8, SQL_WVARCHAR:-9, SQL_WLONGVARCHAR:-10, SQL_GUID:-11
};
const TN = Object.fromEntries(Object.entries(TYPES).map(([k, v]) => [v, k]));

function walk(dir, out = []) {
  let ents; try { ents = fs.readdirSync(dir, { withFileTypes: true }); } catch { return out; }
  for (const e of ents) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) walk(p, out);
    else if (/\.cpp$/i.test(e.name) && !/pugixml/i.test(e.name)) out.push(p);
  }
  return out;
}

// Bir fonksiyon/ blok icindeki tum BindCol satirlarini (indeks, degisken, tip, boyut)
const BINDCOL = /Bind(?:Col|Parameter)\s*\(\s*(\d+)\s*,\s*&?([A-Za-z_][A-Za-z0-9_.\->\[\]]*)\s*,\s*(\d+)\s*(?:,\s*(\d+))?\s*(?:,\s*(\d+))?\s*\)/g;
// SQL cagrisindan sonraki ~25 satir icinde BindCol arar
const SQLCALL = /(?:ExecQuery|Fetch)\s*\(\s*"([^"]*(?:TABLE|[A-Za-z_]+)[^"]*)"[^;]{0,400}?\)\s*;/gi;

const found = new Map();
for (const f of [...walk(path.join(ROOT,'Source/2.DataServer')), ...walk(path.join(ROOT,'Source/4.GameServer/GameServer'))]) {
  const rel = path.relative(ROOT, f).replace(/\\/g, '/');
  const txt = fs.readFileSync(f, 'utf8');
  const low = txt.toLowerCase();
  for (const t of MISSING) {
    const idx = low.indexOf('from ' + t);
    if (idx === -1 && low.indexOf('into ' + t) === -1 && low.indexOf(t + '(') === -1) continue;
    // tum dosyada bu tabloya ait ExecQuery cagrilarini + ardindaki BindCol'lari topla
    let pos = 0;
    while ((pos = low.indexOf(t, pos)) !== -1) {
      const win = txt.slice(Math.max(0, pos - 200), pos + 1400);
      BINDCOL.lastIndex = 0; let b;
      const seen = [];
      while ((b = BINDCOL.exec(win)) !== null) {
        const type = TN[+b[3]] || ('SQL_' + b[3]);
        seen.push({ idx: +b[1], var: b[2], type, size: b[4] ? +b[4] : null });
      }
      if (seen.length) {
        if (!found.has(t)) found.set(t, []);
        found.get(t).push({ file: rel, binds: seen });
      }
      pos += t.length;
      if (found.get(t) && found.get(t).length > 4) break;
    }
  }
}

const L = ['C-02 — BINDCOL TİP/BOYUT ÇIKARIMI (kesin veri tipleri)', ''];
for (const t of MISSING) {
  L.push('### ' + t.toUpperCase());
  const rows = found.get(t) || [];
  if (!rows.length) L.push('  (BindCol bulunamadi — SELECT * kullaniliyor, tip koddan anlasilir)');
  rows.forEach(r => {
    L.push('  ' + r.file);
    r.binds.forEach(b => L.push('     #' + b.idx + '  ' + b.type + (b.size !== null ? '(' + b.size + ')' : '') + '  <- ' + b.var));
  });
  L.push('');
}
fs.writeFileSync(path.join(ROOT, 'BuildLog/2e4/c02_bindcol_types.txt'), L.join('\n'), 'utf8');
console.log(L.join('\n'));