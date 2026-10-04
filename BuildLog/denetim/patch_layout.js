const fs = require('fs');
const f = 'BuildLog/2e7/viewport_layout.js';
let s = fs.readFileSync(f, 'latin1');
if (s.includes('GAMESERVER_HAISLOTRING')) { console.log('zaten yamali'); process.exit(0); }
const rep = (a, b) => { if (s.split(a).length !== 2) throw new Error('cipa yok: ' + a.slice(0, 40)); s = s.replace(a, b); };
rep("// varsayilan: 803 1   (2e.6 derlemesi)",
    "// varsayilan: 603 0   (canli SPK 5.2); EX803 cifti icin: node viewport_layout.js 803 1");
rep("const gsu = parseInt(process.argv[2] || '803', 10);\nconst hais = parseInt(process.argv[3] || '1', 10);\nconst cfg = { GAMESERVER_UPDATE: gsu, HAISLOTRING: hais, GAMESERVER_TYPE: 0, GAMESERVER_LANGUAGE: 1, NEW_PROTOCOL_SYSTEM: 0, EQUIPMENT_LENGTH: CLIENT_CONST.EQUIPMENT_LENGTH };",
    "const gsu = parseInt(process.argv[2] || '603', 10);\nconst hais = parseInt(process.argv[3] || '0', 10);\n// sunucu cfg: kendi HAISLOTRING makrosu; istemci cfg: sunucunun HAISLOTRING'ini\n// temsil eden GAMESERVER_HAISLOTRING (istemcinin KENDI HAISLOTRING'i UI icindir).\nconst SERVER_CFG = { GAMESERVER_UPDATE: gsu, HAISLOTRING: hais, GAMESERVER_TYPE: 0, GAMESERVER_LANGUAGE: 1, NEW_PROTOCOL_SYSTEM: 0, EQUIPMENT_LENGTH: CLIENT_CONST.EQUIPMENT_LENGTH };\nconst CLIENT_CFG = { GAMESERVER_UPDATE: gsu, GAMESERVER_HAISLOTRING: hais, HAISLOTRING: 1, GAMESERVER_TYPE: 0, GAMESERVER_LANGUAGE: 1, NEW_PROTOCOL_SYSTEM: 0, EQUIPMENT_LENGTH: CLIENT_CONST.EQUIPMENT_LENGTH };");
rep('const G = gsStructs(cfg), C = clientStructs(cfg);',
    'const G = gsStructs(SERVER_CFG), C = clientStructs(CLIENT_CFG);');
rep("  index: 'KeyH+KeyL', x: 'PositionX', y: 'PositionY', CharSet: 'Class+Equipment',\n  name: 'ID', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path',\n  MuunItem: 'MuunItem', count: 's_BuffCount',",
    "  index: 'KeyH+KeyL', x: 'PositionX', y: 'PositionY', CharSet: 'Class+Equipment',\n  name: 'ID', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path',\n  MuunItem: 'MuunItem', count: 's_BuffCount',\n  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP',");
rep("  tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', CharSet: 'Class+Equipment',\n  MuunItem: 'MuunItem', count: 's_BuffCount',",
    "  tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', CharSet: 'Class+Equipment',\n  MuunItem: 'MuunItem', count: 's_BuffCount',\n  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP',");
rep("const MAP_MONSTER = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', count: 's_BuffCount' };",
    "const MAP_MONSTER = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', count: 's_BuffCount',\n  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP', CurHp: 'CurHp', Level: 'Level', Life: 'Life' };");
rep("const MAP_SUMMON = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', name: 'ID', count: 's_BuffCount' };",
    "const MAP_SUMMON = { index: 'KeyH+KeyL', type: 'TypeH+TypeL', x: 'PositionX', y: 'PositionY', tx: 'TargetX', ty: 'TargetY', DirAndPkLevel: 'Path', name: 'ID', count: 's_BuffCount',\n  attribute: 'Attribute', level: 'Level', MaxHP: 'MaxHP', CurHP: 'CurHP' };");
fs.writeFileSync(f, s, 'latin1');
fs.appendFileSync(f, "process.exit(verdict === 0 ? 0 : 1);\n", 'latin1');
console.log('viewport_layout.js guncellendi');
