const fs = require('fs');

// 1) CHANGELOG: satir basi fazladan bosluk duzeltmesi
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
const bad = ' yeniden doldurdu';
if (ch.split(bad).length - 1 !== 1) throw new Error('CHANGELOG bosluk capa=' + (ch.split(bad).length - 1));
ch = ch.split(bad).join('yeniden doldurdu');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG bosluk duzeltildi');

// 2) PANO: 08:20 girisinin sonuna duzeltme blogu
const sf = 'Dashboard/data/sohbet.json';
const xf = 'Dashboard/data/sonuc.json';
const sb = JSON.parse(fs.readFileSync(sf, 'utf8'));
if (sb.entries[0].ts !== '2026-10-05 08:20') throw new Error('pano uc: ' + sb.entries[0].ts);
if (sb.entries[0].text.includes('DUZELTME (push #1)')) throw new Error('duzeltme zaten var');

const ek = [
'',
'DUZELTME (push #1 - kanca 7/12)',
' - Tam yeniden derleme disk i yeniden doldurdu: cs603/js603/ds603/gs603 "Diskte yeterli yer yok" (FTK1005/MSB6003/C1085) ile dustu. %TEMP% temizligi 711 MB acti (6669 dosya, >1 gunluk); ayni kosumda EX803 CS/DS/JS + layout 603/803 gecti.',
' - SILINEN Source/Util/cryptopp/Release/cryptlib.lib gercekte LINK GIRDISIYDI (GS stdafx.h icindeki #pragma comment(lib,...)); vcxproj taramasinda gorunmedigi icin "olu kopya" sanilmisti. gs603/gs803 LNK1104 ile dustu; cryptlib.vcxproj (Release/Win32/v143) ile lib yeniden uretildi.',
' - Ders: "referanssiz" kararinda #pragma comment(lib / #include / linker satirlari da taranmali. Yeni yardimci: BuildLog/denetim/temp_cleanup.sh.',
].join('\n');

sb.entries[0].text = sb.entries[0].text + ek;
sb.updated = '2026-10-05 08:45';
fs.writeFileSync(sf, JSON.stringify(sb, null, 2), 'utf8');
const xs = JSON.parse(fs.readFileSync(xf, 'utf8'));
xs.updated = '2026-10-05 08:45';
xs.text = sb.entries[0].text;
fs.writeFileSync(xf, JSON.stringify(xs, null, 2), 'utf8');
console.log('pano duzeltmesi eklendi');
