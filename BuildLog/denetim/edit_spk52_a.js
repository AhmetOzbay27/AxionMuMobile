const fs = require('fs');
function chk(cond, msg) { if (!cond) throw new Error(msg); }
function spanReplace(s, start, semiWord, block) {
  const semi = s.indexOf(semiWord, start);
  chk(semi > 0, 'bitis yok: ' + semiWord);
  let end = s.indexOf('\n', semi);
  if (end < 0) end = s.length;
  return s.slice(0, start) + block + s.slice(end);
}
const T = '\t';

// ---- 1) WSclient.h: 4 viewport yapisi kosullu hale getirilir ----
{
  const f = 'Source/5.Main/source/WSclient.h';
  let s = fs.readFileSync(f, 'utf8');
  const marker = T + '// H-018 (2e.7): bu alanlar sunucudaki PMSG_VIEWPORT_* yapisyla ayni';
  const idxs = [];
  let i = -1;
  while ((i = s.indexOf(marker, i + 1)) >= 0) idxs.push(i);
  chk(idxs.length === 4, 'beklenen 4 yapi, bulunan ' + idxs.length);
  const blocks = {
    CHARACTER: [
      T + '// 04.10.2026 (docs/34): alanlar sunucudaki PMSG_VIEWPORT_* yapisiyla AYNI',
      T + '// kosullara baglandi. Varsayilan (GAMESERVER_UPDATE=603 = SPK 5.2 canli):',
      T + '// count alani Path \'ten hemen sonra. EX803 test cifti icin derlemede',
      T + '// /DGAMESERVER_UPDATE=803 verilir. Bkz. Viewport.h + viewport_layout.js.',
      '#if(GAMESERVER_UPDATE >= 701)',
      T + 'BYTE         Attribute;',
      '#if(GAMESERVER_UPDATE >= 803)',
      T + 'BYTE         MuunItem[2];',
      '#endif',
      T + 'BYTE         Level[2];',
      T + 'BYTE         MaxHP[4];',
      T + 'BYTE         CurHP[4];',
      '#endif',
      T + 'BYTE         s_BuffCount;',
    ].join('\n'),
    TRANSFORM: null,
    SUMMON: [
      T + '// 04.10.2026 (docs/34): sunucu SUMMON yapisiyla ayni kosullar.',
      '#if(GAMESERVER_UPDATE >= 701)',
      T + 'BYTE         Attribute;',
      T + 'BYTE         Level[2];',
      T + 'BYTE         MaxHP[4];',
      T + 'BYTE         CurHP[4];',
      '#endif',
      T + 'BYTE         s_BuffCount;',
    ].join('\n'),
    MONSTER: [
      T + '// 04.10.2026 (docs/34): sunucu MONSTER yapisiyla ayni kosullar.',
      T + '// <701 (SPK 5.2 canli): CurHp/Level/Life (canli PDB: +9/+10/+12, sizeof=20).',
      '#if(GAMESERVER_UPDATE >= 701)',
      T + 'BYTE         Attribute;',
      T + 'BYTE         Level[2];',
      T + 'BYTE         MaxHP[4];',
      T + 'BYTE         CurHP[4];',
      '#else',
      T + 'BYTE         CurHp;',
      T + 'BYTE         Level[2];',
      T + 'DWORD        Life;',
      '#endif',
      T + 'BYTE         s_BuffCount;',
    ].join('\n'),
  };
  blocks.TRANSFORM = blocks.CHARACTER;
  for (let k = idxs.length - 1; k >= 0; k--) {
    const start = idxs[k];
    const head = s.slice(start, start + 700);
    const m = head.match(/}\s*PCREATE_(CHARACTER|TRANSFORM|SUMMON|MONSTER)/);
    chk(m, 'yapi adi bulunamadi');
    s = spanReplace(s, start, 's_BuffCount;', blocks[m[1]]);
  }
  fs.writeFileSync(f, s, 'utf8');
  console.log('WSclient.h guncellendi');
}

