const fs = require('fs');
const ts = '2026-10-05 08:20';
function rep(s, a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); return s.split(a).join(b); }

// --- dogrulamalar (yazimlardan once) ---
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
const anchor = '## [26.10.05 07:20]';
if (ch.split(anchor).length !== 2) throw new Error('CHANGELOG capa 07:20');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-05 07:20') throw new Error('sohbet uc: ' + sb.entries[0].ts);
if (!fs.existsSync('docs/41-GEREKSIZ-DOSYA-TEMIZLIGI.md')) throw new Error('docs/41 yok');
const gi = fs.readFileSync('.gitignore', 'utf8');
if (!gi.includes('*.iobj') || !gi.includes('Source/*/Release/')) throw new Error('.gitignore kurallari eksik');

// --- CHANGELOG ---
const entry = [
'## [26.10.05 08:20] Gereksiz dosya temizligi: 1067 MB + .gitignore kalici duzeltmesi (docs/41)',
'',
'**Ne yapildi**',
'',
'Disk %100 doluyken (208 MB bos) izlenmeyen uretim artiklari temizlendi:',
'derleme ara dizinleri (`BuildLog/5Main,4GS,2DS,3JS,1CS`, Getmain objs), EX803',
'cikti agaclari (`Source/*/Release`, olu `Source/Util/cryptopp/Release`,',
'`MuServe Classic 5.2 Lorencia/`), h018 surucu sembolleri/ekran goruntuleri,',
'istemci `Main.pdb` + bayat `ClientFile/Main`, Android/Gradle artiklari.',
'Her hedefte once `git ls-files` ile izlenen dosya sayisi dogrulandi; 4GS (8) ve',
'Getmain (2) icin `rm -rf` yerine `git clean -fdx` kullanildi.',
'',
'**Olcum**',
'',
'- Bos disk: 208 MB -> **1286 MB** (kazanc **1067 MB**); `BuildLog/` 671M->147M,',
'  `android/` 175M->65M, `Source/1..4`+`Util` ~370M->11M.',
'- `git status` izlenmeyen gurultu: 1384 -> 96 girdi. I**zlenen dosya silinmedi**',
'  (yalniz onceden var olan ` M ClientFile/Main.exe` kalir).',
'- Calisan E2E yigini (CS/DS/JS/GS, `BuildLog/h018/deploy`) etkilenmedi.',
'',
'**Kalici duzeltme**',
'',
'`.gitignore`: `*.iobj`, `*.ipdb`, `*.pch`, `*.tlog/`, `*.recipe`,',
'`Source/*/Release/`, `MuServe Classic 5.2 Lorencia/`, `BuildLog/{1CS,2DS,3JS,4GS,5Main,Getmain,github}/`,',
'`BuildLog/h018/{drvout,deploy}/`, `BuildLog/h018/vinput/obj/` -> docs/33 §4 H1 kapandi.',
'',
'**Not:** Derleme onbellegi silindigi icin bir sonraki push oncesi pre-push kancasi',
'tam yeniden derleme yapar (artik ignore\'lu, ~700 MB alan).',
'',
'**Rapor:** `docs/41-GEREKSIZ-DOSYA-TEMIZLIGI.md`',
'',
'',
].join('\n');
ch = ch.replace(anchor, entry + anchor);
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// --- PANO ---
const text = [
'Gereksiz dosya temizligi: 1067 MB kazanc + .gitignore kalici duzeltmesi (docs/41). (istek: "gereksiz ve kullanilmayan dosya temizligi yaparmisin")',
'',
'YAPILANLAR',
' - Disk %100 doluydu (208 MB bos) - temizlik zorunluydu. Izlenmeyen uretim artiklari silindi: BuildLog ara dizinleri (5Main/4GS/2DS/3JS/1CS + Getmain obj), Source/*/Release (EX803), olu Source/Util/cryptopp/Release, "MuServe Classic 5.2 Lorencia", h018 surucu pdb/map + 80+ ekran goruntusu, ClientFile/Main.pdb + bayat ClientFile/Main, android .gradle/.idea/captures/jniLibs-x86.',
' - Guvenlik: her hedefte once `git ls-files` ile izlenen dosya sayildi; 4GS (8 izlenen) ve Getmain (2 izlenen) icin rm -rf yerine `git clean -fdx` kullanildi.',
'',
'OLCUM',
' - Bos disk 208 MB -> **1286 MB** (kazanc **1067 MB**); BuildLog 671M->147M, android 175M->65M.',
' - git status izlenmeyen gurultusu 1384 -> 96. **Izlenen dosya silinmedi**; izlenen tarafta yalniz onceden var olan ` M ClientFile/Main.exe`.',
' - Calisan E2E yigini (CS/DS/JS/GS, BuildLog/h018/deploy) etkilenmedi; tel kanitlari (s2c.bin, marks.jsonl, crosscheck.*) ve denetim obj leri korundu.',
'',
'KALICI DUZELTME',
' - .gitignore: *.iobj, *.ipdb, *.pch, *.tlog/, *.recipe, Source/*/Release/, MuServe Classic 5.2 Lorencia/, BuildLog uretim dizinleri -> docs/33 §4 H1 (izlenmeyen uretim artiklari) kapandi.',
'',
'ETKI',
' - Derleme onbellegi silindigi icin bir sonraki push oncesi pre-push kancasi tam yeniden derleme yapar (artik ignore lu; ~700 MB alan gerekir).',
' - Android emulator (x86) kullanilacaksa jniLibs/x86 yeniden indirilmeli; arm64/armeabi cihaz derlemesi etkilenmedi.',
'',
'Kanit: docs/41-GEREKSIZ-DOSYA-TEMIZLIGI.md - /tmp/cleanup_unused.log (hedef bazli boyut/rc)',
].join('\n');
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify({ updated: ts, status: 'ok', text }, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
