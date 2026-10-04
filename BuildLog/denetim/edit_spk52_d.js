const fs = require('fs');
function edit(path, fn) {
  const raw = fs.readFileSync(path, 'latin1');
  const NL = raw.includes('\r\n') ? '\r\n' : '\n';
  const out = fn(raw, NL);
  fs.writeFileSync(path, out, 'latin1');
  console.log('OK', path);
}
function must(c, m) { if (!c) throw new Error(m); }

// 1) UuidCreateSequential donus degeri denetimi (4 sunucu Protect.cpp)
for (const p of ['Source/1.ConnectServer/ConnectServer/Protect.cpp',
                 'Source/2.DataServer/DataServer/Protect.cpp',
                 'Source/3.JoinServer/JoinServer/Protect.cpp',
                 'Source/4.GameServer/GameServer/Protect.cpp']) {
  edit(p, (s, NL) => {
    const L = a => a.join(NL);
    const old = L(['\tUUID uuid;', '', '\tUuidCreateSequential(&uuid);']);
    must(s.split(old).length === 2, p + ' uuid cipasi');
    return s.replace(old, L([
      '\tUUID uuid;', '',
      '\tRPC_STATUS uuidStatus = UuidCreateSequential(&uuid);',
      '',
      '\t// 04.10.2026 (docs/34): donus degeri denetlenir; basarisizsa (yerel-only',
      '\t// UUID kabul edilir) rastgele UuidCreate kullanilir ki donanim kimligi',
      '\t// uninitialized bellekten uretilmesin (docs/28 bulgusu).',
      '\tif(uuidStatus != ERROR_SUCCESS && uuidStatus != RPC_S_UUID_LOCAL_ONLY)',
      '\t{',
      '\t\tUuidCreate(&uuid);',
      '\t}',
    ]));
  });
}

// 2) B3: ObjectManager respawn yoluna canli 0x538687 karsiligi monster_add(class,true)
edit('Source/4.GameServer/GameServer/ObjectManager.cpp', (s, NL) => {
  const L = a => a.join(NL);
  must(s.split('#include "CastleSiegeSync.h"' + NL).length === 2, 'CastleSiegeSync include cipasi');
  s = s.replace('#include "CastleSiegeSync.h"' + NL,
    '#include "CastleSiegeSync.h"' + NL + '#include "CB_ActiveInvasions.h"\t// 2c.1-B3: respawn sayaci (canli 0x538687)' + NL);
  const old = L(['\t\t\tlpObj->DieRegen = 0;', '\t\t\tlpObj->State = OBJECT_CREATE;', '', '\t\t\tgObjViewportListProtocolCreate(lpObj);']);
  must(s.split(old).length === 2, 'respawn blok cipasi');
  return s.replace(old, L([
    '\t\t\tlpObj->DieRegen = 0;',
    '\t\t\tlpObj->State = OBJECT_CREATE;',
    '',
    '\t\t\tgCB_ActiveInvasions.monster_add(lpObj->Class,true);\t// 2c.1-B3: canli respawn yolu (0x538687, push 1 + viewport create)',
    '',
    '\t\t\tgObjViewportListProtocolCreate(lpObj);',
  ]));
});

// 3) AutoNap TypeDB=1 uyarisi (gcoin kolonu semada yok - docs/29 B6)
edit('Source/2.DataServer/DataServer/CB_AutoNapGame.cpp', (s, NL) => {
  const L = a => a.join(NL);
  const old = '\tthis->TypeDB = GetPrivateProfileInt("AutoNapBank", "TypeDB", 0, PathConfig);';
  must(s.split(old).length === 2, 'TypeDB cipasi');
  return s.replace(old, L([
    '\tthis->TypeDB = GetPrivateProfileInt("AutoNapBank", "TypeDB", 0, PathConfig);',
    '\tif(this->TypeDB == 1)',
    '\t{',
    '\t\t// 04.10.2026 (docs/34): TypeDB=1 yolu MEMB_INFO.gcoin kolonuna yazar; bu kolon',
    '\t\t// MuOnline/MuOnlineS6 semalarinda YOK (docs/29 B6) -> guncelleme hata verir.',
    '\t\tLogAdd(LOG_RED,"[AutoNap] TypeDB=1: MEMB_INFO.gcoin kolonu gerekli (docs/29 B6).");',
    '\t}',
  ]));
});
