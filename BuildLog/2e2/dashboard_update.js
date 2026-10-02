// dashboard_update.js - 2d.2 + 2e.1/2e.2 kayıtlarını Dashboard/data/*.json'a işler
// Kullanım: node BuildLog/2e2/dashboard_update.js
'use strict';
const fs = require('fs');
const path = require('path');
const D = path.join(__dirname, '..', '..', 'Dashboard', 'data');

const ozet = [
  '[19:10] 2e.1 + 2e.2 TAMAM — gömülü IP hizalandı, derleme çıktısı canlı SPK paketine kuruldu ve PAKETTE ÇALIŞTIRILDI.',
  '2e.1: canlı hedef ölçüldü: ConnectIP.bmd ilk 12 bayt = 45.87.120.29, port 44405 (AntiPort 55858; GetEngine.ini de 44405). Kaynakta 3 yerdeki gömülü 171.235.182.88 hizalandı (Winmain.cpp, GameConfigConstants.h, SceneCore.cpp); çalışma anında ConnectIP 0x20/0x22 IP+portları eziyor. Kaynakta 171.235.182.88 kalmadı.',
  '2e.2: BuildLog/2e2/deploy_spk_package.sh canlı paket düzenini kuruyor: Main.exe + SPK.ini + wzAudio.dll + ogg.dll + vorbisfile.dll + Data/SPK/{ConnectIP,ServerData}.bmd (2d.2 üretimi) + Data/SPK/Config + MUIG->SPK yol eşlemesi. Canlı kökte olmayan APICB.dll ve FreeImage.dll importları kaldırıldı (koşullu no-op + GDI+) — yeni Main.exe importlarında ikisi de YOK.',
  'Bulunan/çözülen dağıtım hataları: (1) GetGPUUse=1 donör kalıntısı nvapi.dll MessageBox ile GPU-su makinede kilitleniyordu -> 0; (2) wzAudio.dll bağımlılıkları vorbisfile.dll+ogg.dll pakette yoksa Windows yükleyicisi csrss sistem hatası veriyor (süreç penceresi yok, teşhisi zor) -> DLL kapanışı pakete eklendi; (3) Data/Local/Mix.bmd ölümcül kontrolü -> MixMgr SPK-first (Data/SPK/Config/Mix.bmd; canlı dosya 87.960 B = 14*4 + 134*656 formata birebir); (4) LoadEncDec SPK alan birleşiminden sonraya alındı; (5) GS port aralığı + FPS SPK verisinden.',
  'Çalıştırma kanıtı: istemci pakette kendi penceresini açtı (Axion Mu, ~0,8 s; BuildLog/2e2/shots/run9-with-player-win-0x340104.png) ve kendi günlüklerini yazdı (KEN.txt LoadBitmap satırları; STACK_ERROR çağrı zinciri: WinMain -> MainLoop -> Scene -> WebzenScene -> CreateTitleSceneUI -> CSprite::Create). 9 koşu sonucu BuildLog/2e2/results/run*.json.',
  'Ders (teşhis yöntemi): eksik DLL hatası csrss.exe penceresidir; ana iş parçacığı NtRaiseHardError içinde bekler ve süreç-penceresi numaralandıran koşucular onu görmez. Masaüstündeki TÜM pencereler taranarak bulundu. Betikler: ps_walk.ps1/ps_stackwalk.ps1/ps_code_scan.ps1.',
  'Derleme: Global Release|Win32 v143 (LNK4099 dışında uyarı yok) -> ClientFile/Main.exe 12.026.880 B, md5 71b008ad6d1d8e9e08c546859401dc92. Kalan: tam Data ağacı kopyası (C: %100 dolu; ~1,6 GB gerekli), SPK-first varlık çözümleme katmanı (2e.4; audit asset_audit.txt 12 tablo), CSprite::Create boş-bitmap dayanıklılığı (H-011), SPK_CRCFILE.ini SPK_MEXE yeniden üretimi. Raporlar docs/21 (2d.2) + docs/22 (2e.1/2e.2).',
  '[19:05] 2d.2 KABUL TESTİ TAMAM (doküman yazıldı): canlı Engine.exe bizim üretimimizle 4 koşuda çalıştırıldı; üretim koşusunda pencere Axion Mu 0,8 s + t~12 s 45.87.120.29:44405 SynSent (BuildLog/2d2/results/run2-ours-accept.json). Negatif kontrol (bozuk ServerData) yine pencere+TCP açtı -> pencere/TCP tek başına içerik kanıtı değil; birincil kanıt bayt karşılaştırması (docs/20). Canlı istemci dosyaları geri konuldu + md5 doğrulandı. Rapor docs/21.'
].join(' || ');

const ov = JSON.parse(fs.readFileSync(path.join(D, 'sonuc.json'), 'utf8'));
ov.updated = '2026-10-02 19:10:00';
ov.status = 'ok';
ov.text = ozet + ' || ' + (ov.text || '');
fs.writeFileSync(path.join(D, 'sonuc.json'), JSON.stringify(ov, null, 2), 'utf8');
console.log('sonuc.json guncellendi; text uzunlugu', ov.text.length);

const ch = JSON.parse(fs.readFileSync(path.join(D, 'sohbet.json'), 'utf8'));
const arr = ch.mesajlar || ch.messages || ch.sohbet || ch.entries;
if (!Array.isArray(arr)) { console.error('sohbet dizisi bulunamadi; anahtarlar:', Object.keys(ch)); process.exit(1); }
arr.push({
  ts: '2026-10-02 19:10',
  kim: 'asistan',
  text: '2e.1 + 2e.2 tamamlandı: gömülü IP 45.87.120.29:44405 ile hizalandı; APICB.dll/FreeImage.dll importları kaldırıldı; paket DLL kapanışı (wzAudio->ogg+vorbisfile) ve Data/SPK/Config kurulumu BuildLog/2e2/deploy_spk_package.sh ile yapıldı; istemci pakette kendi penceresini açtı (kanıt: BuildLog/2e2/shots + results/run*.json + KEN.txt/STACK_ERROR). Kalan: tam Data kopyası (disk %100), 2e.4 SPK-first varlık çözümleme katmanı, CSprite::Create boş-bitmap AV (H-011). Raporlar docs/21 + docs/22; CHANGELOG + docs/02-04 güncellendi.'
});
arr.push({
  ts: '2026-10-02 19:05',
  kim: 'asistan',
  text: '2d.2 kabul testi dokümanı yazıldı (docs/21): canlı Engine.exe bizim üretim ConnectIP+ServerData ile 4 koşu; üretim koşusunda 45.87.120.29:44405 SynSent kanıtı; negatif kontrol dersi (pencere+TCP içerik kanıtı değil, birincil kanıt bayt karşılaştırması).'
});
fs.writeFileSync(path.join(D, 'sohbet.json'), JSON.stringify(ch, null, 2), 'utf8');
console.log('sohbet.json guncellendi; mesaj sayisi', arr.length, 'anahtar:', Object.keys(ch).join(','));
