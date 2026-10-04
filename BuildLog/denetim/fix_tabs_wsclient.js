const fs = require('fs');
const f = 'Source/5.Main/source/WSclient.h';
let s = fs.readFileSync(f, 'latin1');
const before = (s.match(/^\/\/ 04\.10\.2026 \(docs\/34\)/gm) || []).length;
s = s.replace(/^\/\/ 04\.10\.2026 \(docs\/34\)/gm, '\t// 04.10.2026 (docs/34)');
fs.writeFileSync(f, s, 'latin1');
console.log('sekme eklendi:', before);
