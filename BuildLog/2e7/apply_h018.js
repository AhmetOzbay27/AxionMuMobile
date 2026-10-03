// apply_h018.js - H-018 bayt hizalama duzeltmesi (byte-exact, latin1, CRLF-duyarli, idempotent)
const fs = require('fs');
const path = require('path');
const REPO = path.resolve(__dirname, '..', '..');

let failures = 0;

function readBuf(p) { return fs.readFileSync(p, 'latin1'); }
function writeBuf(p, s) { fs.writeFileSync(p, Buffer.from(s, 'latin1')); }

// CRLF/LF farkindan bagimsiz, idempotent degistirme
function sub(text, oldStr, newStr, tag) {
  if (text.indexOf(newStr) >= 0) { console.log(`ATLANDI [${tag}] (zaten uygulanmis)`); return text; }
  const variants = /\r\n/.test(text)
    ? [[oldStr.replace(/\n/g, '\r\n'), newStr.replace(/\n/g, '\r\n')],
       [oldStr, newStr]]
    : [[oldStr, newStr], [oldStr.replace(/\n/g, '\r\n'), newStr.replace(/\n/g, '\r\n')]];
  for (const [o, n] of variants) {
    const c = text.split(o).length - 1;
    if (c === 1) { console.log(`UYGULANDI [${tag}]`); return text.split(o).join(n); }
    if (c > 1) { console.log(`HATA [${tag}]: ${c} kez eslesti (1 bekleniyordu)`); failures++; return text; }
  }
  console.log(`HATA [${tag}]: anchor bulunamadi`);
  failures++;
  return text;
}

const NOTE = `\t// H-018 (2e.7): bu alanlar sunucudaki PMSG_VIEWPORT_* yapisyla ayni
\t// kabloda sirali olmalidir; bkz. Source/4.GameServer/GameServer/Viewport.h
\t// (GAMESERVER_UPDATE=803) ve BuildLog/2e7/viewport_layout.js.
`;

// ---------------------------------------------------------------- WSclient.h
{
  const p = path.join(REPO, 'Source/5.Main/source/WSclient.h');
  let s = readBuf(p);
  s = sub(s,
`\tBYTE         Path;
#if (HAISLOTRING)
\tBYTE         MuunItem[2];
#endif
\tBYTE         s_BuffCount;`,
`\tBYTE         Path;
${NOTE}\tBYTE         Attribute;
#if (HAISLOTRING)
\tBYTE         MuunItem[2];
#endif
\tBYTE         Level[2];
\tBYTE         MaxHP[4];
\tBYTE         CurHP[4];
\tBYTE         s_BuffCount;`, 'PCREATE_CHARACTER');

  s = sub(s,
`\tBYTE         Class;
\tBYTE         Equipment[EQUIPMENT_LENGTH];
#if (HAISLOTRING)
\tBYTE         MuunItem[2];
#endif
\tBYTE         s_BuffCount;`,
`\tBYTE         Class;
\tBYTE         Equipment[EQUIPMENT_LENGTH];
${NOTE}\tBYTE         Attribute;
#if (HAISLOTRING)
\tBYTE         MuunItem[2];
#endif
\tBYTE         Level[2];
\tBYTE         MaxHP[4];
\tBYTE         CurHP[4];
\tBYTE         s_BuffCount;`, 'PCREATE_TRANSFORM');

  s = sub(s,
`\tBYTE         Path;
\tBYTE         ID[MAX_ID_SIZE];
\tBYTE         s_BuffCount;`,
`\tBYTE         Path;
\tBYTE         ID[MAX_ID_SIZE];
${NOTE}\tBYTE         Attribute;
\tBYTE         Level[2];
\tBYTE         MaxHP[4];
\tBYTE         CurHP[4];
\tBYTE         s_BuffCount;`, 'PCREATE_SUMMON');

  s = sub(s,
`\tBYTE         Path;
\tBYTE         s_BuffCount;`,
`\tBYTE         Path;
${NOTE}\tBYTE         Attribute;
\tBYTE         Level[2];
\tBYTE         MaxHP[4];
\tBYTE         CurHP[4];
\tBYTE         s_BuffCount;`, 'PCREATE_MONSTER');
  writeBuf(p, s);
}

