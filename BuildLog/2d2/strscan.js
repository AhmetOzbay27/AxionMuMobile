// strscan.js <exe-yolu> - string tarayici (ASCII + UTF-16LE), baglamli cikti
'use strict';
const fs = require('fs');
const p = process.argv[2] || 'C:/Axion Mu Mobile/Client and Tools/Client/Engine.exe';
const b = fs.readFileSync(p);
console.log('scan:', p, 'size', b.length);

function find(needle, utf16) {
  const s = Buffer.from(needle, utf16 ? 'utf16le' : 'latin1');
  const out = [];
  let i = 0;
  while ((i = b.indexOf(s, i)) >= 0 && out.length < 10) { out.push(i); i++; }
  return out;
}
function fmt(buf) { return buf.toString('latin1').replace(/[^\x20-\x7e]/g, '.'); }
function ctx(off, len) {
  const start = Math.max(0, off - 64);
  const buf = b.slice(start, off + len);
  console.log('   @0x' + off.toString(16).padStart(6, '0') + ' A| ' + fmt(buf).slice(0, 220));
  const t2 = buf.toString('utf16le').replace(/[^\x20-\x7e]/g, '.');
  if ((t2.match(/[A-Za-z0-9]{4,}/g) || []).length > 1) console.log('   @0x' + off.toString(16).padStart(6, '0') + ' U| ' + t2.slice(0, 220));
}
const terms = [
  'Launcher', 'launcher', 'already running', 'instance', 'Instance', 'single', 'Single',
  'Please run', 'run the', 'must be run', 'start via', 'MainCode', 'SPK.ini', 'CreateProcessA',
  'CreateProcessW', 'ShellExecute', 'WinExec', 'cmd.exe', 'VMProtect', 'GetModuleFileName',
  'Engine.exe', 'Engine', 'iU.spk', 'ERROR', 'Error:', '[Error]', 'failed', 'Failed',
  'cannot', 'Cannot', 'The input data', 'inconsistent', 'http://', 'https://', 'www.',
  '.php', '.com', 'patch', 'Patch', 'update', 'Update', 'download', 'Download', '.zip',
  'updater', 'Updater', 'Auto', 'auto', 'Version', 'version', 'serial', 'Serial', 'start', 'Start'
];
for (const t of terms) {
  const a = find(t, false), u = find(t, true);
  if (a.length || u.length) {
    console.log('TERM ' + JSON.stringify(t) + '  A:[' + a.map(x => '0x' + x.toString(16)).join(',') + ']  U:[' + u.map(x => '0x' + x.toString(16)).join(',') + ']');
    for (const off of a.slice(0, 2)) ctx(off, 40);
    for (const off of u.slice(0, 2)) ctx(off, 40);
  }
}
