// update_docs42.js - docs/42 (boost yerel bagimlilik) turu kapanis guncellemeleri.
// Kalip: update_docs40.js / update_docs41.js. Calistir: node BuildLog/denetim/update_docs42.js
const fs = require('fs');
const now = new Date();
const p2 = (n) => String(n).padStart(2, '0');
const ts = p2(now.getHours()) + ':' + p2(now.getMinutes());
const stamp = '26.10.05 ' + ts;              // CHANGELOG basligi
const iso = '2026-10-05 ' + ts;              // pano
function rep(s, a, b, n, label) {
  const c = s.split(a).length - 1;
  if (c !== n) throw new Error(label + ' capa=' + c);
  return s.split(a).join(b);
}

// --- dogrulamalar (yazimlardan once) ---
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
const eol = ch.includes('\r\n') ? '\r\n' : '\n';
if (ch.split('## [26.10.05 08:20]').length !== 2) throw new Error('CHANGELOG capa 08:20');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-05 08:20') throw new Error('sohbet uc: ' + sb.entries[0].ts);
if (!fs.existsSync('docs/42-BOOST-YEREL-BAGIMLILIK.md')) throw new Error('docs/42 yok');
if (!fs.existsSync('BuildLog/denetim/fetch_boost.sh')) throw new Error('fetch_boost.sh yok');
const gi = fs.readFileSync('.gitignore', 'utf8');
if (!gi.includes('Source/5.Main/boost_1_80_0/')) throw new Error('.gitignore boost kurali yok');

