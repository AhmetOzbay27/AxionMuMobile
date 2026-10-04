const fs = require('fs');
const ts = '2026-10-04 06:50';

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 06:50]').length !== 1) throw new Error('CHANGELOG cipasi (yeni)');
if (ch.split('## [26.10.04 03:10]').length !== 2) throw new Error('CHANGELOG cipasi (eski)');
const entry = [
'## [26.10.04 06:50] Main 603 tam derlemesi EXIT=0 - K2 kapatildi (docs/35)',
'',
'**Ne yapildi**',
'',
'K2 (Main tam derlemesi) kapatildi; istemci canli SPK 5.2 tel duzeniyle (603+0)',
'derlenip `ClientFile/Main.exe` yenilendi. Duzeltmeler: (1) `SPKData.h` icinde iki',
'`#define` tek satirda yapisikti (SPK_CAMERA_FPS_OFFSET yorumun icinde kaliyordu) ->',
'ayrildi; (2) `SPKMenuBar.cpp` `DisplayWidth` -> `DisplayWin` (CBInterface.h:21);',
'(3) `SPKMenuBar.cpp` icin `NewUISystem.h` include edildi (g_pBCustomMenuInfo makrosu,',
'CBChoTroi.cpp ile ayni desen). Ayrica bozuk `BuildLog/5Main/vc143.pdb` (C1033) kenara',
'alindi (kilitli degil, bozuktu); build PDB yi sifirdan yazdi.',
'',
'**Dogrulama**',
'',
'- MSBuild Global Release|Win32 v143 /m: 407 .cpp, 0 error, EXIT=0',
'  (`BuildLog/denetim/main_603_build.log`); 40 LNK4099 (detours.pdb) + 1 MSB8004',
'  disinda uyari yok.',
'- Yeni ikili: `ClientFile/Main.exe` 12.034.048 B, md5 91fa8da9d5e4b57f863504069d0e4f61',
'  (onceki 12.031.488 B / c49383bf...). Icinde 3C.0 etiketleri var("Etkinlik Saati"',
'  vb.) -> kod baglandi.',
'- 603 kilit: proje tlog unda GAMESERVER_UPDATE/HAISLOTRING override i YOK;',
'  `verify_603_layout.cpp` proje bayraklariyla EXIT=0 -> 603/0 + tel govde',
'  36/38/20/20 + count@+35/+37/+19/+16 + MONSTER +9/+10/+12 (canli PDB ile ayni).',
'',
'**Degisen dosyalar**',
'',
'`Source/5.Main/source/{SPKData.h,SPKMenuBar.cpp}` (K2 duzeltmeleri)',
'`Source/5.Main/source/{SPKData.cpp,SPKMenuBar.h,CBInterface.cpp}` + `Main.vcxproj(.filters)`',
'(3C.0 ajanindan devralinan, derleme icin zorunlu; ilk kez commit li)',
'`ClientFile/Main.exe` (izlenen ikili guncellendi)',
'`BuildLog/denetim/*` (build + verify kanitlari), `docs/35-MAIN-603-DERLEME.md`',
'',
'**Rapor:** `docs/35-MAIN-603-DERLEME.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 03:10]', entry + '## [26.10.04 03:10]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'MAIN 603 TAM DERLEMESI (docs/35). (istek: "Main tam derlemesini EXIT=0 yap; SPKData.h/SPKMenuBar.cpp hatalarini gider ve istemciyi 603 duzeniyle derleyip Main.exe uret")',
'',
'YAPILANLAR',
' - K2 kapatildi: SPKData.h:40 yapisik #define ikiye bolundu (SPK_CAMERA_FPS_OFFSET tanimsizdi -> C2065 + memcpy C2660).',
' - SPKMenuBar.cpp: DisplayWidth -> DisplayWin (CBInterface.h:21; MenuCustom.cpp ile ayni desen); NewUISystem.h include edildi (g_pBCustomMenuInfo makrosu).',
' - vc143.pdb C1033: dosya kilitli degil BOZUKTU; kenara tasindi, build PDB y i sifirdan yazdi.',
' - Main "Global Release" v143 tam derleme: 407 .cpp, 0 error, EXIT=0. Yeni ClientFile/Main.exe 12.034.048 B, md5 91fa8da9 (onceki 12.031.488 B).',
' - 3C.0 dosyalari (SPKData.cpp/SPKMenuBar.*/CBInterface.cpp/Main.vcxproj) ilk kez commit edildi; kod ikilide dogrulandi (etiket dizeleri Main.exe icinde).',
'',
'DOGRULAMA',
' - Build logu: BuildLog/denetim/main_603_build.log (455 satir, 0 error; 40 LNK4099 detours + 1 MSB8004 zararsiz).',
' - 603 kilit: tlog da GAMESERVER_UPDATE/HAISLOTRING override i YOK; verify_603_layout.cpp proje bayraklariyla EXIT=0 -> 603/0, tel govde 36/38/20/20, count@+35/+37/+19/+16, MONSTER CurHp/Level/Life +9/+10/+12 (canli PDB ile ayni).',
' - NOT: istemci struct sizeof 68/70/52/52 BIR HATA DEGIL (sabit govde + s_BuffCount buff kuyrugu; ilerletme sizeof-(32-count)).',
'',
'KALAN',
' - H-018 runtime testi artik mumkun (EX603 GameServer + yeni Main.exe).',
' - GetMain ikilileri sahipsiz/commit disi; K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/35-MAIN-603-DERLEME.md - BuildLog/denetim/',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
// eski kayitlardaki kacis kusuru: \u0004 -> "\4" (MuServer\4.GameServer...)
let fixed = 0;
for (const e of sb.entries) if (e.text.indexOf('\u0004') >= 0) { e.text = e.text.split('\u0004').join('\4'); fixed++; }
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length + '; eski kayit kacis duzeltmesi=' + fixed);