// ---- 2) Defined_Global.h: GAMESERVER_UPDATE varsayilani 603 (canli) ----
{
  const f = 'Source/5.Main/source/Defined_Global.h';
  let s = fs.readFileSync(f, 'utf8');
  const re = /^#define HAISLOTRING.*$/m;
  const m = s.match(re);
  chk(m, 'HAISLOTRING satiri yok');
  const add = m[0] + '\n' + [
    '// 04.10.2026 (docs/34): SPK 5.2 canli sunucusu GAMESERVER_UPDATE=603 +',
    '// HAISLOTRING=0 ile derlenmis (canli GameServer.pdb viewport: 36/38/20/20).',
    '// Istatemci varsayilani canli ile aynidir; EX803 test cifti icin derleme',
    '// satirina /DGAMESERVER_UPDATE=803 ekleyin.',
    '#ifndef GAMESERVER_UPDATE',
    '#define GAMESERVER_UPDATE\t\t\t\t603',
    '#endif',
  ].join('\n');
  s = s.replace(re, add);
  fs.writeFileSync(f, s, 'utf8');
  console.log('Defined_Global.h guncellendi');
}

// ---- 3) GS stdafx.h: HAISLOTRING disaridan verilebilir ----
{
  const f = 'Source/4.GameServer/GameServer/stdafx.h';
  let s = fs.readFileSync(f, 'utf8');
  const re = /^#define HAISLOTRING.*$/m;
  const m = s.match(re);
  chk(m, 'GS HAISLOTRING yok');
  s = s.replace(re, [
    '// 04.10.2026 (docs/34): disaridan verilebilir; SPK 5.2 canli derlemesi',
    '// HAISLOTRING=0 (kanit: canli PDB viewport 36/38/20/20 -> BuildLog/2e7).',
    '#ifndef HAISLOTRING',
    '#define HAISLOTRING\t\t\t\t\t1',
    '#endif',
  ].join('\n'));
  fs.writeFileSync(f, s, 'utf8');
  console.log('GS stdafx.h guncellendi');
}

// ---- 4) GameServer.vcxproj: Release_EX603 = SPK 5.2 canli esdegeri ----
{
  const f = 'Source/4.GameServer/GameServer/GameServer.vcxproj';
  let s = fs.readFileSync(f, 'utf8');
  const a = '<PreprocessorDefinitions>WIN32;NDEBUG;_WINDOWS;GAMESERVER_TYPE=0;GAMESERVER_UPDATE=603;GAMESERVER_LANGUAGE=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>';
  const n = s.split(a).length - 1;
  chk(n === 1, 'EX603 tanim blogu beklenen 1, bulunan ' + n);
  s = s.replace(a, a.replace('GAMESERVER_UPDATE=603;', 'GAMESERVER_UPDATE=603;HAISLOTRING=0;'));
  fs.writeFileSync(f, s, 'utf8');
  console.log('GameServer.vcxproj guncellendi');
}

// ---- 5) Viewport.cpp: derleme zamani boyut kilitleri ----
{
  const f = 'Source/4.GameServer/GameServer/Viewport.cpp';
  let s = fs.readFileSync(f, 'utf8');
  const a = '#include "BattleSurvivor.h"\n';
  chk(s.includes(a), 'Viewport.cpp include blogu bulunamadi');
  const add = a + [
    '// 04.10.2026 (docs/34): SPK 5.2 canli bayt sayilari derleme zamaninda kilitli.',
    '// Canli PDB kaniti: BuildLog/2e7/live_pdb_viewport_layout.txt (36/38/20/20).',
    '#if (GAMESERVER_UPDATE == 603) && !(HAISLOTRING)',
    'static_assert(sizeof(PMSG_VIEWPORT_PLAYER) == 36, "SPK 5.2 PLAYER 36B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_CHANGE) == 38, "SPK 5.2 CHANGE 38B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_MONSTER) == 20, "SPK 5.2 MONSTER 20B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_SUMMON) == 20, "SPK 5.2 SUMMON 20B olmali");',
    '#endif',
    '#if (GAMESERVER_UPDATE >= 803)',
    'static_assert(sizeof(PMSG_VIEWPORT_PLAYER) == 49, "EX803 PLAYER 49B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_CHANGE) == 51, "EX803 CHANGE 51B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_MONSTER) == 21, "EX803 MONSTER 21B olmali");',
    'static_assert(sizeof(PMSG_VIEWPORT_SUMMON) == 31, "EX803 SUMMON 31B olmali");',
    '#endif',
    '',
  ].join('\n');
  s = s.replace(a, add);
  fs.writeFileSync(f, s, 'utf8');
  console.log('Viewport.cpp guncellendi');
}
console.log('TAMAM');
