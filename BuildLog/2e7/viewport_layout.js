// viewport_layout.js - H-018 viewport paket duzeni denetleyicisi
// Gercek baslik dosyalarini okur (Viewport.h / WSclient.h), #if bloklarini
// verilen (GAMESERVER_UPDATE, HAISLOTRING) degerleriyle cozer ve her iki tarafin
// bayt haritasini karsilastirir.
//
// kullanim: node viewport_layout.js [GAMESERVER_UPDATE] [HAISLOTRING]
// varsayilan: 603 0   (canli SPK 5.2); EX803 cifti icin: node viewport_layout.js 803 1
const fs = require('fs');
const path = require('path');

const REPO = path.resolve(__dirname, '..', '..');
const GS_H = path.join(REPO, 'Source/4.GameServer/GameServer/Viewport.h');
const CL_H = path.join(REPO, 'Source/5.Main/source/WSclient.h');

// --- istemci sabitleri (5.Main/source/WSclient.h:71, Define.h:4, _define.h:665)
const CLIENT_CONST = { EQUIPMENT_LENGTH: 17, MAX_ID_SIZE: 10, MAX_BUFF_SLOT_INDEX: 32 };

// --- temel turler: [bayt, hizalama]
const TYPES = {
  BYTE: [1, 1], char: [1, 1], BOOL: [1, 1],
  WORD: [2, 2], short: [2, 2], 'unsigned short': [2, 2],
  DWORD: [4, 4], int: [4, 4], float: [4, 4], 'unsigned int': [4, 4],
};

function stripComment(s) { return s.replace(/\/\/.*$/, '').trim(); }