// --------------------------------------------------------------- WSclient.cpp
{
  const p = path.join(REPO, 'Source/5.Main/source/WSclient.cpp');
  const eol = /\r\n/.test(readBuf(p)) ? '\r\n' : '\n';
  let lines = readBuf(p).split(/\r?\n/);
  // asagidan yukariya isle: onceki eklemeler satir numaralarini kaydirmasin
  const jobs = [
    { find: 'c->TargetY = Data2->TargetY;', limit: [2280, 2460], tag: 'ReceiveCreatePlayerViewport', text: '\t\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);' },
    { find: 'if(c == NULL) break;', limit: [2759, 2890], tag: 'ReceiveCreateMonsterViewport', text: '\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);' },
    { find: 'c->PK   = Data2->Path&0xf;', limit: [2888, 2970], tag: 'ReceiveCreateSummonViewport', text: '\t\tc->Level = MAKE_NUMBERW(Data2->Level[0],Data2->Level[1]);' },
  ];
  for (const j of jobs) {
    const inRange = (l) => l.indexOf(j.text) >= 0 &&
      lines.slice(Math.max(0, j.limit[0] - 3), j.limit[1]).indexOf(l) >= 0;
    if (lines.slice(j.limit[0] - 1, j.limit[1]).some(inRange)) { console.log(`ATLANDI [${j.tag}]`); continue; }
    let hit = -1;
    for (let i = j.limit[0] - 1; i < j.limit[1] && i < lines.length; i++) {
      if (lines[i].indexOf(j.find) >= 0) { hit = i; break; }
    }
    if (hit < 0) { console.log(`HATA [${j.tag}]: "${j.find}" bulunamadi`); failures++; continue; }
    lines.splice(hit + 1, 0, j.text);
    console.log(`UYGULANDI [${j.tag}] satir ${hit + 1}`);
  }
  writeBuf(p, lines.join(eol));
}

// --------------------------------------------------- Viewport.h (canli MONSTER)
{
  const p = path.join(REPO, 'Source/4.GameServer/GameServer/Viewport.h');
  let s = readBuf(p);
  s = sub(s,
`\tBYTE DirAndPkLevel;
\t#if(GAMESERVER_UPDATE>=701)
\tBYTE attribute;
\tBYTE level[2];
\tBYTE MaxHP[4];
\tBYTE CurHP[4];
\t#endif
\tBYTE count;
};

struct PMSG_VIEWPORT_SUMMON`,
`\tBYTE DirAndPkLevel;
\t#if(GAMESERVER_UPDATE>=701)
\tBYTE attribute;
\tBYTE level[2];
\tBYTE MaxHP[4];
\tBYTE CurHP[4];
\t#else
\t// Canli SPK sunucusu bu blokta 7 baytlik CurHp/Level/Life gonderiyor.
\t// Kanit: canli GameServer.pdb (derleme zaman damgasi 6AAE6177 = 19.09.2026,
\t// GAMESERVER_UPDATE=603) -> CurHp@+9, Level[2]@+10, Life@+12, count@+16,
\t// sizeof=20. Kayit: BuildLog/2e7/live_pdb_viewport_layout.txt
\tBYTE CurHp;
\tBYTE Level[2];
\tDWORD Life;
\t#endif
\tBYTE count;
};

struct PMSG_VIEWPORT_SUMMON`, 'PMSG_VIEWPORT_MONSTER');
  writeBuf(p, s);
}

// ------------------------------------------------------- Viewport.cpp (yazici)
{
  const p = path.join(REPO, 'Source/4.GameServer/GameServer/Viewport.cpp');
  let s = readBuf(p);
  s = sub(s,
`\t\tinfo.CurHP[3] = SET_NUMBERLB(SET_NUMBERLW((lpTarget->Life)));

\t\t#endif

\t\t#if(GAMESERVER_TYPE==1)

\t\tif(lpTarget->Class == 216)`,
`\t\tinfo.CurHP[3] = SET_NUMBERLB(SET_NUMBERLW((lpTarget->Life)));

\t\t#endif

\t\t#if(GAMESERVER_UPDATE<701)
\t\t// Canli ile ayni alan karsiligi (bkz. Viewport.h ve docs/27).
\t\tint iMaxLife = lpTarget->MaxLife + lpTarget->AddLife;
\t\tinfo.CurHp = (BYTE)((iMaxLife > 0)?((lpTarget->Life * 100) / iMaxLife):0);
\t\tinfo.Level[0] = SET_NUMBERHB(lpTarget->Level);
\t\tinfo.Level[1] = SET_NUMBERLB(lpTarget->Level);
\t\tinfo.Life = (DWORD)lpTarget->Life;
\t\t#endif

\t\t#if(GAMESERVER_TYPE==1)

\t\tif(lpTarget->Class == 216)`, 'GCViewportMonsterSend');
  writeBuf(p, s);
}

console.log(failures === 0 ? '### YAMALAMA TAMAM' : `### ${failures} HATA`);
process.exit(failures === 0 ? 0 : 1);