const fs = require('fs');
const ts = '2026-10-04 14:15';
function rep(s, a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); return s.split(a).join(b); }

// ---- docs/38 ----
const p38 = 'docs/38-TOPLU-DERLEME-SCRIPTI.md';
let d = fs.readFileSync(p38, 'latin1');
d = rep(d, '> Tarih: 2026-10-04 14:07',
  '> Tarih: 2026-10-04 14:15 (ilk kosu 13:59; --help duzeltmesi sonrasi kanit loglari\n> 14:11 kosusuyla yeniden uretildi)', 1, 'd38-tarih');
d = rep(d, '## Test kosusu (04.10 13:59) - 12/12 PASS, SCRIPT_EXIT=0',
  '## Test kosusu (04.10 14:11; ilk kosu 13:59) - 12/12 PASS, SCRIPT_EXIT=0', 1, 'd38-baslik');
d = rep(d, '| client | PASS | 12.034.048 | a851551d69d0e69b063af693bf0660ee |',
  '| client | PASS | 12.034.048 | 91fa8da9d5e4b57f863504069d0e4f61 |', 1, 'd38-client');
d = rep(d, '| GS config makrolari | PASS | - | EX603=603+HAISLOTRING=0, EX803=803 |',
  '| GS config makrolari | PASS | - | EX603=603+HAISLOTRING=0, EX803=803 |\n\n' +
  'Not: client 13:59 kosusunda tam yeniden derlenip `a851551d...` uretti (yalniz 13 bayt\n' +
  'meta); docs/36 geregi HEAD ikilisi (`91fa8da9...`) korundu. 14:11 kosusu MSBuild\n' +
  'linkini atladi ve ozet depodaki ikiliyi dogruladi.', 1, 'd38-not');
d = rep(d, '## Script testinde yakalanan 2 hata (duzeltildi)',
  '## Script testinde yakalanan 3 hata (duzeltildi)', 1, 'd38-hata-baslik');
d = rep(d, '   cozumler; script repo kokune `cd` ettigi icin goreli yol kullanildi.',
  '   cozumler; script repo kokune `cd` ettigi icin goreli yol kullanildi.\n' +
  '3. `--help` ve bilinmeyen arguman, arg ayristirmadan ONCE log dosyalarini\n' +
  '   sifirliyordu; 14:07 cagrisi kanit loglarini bosaltti (0 bayt). Arg ayristirma\n' +
  '   log kirpmasindan onceye alindi. Dogrulama: `--help` oncesi/sonrasi log md5 leri\n' +
  '   birebir ayni (d18c958c / 710b355c / 154a02ab), HELP_EXIT=0; `--bogus` ise\n' +
  '   BOGUS_EXIT=1 ve loglara yine dokunmuyor. Kanit loglari 14:11 kosusuyla\n' +
  '   yeniden uretildi.', 1, 'd38-hata3');
fs.writeFileSync(p38, d, 'latin1');
console.log('docs/38 guncellendi');

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 14:15]').length !== 1) throw new Error('CHANGELOG cipasi (yeni)');
if (ch.split('## [26.10.04 14:07] Tek komutlu dogrulanmis build script i:').length !== 2) throw new Error('CHANGELOG cipasi (eski)');
const entry = [
'## [26.10.04 14:15] build_all.sh --help duzeltmesi: log dosyalari artik sifirlanmiyor',
'',
'**Ne yapildi**',
'',
'`--help` ve bilinmeyen arguman yollarinda arg ayristirma, log kirpmasindan ONCEYE',
'alindi; boylece yardim / yanlis kullanim cagrilari kanit loglarini bosaltmiyor.',
'Kanit loglari (`build_all.log`, `build_all_summary.txt`, `build_all_console.log`)',
'duzeltilmis script ile 14:11 kosusunda yeniden uretildi: **12/12 PASS, SCRIPT_EXIT=0**.',
'',
'**Dogrulama**',
'',
'- `bash BuildLog/denetim/build_all.sh --help` oncesi/sonrasi log md5 leri birebir',
'  ayni: log `d18c958c`, ozet `710b355c`, konsol `154a02ab`; `HELP_EXIT=0`.',
'- Bilinmeyen arguman (`--bogus`): `BOGUS_EXIT=1`, loglar yine degismedi.',
'- 14:11 ozeti: client 12.034.048 / `91fa8da9...`, cs603 103.936 / `516d6a49...`,',
'  js603 943.616 / `4698cff7...`, ds603 1.034.240 / `4fe2cda2...`, gs603 10.656.256 /',
'  `02f695e2...`; cs803 `7e5775f8...`, js803 `504a1908...`, ds803 `dfddd76d...`,',
'  gs803 `d1cd380b...`; layout 603/0 + 803/1 "TUM PAKETLER HIZALI".',
'',
'**Rapor:** `docs/38-TOPLU-DERLEME-SCRIPTI.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 14:07] Tek komutlu dogrulanmis build script i:', entry + '## [26.10.04 14:07] Tek komutlu dogrulanmis build script i:');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'Tek komutlu build script - --help duzeltmesi (docs/38). (istek: "Sunucu + istemci derlemelerini tek komutla, dogrulamali ve log\'lu calistiran bir build script\'i yaz (EX603 istemci + EX603/EX803 sunucular)")',
'',
'YAPILANLAR',
' - `--help` ve bilinmeyen arguman, arg ayristirmadan ONCE log dosyalarini sifirliyordu; 14:07 cagrisi kanit loglarini bosaltti (0 bayt). Arg ayristirma log kirpmasindan onceye alindi.',
' - Kanit loglari duzeltilmis script ile 14:11 kosusunda yeniden uretildi: 12/12 PASS, SCRIPT_EXIT=0 (build_all.log 18.229 B, ozet 1.336 B).',
' - docs/38 + CHANGELOG guncellendi; script davranisi degismedi (yalniz sirali arg ayristirma).',
'',
'DOGRULAMA',
' - --help oncesi/sonrasi log md5 leri ayni (d18c958c / 710b355c / 154a02ab), HELP_EXIT=0; --bogus BOGUS_EXIT=1 ve loglara dokunmuyor.',
' - 14:11 ozeti: client 91fa8da9 (12.034.048), gs603 02f695e2 (10.656.256), gs803 d1cd380b (11.294.720); layout 603/0 + 803/1 HIZALI.',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + istemci giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/38-TOPLU-DERLEME-SCRIPTI.md - BuildLog/denetim/build_all_summary.txt',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-04 14:07') throw new Error('sohbet uc kaydi beklenmedik: ' + sb.entries[0].ts);
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
