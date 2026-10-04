const fs = require('fs');
const ts = '2026-10-04 20:02';
function rep(s, a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); return s.split(a).join(b); }

// --- oku + dogrula (yazimlardan once) ---
const p38 = 'docs/38-TOPLU-DERLEME-SCRIPTI.md';
let d38 = fs.readFileSync(p38, 'latin1');
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 14:15]').length !== 2) throw new Error('CHANGELOG capa 14:15');
const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
if (sb.entries.length !== 15) throw new Error('sohbet entries=' + sb.entries.length);
if (sb.entries[0].ts !== '2026-10-04 14:15') throw new Error('sohbet uc: ' + sb.entries[0].ts);
if (!fs.existsSync('docs/39-PUSH-ONCESI-KANCA.md')) throw new Error('docs/39 yok');

// --- docs/38: --logdir satiri (Kullanim blogu) ---
d38 = rep(d38, '    bash BuildLog/denetim/build_all.sh --help\n',
  '    bash BuildLog/denetim/build_all.sh --help\n' +
  '    bash BuildLog/denetim/build_all.sh --logdir DIR  # ciktilari DIR altina yaz (pre-push kancasi, docs/39)\n',
  1, 'd38-logdir');
fs.writeFileSync(p38, d38, 'latin1');
console.log('docs/38 guncellendi');

// --- CHANGELOG ---
const entry = [
'## [26.10.04 20:02] pre-push kancasi: 603 kapisi + build_all dogrulamalari her push oncesi',
'',
'**Ne yapildi**',
'',
'`.githooks/pre-push` + `BuildLog/denetim/pre_push_check.sh`: her push oncesi',
'build_all.sh 9 hedef + tel duzeni + GS makrolari (12 kontrol) ciktilari izlenmez',
'dizine yazarak, 603 kapisi canli makro derlemesi (GECMELI) + negatif derleme',
'(C1189 ile KIRILMALI) kosar; kanca kosusu izlenen bir dosyayi degistirdiyse',
'(or. yeniden linklenen ikili) push bloklanir. Kurulum `install_hooks.sh`',
'(core.hooksPath=.githooks); atlatma `git push --no-verify`. `build_all.sh`',
'`--logdir DIR` kazandi (varsayilan davranis ayni).',
'',
'**Dogrulama**',
'',
'- Yesil: elle EXIT=0 (34 sn); gercek push EXIT=0, kanca PASS (27 sn), dummy remote',
'  `main -> main`. Kirmizi: tel duzeni bozukken elle/gercek push EXIT=1,',
'  `failed to push some refs`; dummy remote refsiz kaldi.',
'- Arg regresyonu: `--help` EXIT=0 + log md5 leri degismedi; `--bogus` EXIT=1;',
'  `--logdir` degersiz EXIT=1. Varsayilan kosu: 12/12 PASS, EXIT=0 (19:53).',
'',
'**Rapor:** `docs/39-PUSH-ONCESI-KANCA.md`',
'',
'',
].join('\n');
ch = ch.replace('## [26.10.04 14:15]', entry + '## [26.10.04 14:15]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// --- PANO ---
const text = [
'Push oncesi kanca: 603 kapisi + build_all dogrulamalari otomatik (docs/39). (istek: "603 derleme kapisini ve build_all.sh dogrulamalarini her push oncesi otomatik kosan bir kancaya bagla")',
'',
'YAPILANLAR',
' - `.githooks/pre-push` + `BuildLog/denetim/pre_push_check.sh`: push oncesi build_all.sh (9 hedef, tel duzeni, GS makrolari), 603 kapisi canli (gecmeli) + negatif (C1189 ile kirilmali), ardindan yeni-kirli kontrolu; izlenen dosya degisirse push bloklanir.',
' - `build_all.sh --logdir DIR`: kanca loglari izlenmez `pre_push/` dizinine yazar; kanonik loglar kirletilmez. Kurulum: `install_hooks.sh` (core.hooksPath=.githooks); atlatma: `git push --no-verify`.',
'',
'DOGRULAMA',
' - Yesil: elle EXIT=0 (34 sn); gercek push EXIT=0, kanca PASS (27 sn), dummy remote `main -> main`.',
' - Kirmizi: tel duzeni bozukken push EXIT=1 / `failed to push`, dummy remote refsiz kaldi. Arg regresyonu temiz; varsayilan tam kosu 12/12 PASS (19:53).',
'',
'KALAN',
' - H-018 runtime dogrulamasi (EX603 GS + istemci giris/cikis sahnesi) bekliyor.',
' - K1 dagitim yolu ve B3/gcoin kararlari acik.',
'',
'Kanit: docs/39-PUSH-ONCESI-KANCA.md - BuildLog/denetim/pre_push_green_push.txt / pre_push_red_push.txt',
].join('\n');
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
