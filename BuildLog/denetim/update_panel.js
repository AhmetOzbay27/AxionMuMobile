const fs = require('fs');
const BS = String.fromCharCode(92);
const raw = [
'PROJE ÇAPINDA DENETİM (docs/33): 2 kritik + 4 orta bulgu. (istek: "bütün projeyi ve amaçları baştan aşağı tara, eksik/yanlış var mı")',
'',
'KRİTİK',
' K1 Dağıtım ikilileri bayat: EX803 derlemeleri dağıtım yoluna akmıyor — GS «BS»MuServe Classic 5.2 Lorencia«BS»GameServer (izlenmeyen), DS/JS/CS Source«BS»...«BS»Release. MuServer\'daki yığın 30.09-02.10 tarihli ve 2e.6-2e.9 düzeltmelerini içermiyor; 4 bileşenin md5\'i yeni derlemelerden farklı. H-018 sonrası Main.exe de yok → "çalışan çift" hâlâ H-018 öncesi.',
' K2 Main derlemesi kırık (3C.0 işi, commit\'siz): SPKData.h:40\'ta iki #define tek satıra yapışmış (SPK_CAMERA_FPS_OFFSET yorum içinde) → C2065+C2660; SPKMenuBar.cpp:158/181 DisplayWidth/g_pBCustomMenuInfo tanımsız; BuildLog«BS»5Main«BS»vc143.pdb C1033. MSBuild EXIT=1. Düzeltilmedi (başka ajanın dosyaları).',
'',
'ORTA',
' • docs/00-02-03 geride kalmıştı (2c.1 ve 2e.6-2e.9 yoktu) → bu turda güncellendi.',
' • docs/30-31 sayımları tablolarıyla uyuşmuyordu: GS 11→10, istemci 8→7, docs/31 6→5; docs/30\'un bozuk ham-veri yolu düzeltildi + denetim notları eklendi.',
' • GetMainInfo.exe 15:02\'de yeniden derlenmiş, commit\'siz (md5 1991c037… vs HEAD c480e0ba…).',
' • H-018 kaynakta 4/4 elle doğrulandı ama çalışan çiftte değil; viewport_layout.js dört alanı eşleyemeyip yine "HIZALI" diyor.',
'',
'DOĞRULANANLAR',
' • Sunucu hattı taze derleme: GS/DS/JS/CS Release_EX803 EXIT=0 ×4 (0 hata).',
' • DB: 14/14 tablo, DataNapGame\'de STT yok, gcoin iki DB\'de de yok.',
' • docs/32 opcode iddiası birebir: Protocol.cpp benzersiz case = 166.',
' • HEAD==origin/main; sunucu süreçleri kapalı (§5.1); pano JSON geçerli (CR=0).',
'',
'Kanıt: docs/33-DENETIM-PROJE-CAPINDA.md · BuildLog/denetim/',
].join('\n').split('«BS»').join(BS);
const ts = '2026-10-04 02:10';
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
sb.entries.unshift({ ts, kim: 'ajan', text: raw });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text: raw };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('panel guncellendi; entries=' + sb.entries.length);
