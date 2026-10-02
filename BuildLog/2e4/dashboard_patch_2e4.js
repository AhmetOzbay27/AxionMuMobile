// dashboard_patch_2e4.js — 2e.4 pano kayıtlarına bayt onarımı + nihai md5 + run15-17 işler.
'use strict';
const fs = require('fs');
const path = require('path');
const D = path.join(__dirname, '..', '..', 'Dashboard', 'data');

function patch(text, repls) {
  for (const [oldS, newS] of repls) {
    if (!text.includes(oldS)) { console.error('BULUNAMADI:', JSON.stringify(oldS.slice(0, 80))); process.exit(1); }
    text = text.split(oldS).join(newS);
  }
  return text;
}

const sonuc = JSON.parse(fs.readFileSync(path.join(D, 'sonuc.json'), 'utf8'));
sonuc.text = patch(sonuc.text, [[
  'md5 b386bd65c4fbdc1aec840adfcfb8af8a (12.029.952 B), 0 hata (yalnız LNK4099/MSB8004). Sahada run14: diyalog yok, çökme yok, kalıntı süreç yok; pencere t=0,9 s (Axion Mu), t=5,2 s 45.87.120.29:44405 SYN_SENT;',
  'md5 b386bd65c4fbdc1aec840adfcfb8af8a idi (run14); düzenleme araçlarının kaynakta bozduğu 26 EUC-KR satırı HEAD baytlarıyla geri konulup yeniden derlendi -> nihai md5 6dc52f5fb63746cb51575ab1370f7e6c (12.029.952 B), 0 hata (yalnız LNK4099/MSB8004). Sahada run14 + run15-17: diyalog yok, çökme yok, kalıntı süreç yok; pencere ~0,8-0,9 s (Axion Mu), run17 de t=21,9 s 45.87.120.29:44405 SYN_SENT;'
]]);
sonuc.updated = '2026-10-02 23:15:00';
fs.writeFileSync(path.join(D, 'sonuc.json'), JSON.stringify(sonuc, null, 2), 'utf8');
console.log('sonuc.json yamalandi');

const ch = JSON.parse(fs.readFileSync(path.join(D, 'sohbet.json'), 'utf8'));
const e = ch.entries.find(x => x.ts === '2026-10-02 22:35' && x.kim === 'ajan');
if (!e) { console.error('sohbet ajan kaydi bulunamadi'); process.exit(1); }
e.text = patch(e.text, [
  ['Derleme: b386bd65c4fbdc1aec840adfcfb8af8a (12.029.952 B), 0 hata. Sahada run14: diyalogsuz/çökmesiz, 45.87.120.29:44405 SYN_SENT;',
   'Derleme: run14 b386bd65c4fbdc1aec840adfcfb8af8a (12.029.952 B); kaynaktaki 26 bozuk EUC-KR satırı HEAD baytlarıyla geri konuldu ve yeniden derlendi -> nihai md5 6dc52f5fb63746cb51575ab1370f7e6c, 0 hata. Sahada run14 + run15-17: diyalogsuz/çökmesiz (run17: t=21,9 s 45.87.120.29:44405 SYN_SENT);'],
  ['(2e2/deploy 324 MB, 5Main obj 161 MB, 2e3/deploy 25 MB) silindi',
   '(2e2/deploy 324 MB, 5Main obj 161 MB, 2e3/deploy 25 MB, 2d1 sandbox/ourtool 35 MB) silindi']
]);
ch.updated = '2026-10-02 23:15:00';
fs.writeFileSync(path.join(D, 'sohbet.json'), JSON.stringify(ch, null, 2), 'utf8');
console.log('sohbet.json yamalandi');
