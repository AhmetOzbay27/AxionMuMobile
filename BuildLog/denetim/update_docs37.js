const fs = require('fs');
const ts = '2026-10-04 13:33';

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 13:33]').length !== 1) throw new Error('CHANGELOG cipasi (yeni)');
if (ch.split('## [26.10.04 12:08]').length !== 2) throw new Error('CHANGELOG cipasi (eski)');
const entry = [
'## [26.10.04 13:33] 603 tel duzeni derleme-zamani kapisi: Main yalniz canli 603+0 derlenir (docs/37)',
'',
'**Ne yapildi**',
'',
'`verify_603_layout` denetimi urun kaynagina tasindi: `WSclient.cpp` icine `#error`',
'makro kilidi + 11 `static_assert` eklendi (tel govde 36/38/20/20, count ofsetleri',
'+35/+37/+19/+16, MONSTER CurHp/Level/Life +9/+10/+12). Bayat EX803 yorumlari',
'guncellendi (`WSclient.h` x3, `Defined_Global.h`).',
'',
'**Kapsam (kullanici karari)**',
'',
'Yalniz canli 603+0 kabul edilir; 603+1 / 803+1 / 803+0 ve digerleri C1189 ile kirilir.',
'EX803 istemci derlemesi artik bilincli kapi degisikligi gerektirir.',
'',
'**Dogrulama**',
'',
'- Canli makroyla tek dosya derleme: EXIT=0 (`BuildLog/denetim/gate_live603.log`).',
'- `/DGAMESERVER_HAISLOTRING=1` ve `/DGAMESERVER_UPDATE=803`: C1189 + EXIT=2',
'  (`gate_bad_hais1.log`, `gate_bad_803.log`).',
'- MSBuild Main Global Release|Win32 v143 (incremental): 0 error, EXIT=0',
'  (`main_gate_build.log`); WSclient.cpp yeniden derlendi, Main.exe linklendi.',
'- Yeni Main.exe vs HEAD: kod bolumleri ozdes (yalniz 13 bayt meta) -> HEAD e geri',
'  alindi (docs/36 kurali).',
'- Harici denetim `verify_603_layout.cpp` tekrar: EXIT=0.',
'',
'**Rapor:** `docs/37-603-DERLEME-KAPISI.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 12:08]', entry + '## [26.10.04 12:08]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'603 TEL DUZENI DERLEME KAPISI (docs/37). (istek: "603 tel duzeni dogrulamasini (verify_603_layout) Main projesine derleme-zamani kapisi olarak ekle; yanlis makroyla derleme kirilsin")',
'',
'YAPILANLAR',
' - verify_603_layout denetimi urun kaynagina tasindi: WSclient.cpp icinde #error makro kilidi + 11 static_assert (tel 36/38/20/20, count +35/+37/+19/+16, MONSTER +9/+10/+12).',
' - Kullanici karari: Main yalniz canli 603+0 (GAMESERVER_UPDATE=603 + HAISLOTRING=0) derlenir; diger kombinasyonlar C1189 ile kirilir. EX803/603+1 bilincli kapi degisikligi gerektirir.',
' - Bayat EX803 yorumlari guncellendi (WSclient.h x3, Defined_Global.h).',
'',
'DOGRULAMA',
' - Canli makro tek dosya: EXIT=0 (gate_live603.log); /DGAMESERVER_HAISLOTRING=1 ve /DGAMESERVER_UPDATE=803: C1189 + EXIT=2 (gate_bad_*.log).',
' - MSBuild Main Global Release|Win32 v143 incremental: 0 error, EXIT=0 (main_gate_build.log); yeni Main.exe vs HEAD kod ozdes (13 bayt meta) -> HEAD e geri alindi.',
' - Harici verify_603_layout.cpp tekrar EXIT=0; kapi metni wsclient_gate.block.txt.',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + yeni Main.exe ile giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/37-603-DERLEME-KAPISI.md - BuildLog/denetim/',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
