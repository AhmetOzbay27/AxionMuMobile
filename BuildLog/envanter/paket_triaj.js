// Paket basligi triaji: canli vs bizim (paket_header_canli.txt / paket_header_bizim.txt)
// Cikti: ortak fonksiyonlarda (marker,size,head,sub) farklari + kanit satirlari
const fs = require('fs');
const path = require('path');
const dir = __dirname;

function parse(file) {
  const lines = fs.readFileSync(path.join(dir, file), 'utf8').split(/\r?\n/);
  const funcs = [];
  let cur = null;
  for (const ln of lines) {
    if (/^\?/.test(ln)) {
      const label = ln.trim();
      const m = label.match(/^\?([^@]+)(?:@[^@]*)?@@(.*)$/);
      const name = m ? m[1] : label;
      const sig = m ? m[2] : '';
      cur = { label, name, sig, entries: [] };
      funcs.push(cur);
    } else if (cur && /^\s+C[1-4] /.test(ln)) {
      const e = ln.match(/^\s+(C[1-4]) size=(\d+) head=([0-9A-F]{1,2}|\?) sub=([0-9A-F]{1,2}|\?)/);
      if (e) cur.entries.push({ marker: e[1], size: e[2], head: e[3], sub: e[4], raw: ln.trim() });
    }
  }
  return funcs;
}

const live = parse('paket_header_canli.txt');
const ours = parse('paket_header_bizim.txt');

function key(f) { return f.name + '|' + f.sig; }
function group(list) {
  const map = new Map();
  for (const f of list) {
    const k = key(f);
    if (!map.has(k)) map.set(k, []);
    map.get(k).push(f);
  }
  return map;
}
const gLive = group(live), gOurs = group(ours);

function sigOf(e) { return `${e.marker} ${e.size} ${e.head} ${e.sub}`; }
function sigOfFun(f) { return f.entries.map(sigOf).join(' ;; ') || '(bos)'; }

const out = [];
const onlyLive = [], onlyOurs = [];
let same = 0, diff = 0, ambiguous = 0;
for (const [k, fs] of gLive) {
  const os = gOurs.get(k);
  if (!os) { onlyLive.push(...fs.map(f => f.label)); continue; }
  if (fs.length > 1 || os.length > 1) { ambiguous++; continue; }
  const a = fs[0], b = os[0];
  const sa = sigOfFun(a), sb = sigOfFun(b);
  if (sa === sb) { same++; continue; }
  diff++;
  out.push('### ' + k);
  out.push('  CANLI ' + a.label);
  for (const e of a.entries) out.push('    L: ' + e.raw);
  out.push('  BIZIM ' + b.label);
  for (const e of b.entries) out.push('    B: ' + e.raw);
  out.push('');
}
// yalniz bizimde olanlar
for (const [k, fs] of gOurs) {
  if (!gLive.has(k)) onlyOurs.push(...fs.map(f => f.label));
}
const uniq = a => [...new Set(a)];
const summary = [];
summary.push(`canli fonksiyon=${live.length} bizim fonksiyon=${ours.length}`);
summary.push(`ortak-eslesen: ayni=${same} farkli=${diff} belirsiz(coklu-eslesme)=${ambiguous}`);
summary.push(`sadece-canli=${uniq(onlyLive).length} sadece-bizim=${uniq(onlyOurs).length}`);
summary.push('');
summary.push('## FARKLAR (ayni isim+imza, alan farki)');
summary.push('');
fs.writeFileSync(path.join(dir, 'paket_diff.txt'), summary.join('\n') + out.join('\n') + '\n## SADECE CANLI\n' + uniq(onlyLive).join('\n') + '\n\n## SADECE BIZIM\n' + uniq(onlyOurs).join('\n') + '\n');
console.log(summary.slice(0, 3).join('\n'));
console.log('fark sayisi:', diff);
for (const l of out.filter(x => x.startsWith('### '))) console.log('  ' + l.slice(4));