function extractStruct(text, target) {
  // iki bicim de desteklenir:
  //   struct NAME { ... };              (Viewport.h)
  //   typedef struct { ... } NAME, *LP; (WSclient.h)
  const re = /\bstruct\b/g;
  let m;
  while ((m = re.exec(text)) !== null) {
    const i = m.index + m[0].length;
    const head = text.slice(i, i + 300).replace(/\r/g, '');
    const before = /^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\{/.exec(head);
    let brace = -1;
    for (let k = 0; k < head.length; k++) { if (head[k] === '{') { brace = i + k; break; } }
    if (brace < 0) continue;
    if (before && before[1] === target) {
      let depth = 0, end = -1;
      for (let j = brace; j < text.length; j++) {
        const c = text[j];
        if (c === '{') depth++;
        else if (c === '}') { depth--; if (depth === 0) { end = j; break; } }
      }
      if (end > 0) return text.slice(brace + 1, end);
      continue;
    }
    let depth = 0, end = -1;
    for (let j = brace; j < text.length; j++) {
      const c = text[j];
      if (c === '{') depth++;
      else if (c === '}') { depth--; if (depth === 0) { end = j; break; } }
    }
    if (end < 0) continue;
    const after = text.slice(end + 1, end + 400).replace(/\r/g, '');
    if (new RegExp('^\\s*,?\\s*\\*?\\s*(?:LP)?' + target + '\\b').test(after)) return text.slice(brace + 1, end);
  }
  return null;
}

// --- mini if-cozucu -------------------------------------------------------
function evalExpr(expr, cfg) {
  let e = expr.replace(/\band\b/g, '&&').replace(/\bor\b/g, '||').replace(/\bnot\b/g, '!').replace(/\s+/g, ' ').trim();
  const ident = (name) => {
    if (name in cfg) return cfg[name];
    if (name in CLIENT_CONST) return CLIENT_CONST[name];
    return 0;
  };
  // >=, <=, ==, !=, >, <, &&, ||, ! oncelikli
  const toks = e.match(/>=|<=|==|!=|&&|\|\||[!<>=]|[A-Za-z_][A-Za-z0-9_]*|\d+/g) || [];
  let pos = 0;
  const peek = () => toks[pos];
  const eat = (t) => { if (toks[pos] === t) { pos++; return true; } return false; };
  function primary() {
    const t = peek();
    if (t === '!') { pos++; return primary() ? 0 : 1; }
    pos++;
    if (/^\d/.test(t)) return parseInt(t, 10);
    return ident(t);
  }
  function mul() { let v = primary(); while (peek() === '*' ) { pos++; v *= primary(); } return v; }
  function cmp() {
    let v = mul();
    for (;;) {
      const t = peek();
      if (t === '>=' || t === '<=' || t === '==' || t === '!=' || t === '>' || t === '<') { pos++; const r = mul();
        if (t === '>=') v = v >= r ? 1 : 0; else if (t === '<=') v = v <= r ? 1 : 0;
        else if (t === '==') v = v === r ? 1 : 0; else if (t === '!=') v = v !== r ? 1 : 0;
        else if (t === '>') v = v > r ? 1 : 0; else v = v < r ? 1 : 0;
      } else break;
    }
    return v;
  }
  function and() { let v = cmp(); while (peek() === '&&') { pos++; const r = cmp(); v = (v && r) ? 1 : 0; } return v; }
  function or() { let v = and(); while (peek() === '||') { pos++; const r = and(); v = (v || r) ? 1 : 0; } return v; }
  const v = or();
  return v ? true : false;
}

function extractIfCond(line) {
  // #if yonergesinin acilis ve kapanis parantezleri arasini alir:
  // "#if(A) && (B)" -> "A) && (B"  (ic parantezler ifadeye aittir)
  const i = line.indexOf('('), j = line.lastIndexOf(')');
  if (i < 0 || j <= i) return null;
  return line.slice(i + 1, j);
}

function parseFields(body, cfg) {
  const out = [];
  const stack = [];              // true = aktif
  let active = true;
  for (const rawLine of body.split(/\r?\n/)) {
    const line = stripComment(rawLine);
    if (!line) continue;
    let m;
    if ((m = line.match(/^#\s*if\b/i))) {
      const cond = extractIfCond(line);
      const parent = stack.length ? stack[stack.length - 1] : true;
      const v = evalExpr(cond === null ? '' : cond, cfg);
      stack.push(v);
      active = parent && v;
      continue;
    }
    if (/^#\s*else/i.test(line)) { const v = stack.pop(); stack.push(!v); active = stack.every(Boolean); continue; }
    if (/^#\s*endif/i.test(line)) { stack.pop(); active = stack.length ? stack.every(Boolean) : true; continue; }
    if (!active) continue;
    m = line.match(/^(?:const\s+)?([A-Za-z_][A-Za-z0-9_]*)\s+([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[\s*([^\]]+)\s*\])?\s*;/);
    if (!m) continue;
    const type = m[1], name = m[2];
    let count = 1;
    if (m[3]) {
      const e = m[3].trim();
      const num = /^\d+$/.test(e) ? parseInt(e, 10) : (CLIENT_CONST[e] !== undefined ? CLIENT_CONST[e] : 0);
      count = num || 1;
    }
    const t = TYPES[type];
    if (!t) continue;                      // diger turler bu denetim disi
    out.push({ type, name, count, size: t[0], align: t[1] });
  }
  return out;
}

function layout(fields) {
  let off = 0, maxAlign = 1;
  const res = [];
  for (const f of fields) {
    if (f.align > 1 && (off % f.align) !== 0) off += f.align - (off % f.align);
    res.push({ ...f, off, bytes: f.size * f.count });
    off += f.size * f.count;
    if (f.align > maxAlign) maxAlign = f.align;
  }
  if (maxAlign > 1 && (off % maxAlign) !== 0) off += maxAlign - (off % maxAlign);
  return { fields: res, size: off };
}

// ------------------------------------------------------------------------
function gsStructs(cfg) {
  const text = fs.readFileSync(GS_H, 'latin1');
  const names = ['PMSG_VIEWPORT_PLAYER', 'PMSG_VIEWPORT_CHANGE', 'PMSG_VIEWPORT_MONSTER', 'PMSG_VIEWPORT_SUMMON', 'PMSG_VIEWPORT_SEND', 'PMSG_VIEWPORT_ITEM'];
  const out = {};
  for (const n of names) {
    const body = extractStruct(text, n);
    if (!body) { out[n] = null; continue; }
    out[n] = layout(parseFields(body, cfg));
  }
  return out;
}
function clientStructs(cfg) {
  const text = fs.readFileSync(CL_H, 'latin1');
  const names = ['PCREATE_CHARACTER', 'PCREATE_TRANSFORM', 'PCREATE_SUMMON', 'PCREATE_MONSTER'];
  const out = {};
  for (const n of names) {
    const body = extractStruct(text, n);
    if (!body) { out[n] = null; continue; }
    out[n] = layout(parseFields(body, cfg));
  }
  return out;
}

function table(l, title) {
  let s = '\n== ' + title + ' ==\n';
  if (!l) return s + '  (yapı bulunamadı)\n';
  s += '  ofs  bayt  alan\n';
  for (const f of l.fields) {
    s += `  ${String(f.off).padStart(3)}  ${String(f.bytes).padStart(4)}  ${f.type}${f.count > 1 ? '[' + f.count + ']' : ''} ${f.name}\n`;
  }
  s += `  sizeof = ${l.size}\n`;
  return s;
}

// GS alan adi -> istemci alan adi (semantik esleme)
const MAP_PLAYER = {
  index: 'KeyH+KeyL', x: 'PositionX', y: 'PositionY', CharSet: 'Class+Equipment',
  name: 'ID', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path',
  MuunItem: 'MuunItem', count: 's_BuffCount',
  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP',
};
const MAP_CHANGE = {
  index: 'KeyH+KeyL', x: 'PositionX', y: 'PositionY', skin: 'TypeH+TypeL', name: 'ID',
  tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', CharSet: 'Class+Equipment',
  MuunItem: 'MuunItem', count: 's_BuffCount',
  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP',
};
const MAP_MONSTER = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', count: 's_BuffCount',
  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP', CurHp: 'CurHp', Level: 'Level', Life: 'Life' };
const MAP_SUMMON = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', name: 'ID', count: 's_BuffCount',
  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP' };

function compare(g, c, map, label) {
  let s = '\n--- ' + label + ' ---\n';
  if (!g || !c) return s + '  eksik yapı\n';
  // istemci alanlarının ofset sözlüğü
  const cf = {};
  for (const f of c.fields) cf[f.name] = f;
  let bad = 0;
  const rows = [];
  for (const f of g.fields) {
    const target = map[f.name];
    if (!target) { rows.push(`  GS +${f.off} ${f.name}(${f.bytes}B)  ->  [istemcide karşılığı yok]`); continue; }
    const parts = target.split('+');
    // ilk istemci alanının ofseti
    const first = cf[parts[0]];
    if (!first) { rows.push(`  GS +${f.off} ${f.name}  ->  istemcide ${parts[0]} yok`); bad++; continue; }
    const want = f.name === 'CharSet' ? first.off : first.off;
    const ok = (want === f.off);
    if (!ok) bad++;
    rows.push(`  ${ok ? 'OK  ' : 'KAYMA'} GS +${String(f.off).padStart(3)} ${f.name.padEnd(14)}(${String(f.bytes).padStart(2)}B) -> istemci ${parts.join('+').padEnd(22)} +${want}`);
  }
  s += rows.join('\n') + '\n';
  s += `  sonuc: ${bad === 0 ? 'HIZALI' : bad + ' KAYMA'}\n`;
  return s;
}

const gsu = parseInt(process.argv[2] || '603', 10);
const hais = parseInt(process.argv[3] || '0', 10);
// sunucu cfg: kendi HAISLOTRING makrosu; istemci cfg: sunucunun HAISLOTRING'ini
// temsil eden GAMESERVER_HAISLOTRING (istemcinin KENDI HAISLOTRING'i UI icindir).
const SERVER_CFG = { GAMESERVER_UPDATE: gsu, HAISLOTRING: hais, GAMESERVER_TYPE: 0, GAMESERVER_LANGUAGE: 1, NEW_PROTOCOL_SYSTEM: 0, EQUIPMENT_LENGTH: CLIENT_CONST.EQUIPMENT_LENGTH };
const CLIENT_CFG = { GAMESERVER_UPDATE: gsu, GAMESERVER_HAISLOTRING: hais, HAISLOTRING: 1, GAMESERVER_TYPE: 0, GAMESERVER_LANGUAGE: 1, NEW_PROTOCOL_SYSTEM: 0, EQUIPMENT_LENGTH: CLIENT_CONST.EQUIPMENT_LENGTH };

console.log('H-018 viewport paket düzeni denetimi');
console.log(`GAMESERVER_UPDATE=${gsu}  HAISLOTRING=${hais}  (EQUIPMENT_LENGTH=${CLIENT_CONST.EQUIPMENT_LENGTH}, MAX_ID_SIZE=${CLIENT_CONST.MAX_ID_SIZE}, MAX_BUFF_SLOT_INDEX=${CLIENT_CONST.MAX_BUFF_SLOT_INDEX})`);

const G = gsStructs(SERVER_CFG), C = clientStructs(CLIENT_CFG);
console.log(table(G.PMSG_VIEWPORT_PLAYER, 'SUNUCU PMSG_VIEWPORT_PLAYER'));
console.log(table(G.PMSG_VIEWPORT_CHANGE, 'SUNUCU PMSG_VIEWPORT_CHANGE'));
console.log(table(G.PMSG_VIEWPORT_MONSTER, 'SUNUCU PMSG_VIEWPORT_MONSTER'));
console.log(table(G.PMSG_VIEWPORT_SUMMON, 'SUNUCU PMSG_VIEWPORT_SUMMON'));
console.log(table(C.PCREATE_CHARACTER, 'ISTEMCI PCREATE_CHARACTER'));
console.log(table(C.PCREATE_TRANSFORM, 'ISTEMCI PCREATE_TRANSFORM'));
console.log(table(C.PCREATE_MONSTER, 'ISTEMCI PCREATE_MONSTER'));
console.log(table(C.PCREATE_SUMMON, 'ISTEMCI PCREATE_SUMMON'));

let verdict = 0;
for (const [g, c, map, lbl] of [
  ['PMSG_VIEWPORT_PLAYER', 'PCREATE_CHARACTER', MAP_PLAYER, 'PLAYER paketi: sunucu -> istemci'],
  ['PMSG_VIEWPORT_CHANGE', 'PCREATE_TRANSFORM', MAP_CHANGE, 'CHANGE paketi: sunucu -> istemci'],
  ['PMSG_VIEWPORT_MONSTER', 'PCREATE_MONSTER', MAP_MONSTER, 'MONSTER paketi: sunucu -> istemci'],
  ['PMSG_VIEWPORT_SUMMON', 'PCREATE_SUMMON', MAP_SUMMON, 'SUMMON paketi: sunucu -> istemci'],
]) {
  const t = compare(G[g], C[c], map, lbl);
  console.log(t);
  if (t.indexOf('HIZALI') < 0) verdict++;
}
console.log(`\n### ${verdict === 0 ? 'TUM PAKETLER HIZALI' : verdict + ' PAKET KAYMALI'}`);
process.exit(verdict === 0 ? 0 : 1);process.exit(verdict === 0 ? 0 : 1);
