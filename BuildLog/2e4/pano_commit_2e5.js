// 2e.5 commit kapanis pano kaydi (sohbet.json)
const fs = require('fs');
const P = 'Dashboard/data/sohbet.json';
const raw = fs.readFileSync(P, 'utf8');
const d = JSON.parse(raw);

const text = [
  '2e.5 COMMIT KAPANDI — kod + doküman + kanıt tek commit\'te (010c41da8), hash takibi b7a9ae062.',
  '',
  'Staged: Source/5.Main/source/{ZzzInfomation.cpp,ZzzInfomation.h,ZzzOpenData.cpp}, ClientFile/Main.exe,',
  'docs/25-2E5-B08-TEST-E2E.md (yeni) + docs/02 + docs/03 + docs/04 + docs/CHANGELOG, pano (sohbet+sonuc),',
  'BuildLog/2e4 kanıtları (build_2e5*.log, results/run18-26*.json, shots/*.png, evidence/KEN_run26.txt) ve',
  'E2E araç betikleri (run19_watch.ps1, run22_e2e.ps1, make_connectip.js, trap.js, serve.js, shot_*.ps1,',
  'apply_open_data_patch.js, dashboard_update_2e5.js, connectip_*.bmd). 111 dosya / +2924 satır.',
  '',
  'Diğer ajanlara ait dokunulmadı: docs/00,05,07,08,09,13 · Source/4.GameServer · Source/6.GetMainInfo ·',
  'MuServer/* · GetMain/GetMainInfo.exe · onların untracked dosyaları (docs/14-20, BuildLog/2c1, 2d1, envanter).',
  'Encoding kontrolü: 9 dosyanın diff\'inde 0 U+FFFD (Korece EUC-KR dosyalarında bozulma yok).',
  '',
  'GITHUB PENGEL: .git/config içinde remote YOK, gh CLI yok, PAT/SSH anahtarı yok → push için repo URL +',
  'kimlik doğrulama yöntemi patron onayı bekleniyor. Kullanıcı talebi: "githuba yükle".'
].join('\n');

d.entries.unshift({ ts: '2026-10-03 01:55', kim: 'ajan', text });
d.updated = '2026-10-03 01:55';

fs.writeFileSync(P, JSON.stringify(d, null, 2), 'utf8');
console.log('entries:', d.entries.length, '| top:', d.entries[0].ts);