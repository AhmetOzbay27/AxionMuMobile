const fs = require('fs');
const ts = '2026-10-04 12:08';

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 12:08]').length !== 1) throw new Error('CHANGELOG cipasi (yeni)');
if (ch.split('## [26.10.04 06:50]').length !== 2) throw new Error('CHANGELOG cipasi (eski)');
const entry = [
'## [26.10.04 12:08] GetMain ikili karari: sahipsiz yeniden derleme HEAD e geri alindi (docs/36)',
'',
'**Ne yapildi**',
'',
'`GetMain/GetMainInfo.exe` + `GetMainInfo.pdb` (03.10 15:02 artigi) incelendi. Farkin',
'tamami derleme meta verisi (16 bayt = COFF ts 3B + 4x debug-dir ts 12B + RSDS age 4->5',
'1B); `.text` dahil tum kod bolumleri HEAD ile birebir ayni. Islevsel delta yok ->',
'`git checkout --` ile HEAD e geri alindi; ikililer zaten `32f5cdcb0` soydan geliyor.',
'',
'**Dogrulama**',
'',
'- `cmp -l`: tam 16 bayt fark (`BuildLog/denetim/getmain_cmp_exe.txt`).',
'- PE bolum denetimi (`BuildLog/denetim/pe_cmp.js`): fark yalniz HEADER=3 + .rdata=13;',
'  diger tum bolumler + dosya uzunlugu birebir ayni.',
'- Geri alma sonrasi md5 = HEAD: exe `c480e0ba...`, pdb `c1e9fe73...`; `git diff` bos.',
'- Yeniden derleme kopyasi `BuildLog/Getmain/GetMainInfo.rebuilt.{exe,pdb}` arsivde.',
'',
'**Kural:** Kaynak degismeden ikili yeniden derleme commit edilmez.',
'',
'**Rapor:** `docs/36-GETMAIN-IKILI-KARARI.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 06:50]', entry + '## [26.10.04 06:50]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'SAHIPSIZ GETMAIN IKILILERI KARARI (docs/36). (istek: "Sahipsiz GetMain ikililerini (GetMainInfo.exe/pdb) incele: yeniden derlenmeli mi, hangi commit e ait olmali, yoksa geri alinmali mi karar ver")',
'',
'YAPILANLAR',
' - GetMain/GetMainInfo.exe+pdb (03.10 15:02 artigi) incelendi: fark TAM 16 bayt = COFF ts 3B + 4x debug-dir ts 12B + RSDS age 4->5 1B; .text dahil tum kod bolumleri HEAD ile birebir ayni.',
' - Islevsel fark yok, kaynakta commit disi degisiklik yok, ikililer zaten 32f5cdcb0 soydan -> KARAR: HEAD e geri alindi (git checkout --).',
' - Yeniden derleme kopyasi BuildLog/Getmain/GetMainInfo.rebuilt.{exe,pdb} arsivlendi.',
'',
'DOGRULAMA',
' - cmp -l: 16 bayt (BuildLog/denetim/getmain_cmp_exe.txt); PE bolum denetimi (pe_cmp.js): fark yalniz HEADER=3 + .rdata=13.',
' - Geri alma sonrasi md5 = HEAD blogu (exe c480e0ba..., pdb c1e9fe73...); git diff bos.',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + yeni Main.exe ile giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/36-GETMAIN-IKILI-KARARI.md - BuildLog/denetim/',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
