const fs = require('fs');
const { execSync } = require('child_process');
for (const f of ['Dashboard/data/sohbet.json', 'Dashboard/data/sonuc.json']) {
  JSON.parse(fs.readFileSync(f, 'utf8'));
  const raw = fs.readFileSync(f, 'latin1');
  console.log(f, 'JSON OK | BOM:', raw.charCodeAt(0) === 0xFEFF, '| CRLF:', raw.includes('\r\n'), '| repl:', (raw.match(/\uFFFD/g) || []).length);
}
for (const f of ['docs/39-PUSH-ONCESI-KANCA.md', 'docs/38-TOPLU-DERLEME-SCRIPTI.md', 'docs/CHANGELOG.md', 'BuildLog/denetim/pre_push_check.sh', 'BuildLog/denetim/install_hooks.sh', 'BuildLog/denetim/build_all.sh', '.githooks/pre-push']) {
  const raw = fs.readFileSync(f, 'latin1');
  const ctrl = (raw.match(/[\x00-\x08\x0B\x0C\x0E-\x1F]/g) || []).length;
  console.log(f, '| BOM:', raw.charCodeAt(0) === 0xFEFF, '| CRLF:', raw.includes('\r\n'), '| repl:', (raw.match(/\uFFFD/g) || []).length, '| ctrl:', ctrl);
}
for (const f of ['BuildLog/denetim/pre_push_green.log', 'BuildLog/denetim/pre_push_green_push.txt', 'BuildLog/denetim/pre_push_red.log', 'BuildLog/denetim/pre_push_red_push.txt']) {
  const raw = fs.readFileSync(f, 'latin1');
  console.log(f, '| repl:', (raw.match(/\uFFFD/g) || []).length, '| satir:', raw.split('\n').length - 1);
}
const c = fs.readFileSync('docs/CHANGELOG.md', 'latin1');
console.log('CHANGELOG 20:02:', c.split('## [26.10.04 20:02]').length - 1, '| 14:15:', c.split('## [26.10.04 14:15]').length - 1);
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
console.log('sohbet entries:', sb.entries.length, '| top ts:', sb.entries[0].ts, '| updated:', sb.updated);
console.log('hooksPath:', execSync('git config --local core.hooksPath').toString().trim());
