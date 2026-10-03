// add_log_include.js - gLog kullanan GameServer dosyalarina Log.h ekler
// (projedeki mevcut duzen: Connection.cpp "#include "Log.h"" kullanıyor)
const fs = require('fs');
const path = require('path');
const R = path.resolve(__dirname, '..', '..');
let fail = 0;

function patch(rel, oldStr, newStr, tag) {
  const p = path.join(R, rel);
  let s = fs.readFileSync(p, 'latin1');
  if (s.indexOf(newStr.replace(/\n/g, '\r\n')) >= 0 || s.indexOf(newStr) >= 0) {
    console.log(`ATLANDI [${tag}]`); return;
  }
  const o = /\r\n/.test(s) ? oldStr.replace(/\n/g, '\r\n') : oldStr;
  const n = /\r\n/.test(s) ? newStr.replace(/\n/g, '\r\n') : newStr;
  if (s.indexOf(o) < 0) { console.log(`HATA [${tag}]: anchor yok`); fail++; return; }
  fs.writeFileSync(p, Buffer.from(s.replace(o, n), 'latin1'));
  console.log(`UYGULANDI [${tag}]`);
}

patch('Source/4.GameServer/GameServer/PacketManager.cpp',
  '#include "stdafx.h"\n#include "PacketManager.h"',
  '#include "stdafx.h"\n#include "PacketManager.h"\n#include "Log.h"',
  'PacketManager.cpp Log.h');

patch('Source/4.GameServer/GameServer/GameServer.cpp',
  '#include "stdafx.h"\n#include "Resource.h"',
  '#include "stdafx.h"\n#include "Resource.h"\n#include "Log.h"',
  'GameServer.cpp Log.h');

console.log(fail === 0 ? '### INCLUDE EKLENDI' : `### ${fail} HATA`);
process.exit(fail === 0 ? 0 : 1);