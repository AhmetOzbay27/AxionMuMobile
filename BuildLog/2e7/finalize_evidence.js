// finalize_evidence.js - 2e.7 kanit dosyalari derinligi + otomatik capraz kontrol
const fs = require('fs');
const path = require('path');
const R = path.resolve(__dirname, '..', '..');
let fail = 0;

// docs/27: kanit dosyasi ve otomatik capraz kontrolu kayda gecir
{
  const p = path.join(R, 'docs/27-H018-VIEWPORT-PAKET-DUZENI.md');
  let s = fs.readFileSync(p, 'utf8');
  const anchor = 'Bu, hem düzeltmeyi hem de aracı doğrulayan bir çapraz kontroldür.';
  if (s.indexOf(anchor) < 0) { console.log('HATA docs/27 capraz kontrol paragrafi yok'); fail++; }
  else {
    const add = anchor + `

Bu kontrol **elle değil, otomatik** olarak da doğrulanır —
\`node BuildLog/2e7/crosscheck_live_parity.js\` (çıktı:
\`crosscheck_live_parity.txt\`). Araç dört yapının **her alanının offset'ini**
canlı PDB kaydıyla tek tek karşılaştırır ve eşleşmezse \`exit 1\` verir:

\`\`\`
  ESIT   PMSG_VIEWPORT_PLAYER  sizeof 36/36  alan 9
  ESIT   PMSG_VIEWPORT_CHANGE  sizeof 38/38  alan 10
  ESIT   PMSG_VIEWPORT_MONSTER sizeof 20/20  alan 11 (CurHp,Level,Life dahil)
  ESIT   PMSG_VIEWPORT_SUMMON  sizeof 20/20  alan 9
### CANLI PARITE DOGRULANDI (tum yapilar bayt-bayt ayni)
\`\`\``;
    s = s.replace(anchor, add);
    fs.writeFileSync(p, s, 'utf8');
    console.log('docs/27 guncellendi');
  }
  // yeniden uretim bolumune capraz kontrol komutu
  s = fs.readFileSync(p, 'utf8');
  const cmdAnchor = 'node viewport_layout.js 603 0        # canlı yapılandırma';
  if (s.indexOf(cmdAnchor) >= 0) {
    s = s.replace(cmdAnchor, cmdAnchor + '\nnode crosscheck_live_parity.js  # arac ciktisi vs canli PDB (exit 0 = bayt-bayt ayni)');
    fs.writeFileSync(p, s, 'utf8');
    console.log('docs/27 yeniden uretim bolumu guncellendi');
  }
}

// CHANGELOG: kanit derinligi notu
{
  const p = path.join(R, 'docs/CHANGELOG.md');
  let s = fs.readFileSync(p, 'utf8');
  const a = '- `node BuildLog/2e7/viewport_layout.js 803 1` → **`### TUM PAKETLER HIZALI`,\n  exit 0** (`layout_803_1.txt`).';
  if (s.indexOf(a) >= 0) {
    s = s.replace(a, a + '\n- `node BuildLog/2e7/crosscheck_live_parity.js` → **exit 0**; dört yapının her\n  alan ofseti canlı PDB ile otomatik karşılaştırıldı (`crosscheck_live_parity.txt`).\n  Kanıt dosyası `pdbtype ... 1` (derinlik 1) ile alan ofsetlerini içerecek şekilde yeniden üretildi.');
    fs.writeFileSync(p, s, 'utf8');
    console.log('CHANGELOG guncellendi');
  } else { console.log('HATA CHANGELOG girisi bulunamadi'); fail++; }
}

console.log(fail === 0 ? '### KANIT TAMAMLANDI' : `### ${fail} HATA`);
process.exit(fail === 0 ? 0 : 1);