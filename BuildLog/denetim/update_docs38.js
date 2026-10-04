const fs = require('fs');
const ts = '2026-10-04 14:07';

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 14:07]').length !== 1) throw new Error('CHANGELOG cipasi (yeni)');
if (ch.split('## [26.10.04 13:33]').length !== 2) throw new Error('CHANGELOG cipasi (eski)');
const entry = [
'## [26.10.04 14:07] Tek komutlu dogrulanmis build script i: EX603 istemci + EX603/EX803 sunucu (docs/38)',
'',
'**Ne yapildi**',
'',
'`BuildLog/denetim/build_all.sh` yeniden yazildi: 9 derleme (Main + CS/JS/DS/GS x',
'EX603/EX803), hedef bazli loglar, tel duzeni + GS config makro dogrulamalari ve',
"md5 li PASS/FAIL ozeti; exit kodu sonucu yansitir. Test kosusu: 12/12 PASS, EXIT=0.",
'',
'**Bulgular (kosu)**',
'',
'- cs603/js603/ds603 izlenen ikilileri bayatti (30.09; docs/34 UUID duzeltmesi yoktu):',
'  yeniden derleme kod farki getirdi (CS .text 23 B, JS .text 31 B, DS +3.584 B) ->',
'  ikililer guncellendi.',
'- Main.exe kod ozdes (13 bayt meta) -> HEAD e geri alindi (docs/36 kurali).',
'- gs603 zaten gunceldi; degismedi.',
'',
'**Rapor:** `docs/38-TOPLU-DERLEME-SCRIPTI.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 13:33]', entry + '## [26.10.04 13:33]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'TEK KOMUTLU DOGRULANMIS BUILD SCRIPT (docs/38). (istek: "Sunucu + istemci derlemelerini tek komutla, dogrulamali ve log\'lu calistiran bir build script\'i yaz (EX603 istemci + EX603/EX803 sunucular)")',
'',
'YAPILANLAR',
' - BuildLog/denetim/build_all.sh yeniden yazildi: 9 derleme (Main + CS/JS/DS/GS x EX603/EX803), hedef bazli loglar, tel duzeni + GS config makro dogrulamasi, md5\'li PASS/FAIL ozeti; exit kodu sonucu yansitir.',
' - Test kosusu: 12/12 PASS, EXIT=0 (ozet: build_all_summary.txt; tam log: build_all.log).',
' - Eski script (yalniz EX803 yigini + Main, dogrulamasiz) bu surumle degisti.',
'',
'BULGULAR (kosu)',
' - cs603/js603/ds603 izlenen ikilileri bayatti (30.09; docs/34 UUID duzeltmesi yoktu): yeni derlemeler kod farki getirdi (CS/JS .text ~23-31 B, DS +3.584 B) -> ikililer guncellendi.',
' - Main.exe kod ozdes (13 bayt meta) -> HEAD e geri alindi (docs/36).',
' - Script testinde 2 hata yakalanip duzeltildi (bash ${u}_${h} genislemesi; node /c/ yolu).',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + istemci giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/38-TOPLU-DERLEME-SCRIPTI.md - BuildLog/denetim/',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
