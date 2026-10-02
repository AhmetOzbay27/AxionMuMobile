// fix_encoding.js — Düzenlenen dosyalarda U+FFFD'ye dönüşmüş EUC-KR baytları geri koyar.
// Yöntem: git diff --cached -U0 hunk'larını gezer; "+" tarafında EF BF BD (latin1: ï¿½)
// olan ama "-" tarafında olmayan ve satır sayısı eşit olan hunk'larda, yeni satırı HEAD
// satırıyla birebir (bayt düzeyinde, latin1) değiştirir.
'use strict';
const { execSync } = require('child_process');
const fs = require('fs');
const ROOT = process.cwd();
const FFFD = '\u00ef\u00bf\u00bd'; // latin1 karşılığı EF BF BD

const staged = execSync('git diff --cached --name-only', { cwd: ROOT }).toString().trim().split('\n').filter(Boolean);
let totalFix = 0;

for (const file of staged) {
  let diff;
  try {
    diff = execSync(`git diff --cached -U0 -- "${file}"`, { cwd: ROOT, maxBuffer: 64 * 1024 * 1024 }).toString('latin1');
  } catch (e) { continue; }
  if (!diff || diff.includes('Binary files')) continue;

  const lines = diff.split('\n');
  let oldStart = 0, newStart = 0, oldBuf = [], newBuf = [], inHunk = false;
  const fixes = []; // { newLine, oldText }

  const close = () => {
    if (!inHunk) return;
    const newHas = newBuf.some(l => l.includes(FFFD));
    const oldHas = oldBuf.some(l => l.includes(FFFD));
    if (newHas && !oldHas && oldBuf.length === newBuf.length && oldBuf.length > 0) {
      for (let i = 0; i < oldBuf.length; i++) {
        fixes.push({ newLine: newStart + i, oldText: oldBuf[i].slice(1) });
      }
    }
    inHunk = false; oldBuf = []; newBuf = [];
  };

  for (const raw of lines) {
    const m = raw.match(/^@@ -(\d+)(?:,\d+)? \+(\d+)(?:,\d+)? @@/);
    if (m) { close(); oldStart = +m[1]; newStart = +m[2]; oldBuf = []; newBuf = []; inHunk = true; continue; }
    if (!inHunk) continue;
    if (raw.startsWith('-')) oldBuf.push(raw);
    else if (raw.startsWith('+')) newBuf.push(raw);
    else if (raw.startsWith('\\')) continue;
  }
  close();
  if (fixes.length === 0) continue;

  const cur = fs.readFileSync(file).toString('latin1').split('\n');
  let ok = 0;
  for (const f of fixes) {
    const idx = f.newLine - 1;
    const line = cur[idx];
    if (line === undefined || !line.includes(FFFD)) { console.log('  ATLANDI (beklenen bozuk satir degil):', file, f.newLine); continue; }
    const cr = line.endsWith('\r') ? '\r' : '';
    cur[idx] = f.oldText + cr;
    ok++;
  }
  fs.writeFileSync(file, Buffer.from(cur.join('\n'), 'latin1'));
  console.log(`${file}: ${ok} satir geri alindi`);
  totalFix += ok;
}
console.log('TOPLAM geri alinan satir:', totalFix);
