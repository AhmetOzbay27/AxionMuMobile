const fs = require('fs');
for (const f of ['Dashboard/data/sohbet.json', 'Dashboard/data/sonuc.json']) {
  JSON.parse(fs.readFileSync(f, 'utf8'));
  const raw = fs.readFileSync(f, 'latin1');
  console.log(f, 'JSON OK | BOM:', raw.charCodeAt(0) === 0xFEFF, '| CRLF:', raw.includes('\r\n'), '| repl:', (raw.match(/\uFFFD/g) || []).length);
}
const d = fs.readFileSync('docs/37-603-DERLEME-KAPISI.md', 'latin1');
console.log('docs/37 BOM:', d.charCodeAt(0) === 0xFEFF, '| CRLF:', d.includes('\r\n'), '| repl:', (d.match(/\uFFFD/g) || []).length, '| lines:', d.split('\n').length - 1);
const c = fs.readFileSync('docs/CHANGELOG.md', 'latin1');
console.log('CHANGELOG BOM:', c.charCodeAt(0) === 0xFEFF, '| 13:33:', c.split('## [26.10.04 13:33]').length - 1, '| 12:08:', c.split('## [26.10.04 12:08]').length - 1, '| repl:', (c.match(/\uFFFD/g) || []).length);
for (const f of ['Source/5.Main/source/WSclient.cpp', 'Source/5.Main/source/WSclient.h', 'Source/5.Main/source/Defined_Global.h']) {
  const raw = fs.readFileSync(f, 'latin1');
  console.log(f, '| BOM:', raw.charCodeAt(0) === 0xFEFF, '| repl:', (raw.match(/\uFFFD/g) || []).length, '| CRLF:', raw.includes('\r\n'));
}
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
console.log('sohbet entries:', sb.entries.length, '| top ts:', sb.entries[0].ts, '| updated:', sb.updated);