// --- CHANGELOG ---
const L = [
  '## [' + stamp + '] Boost yerel bagimlilik: 68.475 dosya izlemeden cikarildi (docs/42)',
  '',
  '**Ne yapildi**',
  '',
  '`Source/5.Main/boost_1_80_0/` (847 MB, 68.475 dosya - depodaki tum izlenen',
  'dosyalarin %85\'i) `git rm -r --cached` ile izlemeden cikarildi; **diskteki kopya',
  'korundu** (istemci derlemesi bozulmadi). `.gitignore` kurali docs/41 turunda',
  'zaten eklenmisti. Yerine indirilebilir/yerel bagimlilik geldi:',
  '',
  '- `BuildLog/denetim/fetch_boost.sh` (YENI): resmi arsivden indirir',
  '  (`archives.boost.io/.../boost_1_80_0.tar.gz`, 136.670.223 bayt, sha256',
  '  `4b2136f98bdd1f5857f1c3dea9ac2018effe65286cf251534b6ae20cc45e1847`), dogrular ve',
  '  `Source/5.Main/boost_1_80_0/` altina acar (idempotent; `--force`, `BOOST_DEST`).',
  '- `BuildLog/denetim/build_all.sh`: istemci hedefinden once "yerel boost include',
  '  var mi" on kosul kapisi; yoksa hedefler baslamadan exit 1 + fetch onerisi.',
  '- `.gitignore`: boost kuralinin aciklamasina geri yukleme ipucu eklendi.',
  '',
  '**Olcum**',
  '',
  '- Izlenen dosya: **80.312 -> 11.837** (-68.475, %85); izlenen ham bayt -651,6 MB.',
  '- `.git` 785 MB: degismedi - boost blob\'lari gecmiste duruyor; gercek kuculme',
  '  history rewrite ister (kullanici karari). Paketteki boost payi **134,7 MB**;',
  '  ayrica HEAD\'de bulunmayan `ClientBuild_192.168.99.200/` ~440 MB pakette duruyor.',
  '',
  '**Dogrulama**',
  '',
  '- fetch_boost.sh **gercek kosum**: indirme + sha256 + cikarma (72.069 dosya /',
  '  847 MB, version 1_80) -> exit 0, 278 sn. Kosum sirasinda 2 gercek hata bulunup',
  '  duzeltildi (hedef ust dizini yokken disk kontrolu; `MSYS2_ARG_CONV_EXCL` altinda',
  '  yerel curl\'e POSIX yol verilmesi).',
  '- build_all on kosulu **negatif test**: marker gizlenince exit 1, 0 hedef basladi,',
  '  ozet `ONKOSUL HATA ... COZUM: fetch_boost.sh`; marker geri konuldu.',
  '- Arsiv karsilastirmasi: 68.235 dosya birebir ayni; 83 satir-sonu farki',
  '  (autocrlf normalize); 156 gercek fark - 155 dokuman/test + yalnizca',
  '  `dynamic_bitset.hpp` basligi (proje kodunda kullanilmiyor).',
  '- Pre-push kancasi: build_all 12/12 + 603 kapisi canli/negatif GECTI.',
  '',
  '**Rapor:** `docs/42-BOOST-YEREL-BAGIMLILIK.md`',
  '',
  '',
].join(eol);
ch = ch.replace('## [26.10.05 08:20]', L + '## [26.10.05 08:20]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// --- PANO ---
const text = [
  'Boost surum kontrolunden cikarildi: 68.475 dosya izlendi -> 0; yerel/indirilebilir bagimlilik (docs/42). (istek: "boost kutuphanesini surum kontrolunden cikarip indirilebilir/yerel bagimliliga donustur; depo ve push boyutunu olculebilir sekilde kucult")',
  '',
  'YAPILANLAR',
  ' - `Source/5.Main/boost_1_80_0/` (847 MB, depodaki izlenen dosyalarin %85\'i) `git rm -r --cached` ile izlemeden cikarildi; diskteki kopya KORUNDU, istemci derlemesi bozulmadi.',
  ' - Yerine `BuildLog/denetim/fetch_boost.sh` geldi: resmi arsiv (archives.boost.io, sha256 `4b2136f98b...`) indirir + dogrular + acar; idempotent, `--force`/`BOOST_DEST` destekli.',
  ' - `build_all.sh` on kosul kapisi: yerel boost yoksa istemci hedefi hic baslamaz, exit 1 + fetch onerisi. `.gitignore` aciklamasina geri yukleme ipucu.',
  '',
  'OLCUM',
  ' - Izlenen dosya **80.312 -> 11.837** (-68.475, %85); izlenen ham bayt -651,6 MB.',
  ' - `.git` 785 MB degismedi (blob\'lar gecmiste); gercek kuculme icin history rewrite gerekir - pakette boost payi 134,7 MB, ayrica HEAD\'de olmayan `ClientBuild_192.168.99.200/` ~440 MB. Karar kullaniciya birakildi.',
  '',
  'DOGRULAMA',
  ' - fetch_boost.sh gercek kosum: sha256 + 72.069 dosya/847 MB cikarma -> exit 0 (278 sn); kosumda 2 gercek hata bulunup duzeltildi (disk kontrolu, curl yol cevrimi).',
  ' - On kosul negatif testi: marker gizlenince exit 1 ve 0 hedef basladi.',
  ' - Arsiv karsilastirmasi: 68.235 dosya birebir ayni, 83 satir-sonu farki; 156 gercek fark (155 dokuman/test + kullanilmayan `dynamic_bitset.hpp`).',
  ' - Pre-push kancasi: build_all 12/12 + 603 kapisi canli/negatif GECTI.',
  '',
  'Not: Temiz klonda kurulum tek adim: `bash BuildLog/denetim/fetch_boost.sh` (~5 dk). Ayrinti: docs/42.',
].join('\n');
sb.entries.unshift({ ts: iso, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = iso;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify({ updated: iso, status: 'ok', text }, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);

// --- .gitignore ipucu ---
const anchorGi = '# \u2500\u2500 \u00dc\u00e7\u00fcnc\u00fc taraf k\u00fct\u00fcphaneler (depoya al\u0131nmaz; ~850 MB) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500';
if (!gi.includes(anchorGi)) throw new Error('.gitignore boost aciklama capasi yok');
const giNew = rep(
  gi,
  anchorGi + '\nSource/5.Main/boost_1_80_0/',
  anchorGi + '\n#    Geri y\u00fckleme (temiz klon, bir kez): bash BuildLog/denetim/fetch_boost.sh  (docs/42)\nSource/5.Main/boost_1_80_0/',
  1,
  '.gitignore boost blogu'
);
fs.writeFileSync('.gitignore', giNew, 'utf8');
console.log('.gitignore ipucu eklendi');
