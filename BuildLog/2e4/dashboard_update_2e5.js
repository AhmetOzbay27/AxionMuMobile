// dashboard_update_2e5.js — 2e.5 turu: sohbet.json'a en üste kayıt, sonuc.json'a özet
'use strict';
const fs = require('fs');
const path = require('path');

const root = path.join(__dirname, '..', '..');
const sohbetPath = path.join(root, 'Dashboard', 'data', 'sohbet.json');
const sonucPath = path.join(root, 'Dashboard', 'data', 'sonuc.json');
const STAMP = '2026-10-03 01:20';

const sohbetEntry = {
  ts: STAMP,
  kim: 'ajan',
  text: [
    '2e.5 TAMAM — B-08 (SPK ToolTipText) + FAZ 3 E2E BAĞLANTI KANITI (docs/25).',
    '',
    'B-08: canlı pakette itemtooltiptext_*.bmd yok; tablo düz metin Data\\SPK\\Config\\ToolTipText.txt (19 kayıt) içinde.',
    'Yeni yükleyici load_item_tooltip_text_spk() (ZzzInfomation.cpp) tabloyu tooltip_text_data[] üyesine aktarıyor',
    '(markup <f c=...></f> temizliği, type=-1), ZzzOpenData itemtooltiptext çözümlenemezse çözümleyici üzerinden çağırıyor.',
    'Runtime kanıt (Release): KEN.txt → "[SPK] ToolTipText: 19 kayit". Derleme md5 c49383bf62c412bf53adf1ec62c67f77 (12.031.488 B).',
    '',
    'FAZ 3.1: CS/DS/JS/GS + MuOnlineS6 DB + MuOnlineS6ODBC DSN; GS ServerVersion=1.03.34/ServerSerial=!571Axion@Mobile;',
    'CS ServerList → 45.87.120.29:55901; e2etest hesabı; istemci ConnectIP = 45.87.120.29:63000 (make_connectip.js).',
    'FAZ 3.2 (kısmi): istemci ConnectServer\'a TCP ESTABLISHED (t≈4,4 s; 100 ms örnekleme) + SUNUCU SEÇİM EKRANI görüntülendi;',
    'diyalog/çökme/kalıntı süreç yok (run21/22/26).',
    '',
    'YENİ BULGULAR: (H-016) istemci 127.0.0.1 hedefini kasten reddediyor (WSctlc.cpp:230 → hiç SYN yok + MESSAGE_SERVER_LOST)',
    '→ E2E hedefi makinenin gerçek IPv4\'ü olmalı; (H-017) ajan oturumu disconnected (GetForegroundWindow()==0) → UI tıklaması',
    'otomatikleştirilemiyor; sunucu seç → GS/login adımı etkileşimli masaüstü oturumunda elle koşulacak (run22-25 denemeleri, docs/25 §4.3).',
    '',
    'KAPANIŞ: H-005/H-006/H-007 kapatıldı (docs/04); B-08 → docs/03 ✅; Faz 3 durumu docs/02; test yığını KAPATILDI (§5.1);',
    'geçici kopyalar silindi (2e2\\deploy, 2e3\\deploy, 5Main → ~513 MB kazanç).',
    '',
    'GITHUB: remote tanımlı değil, gh CLI ve kimlik bilgisi yok → repo URL + erişim bilgisi gerekli (patron onayı bekleniyor).'
  ].join('\n')
};

const sohbet = JSON.parse(fs.readFileSync(sohbetPath, 'utf8').replace(/^\uFEFF/, ''));
sohbet.updated = STAMP;
sohbet.entries = [sohbetEntry].concat(sohbet.entries || []);
fs.writeFileSync(sohbetPath, '\uFEFF' + JSON.stringify(sohbet, null, 2) + '\n', 'utf8');
console.log('sohbet.json: ' + sohbet.entries.length + ' kayit (en üstte 2e.5)');

const sonuc = JSON.parse(fs.readFileSync(sonucPath, 'utf8').replace(/^\uFEFF/, ''));
sonuc.updated = STAMP;
sonuc.status = 'ok';
sonuc.text = [
  '[01:20] 2e.5 TAMAM — B-08 (SPK ToolTipText yükleyicisi) + FAZ 3 E2E BAĞLANTI KANITI.',
  'B-08: canlı Data\\SPK\\Config\\ToolTipText.txt (19 kayıt, düz metin) yeni load_item_tooltip_text_spk() ile istemci tooltip',
  'tablosuna aktarıldı (markup temizliği, type=-1; ZzzOpenData çağrı noktası + SPK çözümleyici). Runtime kanıtı KEN.txt:',
  '" [SPK] ToolTipText: 19 kayit"; derleme md5 c49383bf62c412bf53adf1ec62c67f77 (12.031.488 B).',
  '',
  'FAZ 3.1/3.2: test yığını (CS 63000 / DS 63002 / JS 63003 / GS 55901, bizim derlemeler + MuOnlineS6 DB + MuOnlineS6ODBC DSN,',
  'e2etest hesabı) ayakta iken istemci paketi (ConnectIP = 45.87.120.29:63000) koşuldu: ConnectServer bağlantısı TCP ESTABLISHED',
  '(t≈4,4 s) ve SUNUCU SEÇİM EKRANI render edildi; diyalog/çökme yok. Negatif kanıt: hedef 127.0.0.1 iken istemci hiç bağlanmıyor',
  '— WSctlc.cpp:230 loopback reddi (H-016); E2E hedefi makinenin gerçek IPv4\'ü olmalı.',
  '',
  'SINIR (H-017): ajan oturumu disconnected (GetForegroundWindow()==0) → istemci g_bWndActive kuramıyor, fare girdisi işlenmiyor',
  '(post/real mouse/ALT+foreground/TOPMOST denemeleri); sunucu seç → GS/login → karakter akışı etkileşimli oturumda elle koşulacak.',
  'Test yığını kapatıldı (§5.1), geçici kopyalar silindi (~513 MB). Rapor: docs/25. GitHub: remote/kimlik bilgisi yok → patron girdisi gerekli.'
].join('\n');
fs.writeFileSync(sonucPath, '\uFEFF' + JSON.stringify(sonuc, null, 2) + '\n', 'utf8');
console.log('sonuc.json guncellendi (' + sonuc.status + ')');
