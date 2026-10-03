const fs = require('fs');
const bad = '31-ISTEMCI-EKSENI-IS-EMRI.md';
const good = '31-ISTEMCI-EKSENI-IS-EMI.md';
for (const f of ['docs/00-PROJE-HARITASI.md','docs/02-YOL-HARITASI.md','docs/30-PARITE-MANIFESTI.md','docs/33-DENETIM-PROJE-CAPINDA.md']) {
  let s = fs.readFileSync(f, 'utf8');
  const n = s.split(bad).length - 1;
  if (n > 0) { s = s.split(bad).join(good); fs.writeFileSync(f, s, 'utf8'); }
  console.log(f, 'duzeltilen=' + n);
}
