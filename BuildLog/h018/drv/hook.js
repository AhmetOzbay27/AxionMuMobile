// hook.js - H-018: iki #ifdef H018_TDRV kancasini yerlestirir (latin1, CRLF korunur).
const fs = require('fs');
function hook(file, anchor, ins, label) {
  const s = fs.readFileSync(file, 'latin1');
  if (s.includes('H018_TDRV')) throw new Error(label + ': zaten kancali');
  const c = s.split(anchor).length - 1;
  if (c !== 1) throw new Error(label + ': capa=' + c);
  fs.writeFileSync(file, s.replace(anchor, anchor + ins), 'latin1');
  console.log('OK ' + file + ' (' + label + ')');
}
hook('Source/5.Main/source/WSclient.cpp',
  'BOOL TranslateProtocol( int HeadCode, BYTE *ReceiveBuffer, int Size, BOOL bEncrypted)\r\n{\r\n',
  '\t//H-018 runtime test kancasi (yalniz H018_TDRV derlemesinde)\r\n' +
  '#ifdef H018_TDRV\r\n' +
  '\textern void H018PktDump(int HeadCode, BYTE* buf, int size, BOOL enc);\r\n' +
  '\tH018PktDump(HeadCode, ReceiveBuffer, Size, bEncrypted);\r\n' +
  '#endif\r\n',
  'wsclient-pktdump');
hook('Source/5.Main/source/ZzzScene.cpp',
  'void Scene(HDC hDC)\r\n{\r\n\t//::Sleep(1);\r\n',
  '\t//H-018 runtime test kancasi (yalniz H018_TDRV derlemesinde)\r\n' +
  '#ifdef H018_TDRV\r\n' +
  '\textern void H018DrvTick(void);\r\n' +
  '\tH018DrvTick();\r\n' +
  '#endif\r\n',
  'zzzscene-tick');
console.log('KANCA TAMAM');
