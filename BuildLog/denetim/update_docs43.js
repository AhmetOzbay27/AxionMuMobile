// update_docs43.js - docs/43 (gecmis yeniden yazimi) turu kapanis guncellemeleri.
// Kalip: update_docs40/41/42.js. Calistir: node BuildLog/denetim/update_docs43.js
const fs = require('fs');
const now = new Date();
const p2 = (n) => String(n).padStart(2, '0');
const ts = p2(now.getHours()) + ':' + p2(now.getMinutes());
const stamp = '26.10.05 ' + ts;
const iso = '2026-10-05 ' + ts;

const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
const eol = ch.includes('\r\n') ? '\r\n' : '\n';
const anchor = '## [26.10.05 10:40]';
if (ch.split(anchor).length !== 2) throw new Error('CHANGELOG capa 10:40');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-05 10:40') throw new Error('sohbet uc: ' + sb.entries[0].ts);
for (const f of ['docs/43-GECMIS-YENIDEN-YAZIMI.md', 'BuildLog/denetim/rewrite_filter.js',
  'BuildLog/denetim/gecmis_yeniden_yazim_haritasi.txt']) {
  if (!fs.existsSync(f)) throw new Error('eksik: ' + f);
}

const L = [
  '## [' + stamp + '] Gecmis yeniden yazildi: boost + ClientBuild tarihten silindi (.git 776 -> 178 MB) (docs/43)',
  '',
  '**Ne yapildi**',
  '',
  'Kullanici karariyla tum gecmis (141 commit) yeniden yazildi;',
  '`Source/5.Main/boost_1_80_0/` (paket payi 134,7 MB) ve HEAD\'de zaten bulunmayan',
  'olu `ClientBuild_192.168.99.200/` (440,2 MB) hicbir commit\'te kalmayacak sekilde',
  'cikarildi. Yontem: `git fast-export --no-data` -> yeni',
  '`BuildLog/denetim/rewrite_filter.js` (yalnizca M/D satirlarini filtreler) ->',
  '`git fast-import`; blob\'lar yeniden yazilmaz, yalniz agac/commit nesneleri uretilir.',
  'Filtre 171.866 satir atti; hedef oneklere ait kalinti M/D satiri **0**.',
  '',
  '**Olcum**',
  '',
  '- `.git` **776 MB -> 178 MB** (-598 MB, -%77); pack 766,96 -> 176,27 MiB;',
  '  nesne 95.668 -> 9.942. Bos disk ~911 MB -> ~1,9 GB.',
  '- Commit/izlenen dosya sayisi ayni (141 / 11.840); **tip agaci SHA\'si birebir ayni**',
  '  (`a26d2bf7...`) -> calisma agaci ve derleme girdileri degismedi.',
  '',
  '**Dogrulama**',
  '',
  '- 141 eski<->yeni commit ciftinin tamami karsilastirildi; tum farklar iki hedef',
  '  onekte silme; `uyumsuz_cift=0`.',
  '- `git log --all -- <hedefler>` = 0; `git fsck --connectivity-only` exit 0.',
  '- Pre-push kancasi iki kez PASS (build_all 12/12 + 603 canli/negatif).',
  '- `main`: `0b9d9e96d` -> **`c592b5e5`**; uzak main `--force-with-lease` ile',
  '  guncellendi; yerel dal `cline/33893` ve `faz1-tamamlandi` tag\'i de hizalandi.',
  '  Eski->yeni SHA haritasi (141 satir):',
  '  `BuildLog/denetim/gecmis_yeniden_yazim_haritasi.txt`.',
  '',
  '**Rapor:** `docs/43-GECMIS-YENIDEN-YAZIMI.md`',
  '',
  '',
].join(eol);
ch = ch.replace(anchor, L + anchor);
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

const text = [
  'Gecmis yeniden yazildi: boost + olu ClientBuild agaclari tum tarihten silindi; .git 776 MB -> 178 MB (docs/43). (istek: "depo ve push boyutunu olculebilir sekilde kucult" - boost turu devami)',
  '',
  'YAPILANLAR',
  ' - Kullanici onayiyla 141 commit yeniden yazildi; `Source/5.Main/boost_1_80_0/` (134,7 MB paket payi) + olu `ClientBuild_192.168.99.200/` (440,2 MB) hicbir commit\'te kalmayacak sekilde cikarildi.',
  ' - Yeni arac `BuildLog/denetim/rewrite_filter.js`: `git fast-export --no-data` -> M/D satiri filtresi -> `git fast-import` (blob\'lar yeniden yazilmaz). 171.866 satir atildi; hedef kalintisi 0.',
  '',
  'OLCUM',
  ' - `.git` **776 MB -> 178 MB** (-598 MB, -%77); pack 766,96 -> 176,27 MiB; nesne 95.668 -> 9.942; bos disk ~911 MB -> ~1,9 GB.',
  ' - Commit sayisi 141, izlenen dosya 11.840, **tip agaci SHA\'si birebir ayni** -> calisma agaci/derleme girdileri degismedi.',
  '',
  'DOGRULAMA',
  ' - 141 eski<->yeni commit ciftinin tamami denetlendi: farklar yalnizca hedef agaclarda silme (uyumsuz cift 0).',
  ' - `git fsck --connectivity-only` temiz; pre-push kancasi iki kez PASS (12/12 + 603).',
  ' - main `0b9d9e96d` -> `c592b5e5`, uzak force-with-lease ile guncel; eski->yeni SHA haritasi `BuildLog/denetim/gecmis_yeniden_yazim_haritasi.txt`.',
  '',
  'NOT / RISK',
  ' - Eski SHA\'lar yerelde yok; diger klonlar `git fetch --all && git reset --hard origin/main` yapmali (veya yeniden klonlamali). GitHub depo boyutu sunucu GC\'sine kadar gecikmeli duser.',
  ' - Ayrinti ve sinirlar: docs/43-GECMIS-YENIDEN-YAZIMI.md',
].join('\n');
sb.entries.unshift({ ts: iso, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = iso;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify({ updated: iso, status: 'ok', text }, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
