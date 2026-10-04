const fs = require('fs');
for (const f of ['Dashboard/data/sohbet.json', 'Dashboard/data/sonuc.json']) {
  JSON.parse(fs.readFileSync(f, 'utf8'));
  const raw = fs.readFileSync(f, 'latin1');
  console.log(f, 'JSON OK | BOM:', raw.charCodeAt(0) === 0xFEFF, '| CRLF:', raw.includes('\r\n'), '| repl:', (raw.match(/\uFFFD/g) || []).length);
}
const d = fs.readFileSync('docs/36-GETMAIN-IKILI-KARARI.md', 'latin1');
console.log('docs/36 BOM:', d.charCodeAt(0) === 0xFEFF, '| CRLF:', d.includes('\r\n'), '| repl:', (d.match(/\uFFFD/g) || []).length, '| lines:', d.split('\n').length - 1);
const c = fs.readFileSync('docs/CHANGELOG.md', 'latin1');
console.log('CHANGELOG BOM:', c.charCodeAt(0) === 0xFEFF, '| yeni cipa:', c.split('## [26.10.04 12:08]').length - 1, '| eski cipa:', c.split('## [26.10.04 06:50]').length - 1, '| repl:', (c.match(/\uFFFD/g) || []).length);
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
console.log('sohbet entries:', sb.entries.length, '| top ts:', sb.entries[0].ts, '| updated:', sb.updated);
