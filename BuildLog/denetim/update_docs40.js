const fs = require('fs');
const ts = '2026-10-05 07:20';
function rep(s, a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); return s.split(a).join(b); }

// --- dogrulamalar (yazimlardan once) ---
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
const anchor = '## [26.10.04 20:02]';
if (ch.split(anchor).length !== 2) throw new Error('CHANGELOG capa 20:02');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-04 20:02') throw new Error('sohbet uc: ' + sb.entries[0].ts);
if (!fs.existsSync('docs/40-H018-RUNTIME-DOGRULAMA.md')) throw new Error('docs/40 yok');
const xs = JSON.parse(fs.readFileSync('BuildLog/h018/capture/runtime_spec_crosscheck.json', 'utf8'));
if (xs.verdict !== 'PASS') throw new Error('crosscheck verdict=' + xs.verdict);

// --- CHANGELOG ---
const entry = [
'## [26.10.05 07:20] H-018 runtime dogrulamasi: giris/cikis sahnesi + paket duzeni (docs/40)',
'',
'**Ne yapildi**',
'',
'EX603 GameServer + yeni Main ile uctan uca sahne testi gecirildi: sunucu secimi',
'-> GS baglantisi -> giris -> karakter listesi -> MAIN_SCENE (dunya) -> cikis(1) ->',
'karakter sahnesi -> cikis(2) -> giris sahnesi. Karakter listesinin bos donme',
'koku nedeni bulundu: DataServer listeyi `AccountCharacter.GameID1..5`',
'slotlarindan kuruyor; `e2etest` satirinda slotlar NULL idi (karakter `Character`',
'tablosunda duruyordu ama hesaba bagli degildi). Slot dolduruldu.',
'',
'**Dogrulama**',
'',
'- `H018DRV.log`: SONUC=PASS rc=0; asama0..6 tamam, MAIN_SCENE yuklendi',
'  (`canli_oyuncu=2`), istemci WM_CLOSE ile temiz kapandi.',
'- Runtime paketleri <-> layout spec: **13/13 PASS**. Iki bagimsiz kayit',
'  (istemci duz metni `H018DRV_recv.bin` + proxy tel kaydi `s2c.bin`, K1/K2 ile',
'  cozuldu) karsilastirildi; ikisi de 72 cerceve / 0 sapan ve birbiriyle bayt',
'  duzeyinde ayni. PLAYER 36 B (name@+22=H018Test, count@+35), MONSTER 20 B',
'  (CurHp@+9=100, count@+16).',
'- Statik: `node BuildLog/2e7/viewport_layout.js 603 0` -> TUM PAKETLER HIZALI.',
'- Arac duzeltmesi: `parse_gs_wire.js` iniVal() regex hatasi (tek backslash ->',
'  `^s*`) giderildi; araç artik CustomerName/ServerSerial okuyabiliyor.',
'',
'**Rapor:** `docs/40-H018-RUNTIME-DOGRULAMA.md`',
'',
'',
].join('\n');
ch = ch.replace(anchor, entry + anchor);
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// --- PANO ---
const text = [
'H-018 runtime dogrulamasi: giris/cikis sahnesi + paket duzeni (docs/40). (istek: "EX603 GameServer + yeni Main.exe ile giris/cikis sahnesi testini yapip H-018 viewport paket duzeni runtime\'da dogrula")',
'',
'YAPILANLAR',
' - Yerel test yigini ayakta dogrulandi (CS 44405 / proxy 55901 / GS 55902 / DS 63002 / JS 63003); deploy GS md5 = MuServer EX603 ikilisi ile ayni.',
' - Bos karakter listesinin koku: DataServer listeyi `AccountCharacter.GameID1..5` slotlarindan kuruyor; `e2etest` slotlari NULL idi. `fix_account_char.sql` ile slot dolduruldu.',
' - Surucu (Main_h018.exe) uctan uca gecti: sunucu secimi -> GS -> giris -> karakter (`H018Test`) -> MAIN_SCENE dunya -> cikis(1) -> karakter sahnesi -> cikis(2) -> giris sahnesi; SONUC=PASS rc=0.',
'',
'DOGRULAMA',
' - Runtime paketleri <-> layout spec (603/0): **13/13 PASS**. Iki bagimsiz kayit (istemci duz metni + proxy teli, K1/K2=0x3F/0x24) birebir ayni; 72 cerceve / 0 sapan.',
' - PLAYER 36 B (name@+22="H018Test", count@+35=0), MONSTER 20 B (CurHp@+9=100, Level@+10, Life@+12, count@+16=0).',
' - Statik capraz kontrol: `viewport_layout.js 603 0` -> TUM PAKETLER HIZALI. Arac hatasi (parse_gs_wire.js iniVal) duzeltildi.',
'',
'KALAN',
' - CHANGE/SUMMON runtime kaniti (donusum/pet sahnesi ile tetiklenmeli); spec tarafi hazir.',
' - K1 (EX803 dagitim yolu) ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/40-H018-RUNTIME-DOGRULAMA.md - BuildLog/h018/capture/runtime_spec_crosscheck.txt - BuildLog/h018/client/H018DRV.log',
].join('\n');
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify({ updated: ts, status: 'ok', text }, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
