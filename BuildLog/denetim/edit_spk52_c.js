const fs = require('fs');
function edit(path, fn) {
  const raw = fs.readFileSync(path, 'latin1');
  const NL = raw.includes('\r\n') ? '\r\n' : '\n';
  const out = fn(raw, NL);
  fs.writeFileSync(path, out, 'latin1');
  console.log('OK', path, 'NL=' + JSON.stringify(NL));
}
function must(cond, msg) { if (!cond) throw new Error(msg); }

// ---- WSclient.h ----
edit('Source/5.Main/source/WSclient.h', (s, NL) => {
  const L = a => a.join(NL);
  const anchor = '} PRECEIVE_EQUIPMENT, * LPPRECEIVE_EQUIPMENT;' + NL;
  must(s.split(anchor).length === 2, 'PRECEIVE_EQUIPMENT cipasi');
  const macroBlok = L([
    '',
    '// 04.10.2026 (docs/34): viewport tel duzeni makrolari.',
    '// Sunucudaki Viewport.h kosullari aynen istemciye uygulanir; sunucunun derleme',
    '// degerleri asagidaki iki makro ile verilir. Varsayilan = canli SPK 5.2:',
    '// GAMESERVER_UPDATE=603 + sunucu HAISLOTRING=0 (kanit: BuildLog/2e7 PDB',
    '// 36/38/20/20). EX803 cifti icin derlemede /DGAMESERVER_UPDATE=803 verin.',
    '// Istemcinin KENDI HAISLOTRING makrosu (UI ozellikleri) bundan bagimsizdir.',
    '#ifndef GAMESERVER_HAISLOTRING',
    '#define GAMESERVER_HAISLOTRING 0',
    '#endif',
    '#if ((GAMESERVER_HAISLOTRING) && (GAMESERVER_UPDATE<701)) || (GAMESERVER_UPDATE>=803)',
    '#define VIEWPORT_HAS_MUUNIT 1',
    '#else',
    '#define VIEWPORT_HAS_MUUNIT 0',
    '#endif',
    '',
  ]);
  s = s.replace(anchor, anchor + macroBlok);

  const oldC = L([
    '\t// 04.10.2026 (docs/34): alanlar sunucudaki PMSG_VIEWPORT_* yapisiyla AYNI',
    '\t// kosullara baglandi. Varsayilan (GAMESERVER_UPDATE=603 = SPK 5.2 canli):',
    "\t// count alani Path 'ten hemen sonra. EX803 test cifti icin derlemede",
    '\t// /DGAMESERVER_UPDATE=803 verilir. Bkz. Viewport.h + viewport_layout.js.',
    '#if(GAMESERVER_UPDATE >= 701)',
  ]);
  const newC = L([
    '\t// 04.10.2026 (docs/34): kosullar sunucudaki Viewport.h ile BIREBIR aynidir.',
    '\t// Varsayilan = canli SPK 5.2: 603 + sunucu HAISLOTRING 0 -> 36/38/20/20',
    '\t// (kanit: BuildLog/2e7). EX803 cifti icin /DGAMESERVER_UPDATE=803 verin.',
    '#if(GAMESERVER_HAISLOTRING) && (GAMESERVER_UPDATE < 701)',
    '\tBYTE         MuunItem[2];',
    '#endif',
    '#if(GAMESERVER_UPDATE >= 701)',
  ]);
  must(s.split(oldC).length === 3, 'PLAYER/TRANSFORM blok cipasi (2 bekleniyor)');
  s = s.split(oldC).join(newC);
  return s;
});

// ---- WSclient.cpp ----
edit('Source/5.Main/source/WSclient.cpp', (s, NL) => {
  const L = a => a.join(NL);
  const oldP = L(['\t\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);']);
  must(s.split(oldP).length === 2, 'PLAYER Level tek olmali');
  s = s.replace(oldP, L([
    '#if(GAMESERVER_UPDATE >= 701)',
    '\t\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);',
    '#endif',
  ]));
  const oldS = L(['\t\tc->PK   = Data2->Path&0xf;', '\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);']);
  must(s.split(oldS).length === 2, 'SUMMON Level tek olmali');
  s = s.replace(oldS, L([
    '\t\tc->PK   = Data2->Path&0xf;',
    '#if(GAMESERVER_UPDATE >= 701)',
    '\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);',
    '#endif',
  ]));
  const m1 = L(['#if(HAISLOTRING)', '\t\t\tif (c)']);
  must(s.split(m1).length === 2, 'MuunItem(c) tek olmali');
  s = s.replace(m1, L(['#if(HAISLOTRING) && (VIEWPORT_HAS_MUUNIT)', '\t\t\tif (c)']));
  const m2 = L(['#if(HAISLOTRING)', '\t\t\tif (pCha)']);
  must(s.split(m2).length === 2, 'MuunItem(pCha) tek olmali');
  s = s.replace(m2, L(['#if(HAISLOTRING) && (VIEWPORT_HAS_MUUNIT)', '\t\t\tif (pCha)']));
  return s;
});
