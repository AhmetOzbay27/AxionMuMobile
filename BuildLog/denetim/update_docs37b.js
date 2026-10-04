const fs = require('fs');
const ts = '2026-10-04 13:33';
function rep(s, a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); return s.split(a).join(b); }

// --- docs/37: tablo satiri 4b + not ---
const p37 = 'docs/37-603-DERLEME-KAPISI.md';
let d = fs.readFileSync(p37, 'latin1');
d = rep(d, '| 4 | MSBuild Main "Global Release" Win32 v143 (incremental) | 0 error, EXIT=0 | `BuildLog/denetim/main_gate_build.log` |',
  '| 4 | MSBuild Main "Global Release" Win32 v143 (incremental) | 0 error, EXIT=0 | `BuildLog/denetim/main_gate_build.log` |\n' +
  '| 4b | MSBuild projesine enjekte yanlis makro (gecici Directory.Build.props + SelectedFiles) | C1189 + EXIT=1 | `BuildLog/denetim/gate_bad_msbuild.log` |', 1, 'docs37-tablo');
d = rep(d, '- Test 4: WSclient.cpp yeniden derlendi (obj 13:29) ve Main.exe linklendi; hata yok.',
  '- Test 4: WSclient.cpp yeniden derlendi (obj 13:29) ve Main.exe linklendi; hata yok.\n' +
  '- Test 4b: projeye yalniz WSclient.cpp icin `GAMESERVER_UPDATE=803` enjekte edildi (gecici\n' +
  '  `Directory.Build.props` + `/t:ClCompile /p:SelectedFiles`); MSBuild C1189 ile durdu\n' +
  '  (yalniz StdAfx.cpp + WSclient.cpp derlendi, link yok). Gecici props silindi.', 1, 'docs37-not');
fs.writeFileSync(p37, d, 'latin1');
console.log('docs/37 guncellendi');

// --- CHANGELOG: 13:33 girdisine 4b kaniti ---
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'latin1');
ch = rep(ch, '  (`main_gate_build.log`); WSclient.cpp yeniden derlendi, Main.exe linklendi.',
  '  (`main_gate_build.log`); WSclient.cpp yeniden derlendi, Main.exe linklendi.\n' +
  '- Projeye enjekte yanlis makro (gecici Directory.Build.props + SelectedFiles): C1189 +\n' +
  '  EXIT=1 (`gate_bad_msbuild.log`).', 1, 'CHANGELOG-4b');
fs.writeFileSync(cf, ch, 'latin1');
console.log('CHANGELOG guncellendi');

// --- PANO: ust kaydi ayni ts ile tazele ---
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
' - Projeye enjekte yanlis makro (gecici props + SelectedFiles): MSBuild C1189 + EXIT=1 (gate_bad_msbuild.log); harici verify_603_layout.cpp tekrar EXIT=0.',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + yeni Main.exe ile giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/37-603-DERLEME-KAPISI.md - BuildLog/denetim/',
].join('\n');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries[0].ts !== ts) throw new Error('pano cipasi');
sb.entries[0].text = text;
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = JSON.parse(fs.readFileSync('Dashboard/data/sonuc.json', 'utf8'));
so.text = text;
so.updated = ts;
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi');
