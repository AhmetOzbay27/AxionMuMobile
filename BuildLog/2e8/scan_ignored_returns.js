// scan_ignored_returns.js - Sunucu kaynaklarinda dönüş değeri yok sayılan
// "okuma" ve "mesaj" API çağrılarını bulur.
// Kural: satır bir API çağrısıyla başlıyor, sonucu '=' ile yakalanmıyor,
// if/while/return/switch içinde değil ve (void) ile açıkça yok sayılmıyor ->
// dönüş değeri YOKSAYILMIŞ demektir. 2e.8'den itibaren (void) cast'i
// "bilinçli yok sayma" olarak kabul ederiz.
const fs = require('fs');
const path = require('path');

const REPO = path.resolve(__dirname, '..', '..');
const ROOTS = ['Source/1.ConnectServer', 'Source/2.DataServer', 'Source/3.JoinServer', 'Source/4.GameServer'];

// (a) okuma hatalari
const READ_APIS = ['ReadFile', 'ReadFileEx', 'ReadProcessMemory', 'ReadConsole', 'fread', '_read', '_read_s',
  'recv', 'recvfrom', 'send', 'sendto', 'GetQueuedCompletionStatus', 'PeekNamedPipe', 'GetOverlappedResult'];
// (b) mesaj hatalari
const MSG_APIS = ['SendMessage', 'SendMessageA', 'SendMessageW', 'SendMessageTimeout', 'PostMessage',
  'PostThreadMessage', 'SendNotifyMessage', 'SendDlgItemMessage', 'SendDlgItemMessageA', 'SendDlgItemMessageW',
  'FindWindow', 'FindWindowA', 'FindWindowW', 'FindWindowEx', 'FindWindowExA', 'FindWindowExW',
  'GetMessage', 'PeekMessage', 'TranslateMessage', 'DispatchMessage', 'SendMessageCallback'];
// (c) yazma/yardimci — rapor kapsamina alinir ki sessizce dusen kalem olmasin
const IO_APIS = ['WriteFile', 'WriteFileEx', 'WriteConsole', 'SetFilePointer', 'SetFilePointerEx',
  'CreateFile', 'DeleteFile', 'RemoveDirectory', 'CreateDirectory', 'MoveFile', 'CopyFile',
  'DeviceIoControl', 'CreateFileMapping', 'MapViewOfFile', 'UnmapViewOfFile', 'FlushFileBuffers',
  'fwrite', 'fseek', 'ftell', 'fclose', '_commit', 'VirtualAlloc', 'VirtualFree',
  'socket', 'bind', 'listen', 'accept', 'connect', 'closesocket', 'select', 'setsockopt', 'ioctlsocket',
  'WSAStartup', 'WSACleanup', 'CreateThread', 'WaitForSingleObject', 'WaitForMultipleObjects'];

const ALL = [...READ_APIS, ...MSG_APIS, ...IO_APIS];
const RX = new RegExp('^(?:::)?(' + ALL.join('|') + ')\\s*\\(');

function* walk(dir) {
  for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) { if (!/^(Release|Debug|\.vs|obj|bin)$/i.test(e.name)) yield* walk(p); }
    else if (/\.(cpp|h)$/i.test(e.name)) yield p;
  }
}

function classify(file, lineNo, line) {
  const t = line.trim();
  // sonuc yakalanmis mi?
  if (/^(if|while|for|switch|return|else if)\b/.test(t)) return null;
  if (/^\(void\)/.test(t)) return null;                                     // bilerek yok sayildi
  if (t.includes('=') && t.indexOf('=') < t.indexOf('(')) return null;      // x = API(
  if (/^[\w:]+\s*=/.test(t)) return null;
  const m = RX.exec(t.replace(/^\s*/, ''));
  if (!m) return null;
  const api = m[1];
  if (api === 'GetMessage' && !/::/.test(t)) return null;                  // gMessage.GetMessage bir uye fonksiyon
  const cat = READ_APIS.includes(api) ? 'OKUMA' : MSG_APIS.includes(api) ? 'MESAJ' : 'YAZMA/yardimci';
  return { file, lineNo, api, cat, text: t };
}

const rows = [];
for (const r of ROOTS) {
  const root = path.join(REPO, r);
  if (!fs.existsSync(root)) continue;
  for (const f of walk(root)) {
    const lines = fs.readFileSync(f, 'latin1').split(/\r?\n/);
    for (let i = 0; i < lines.length; i++) {
      const hit = classify(path.relative(REPO, f), i + 1, lines[i]);
      if (hit) rows.push(hit);
    }
  }
}

rows.sort((a, b) => (a.cat + a.file + a.lineNo).localeCompare(b.cat + b.file + b.lineNo));
const byCat = { 'OKUMA': [], 'MESAJ': [], 'YAZMA/yardimci': [] };
for (const r of rows) byCat[r.cat].push(r);

console.log(`### Sunucu kaynaklarında dönüş değeri yok sayılan çağrı: ${rows.length}`);
for (const cat of ['OKUMA', 'MESAJ', 'YAZMA/yardimci']) {
  console.log(`\n===== ${cat} (${byCat[cat].length}) =====`);
  for (const r of byCat[cat]) console.log(`${r.file}:${r.lineNo}\t${r.api}\t${r.text.slice(0, 100)}`);
}
process.exit(0);