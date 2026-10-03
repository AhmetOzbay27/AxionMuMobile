// trap.js - 63000 portunda istemci baglantisini yakalayip ham baytlari loglar
// Kullanim: node trap.js [port] [logfile] [replyHex] [replyDelayMs]
const net = require('net');
const fs = require('fs');

const port = parseInt(process.argv[2] || '63000', 10);
const logFile = process.argv[3] || 'C:\\Axion Mu Source\\BuildLog\\2e4\\trap.log';
const replyHex = process.argv[4] || '';
const replyDelayMs = parseInt(process.argv[5] || '0', 10);

function ts() { return new Date().toISOString().substr(11, 12); }
function log(line) {
  const text = ts() + ' ' + line;
  console.log(text);
  fs.appendFileSync(logFile, text + '\n');
}

const server = net.createServer((sock) => {
  log('CONNECT from ' + sock.remoteAddress + ':' + sock.remotePort + ' (local ' + sock.localPort + ')');
  let total = 0;
  sock.on('data', (buf) => {
    total += buf.length;
    log('DATA len=' + buf.length + ' total=' + total + ' hex=' + buf.toString('hex').toUpperCase());
    log('DATA asc="' + buf.toString('latin1').replace(/[^\x20-\x7e]/g, '.') + '"');
    if (replyHex) {
      const payload = Buffer.from(replyHex, 'hex');
      const delay = replyDelayMs;
      setTimeout(() => {
        try { sock.write(payload); log('REPLY sent len=' + payload.length); } catch (e) { log('REPLY err ' + e.message); }
      }, delay);
    }
  });
  sock.on('close', () => log('CLOSE from ' + sock.remoteAddress + ':' + sock.remotePort + ' after ' + total + ' bytes'));
  sock.on('error', (e) => log('SOCKET-ERROR ' + e.message));
});

server.on('error', (e) => log('SERVER-ERROR ' + e.message));
server.listen(port, '0.0.0.0', () => log('TRAP listening on 0.0.0.0:' + port + (replyHex ? ' replyHex=' + replyHex + ' delay=' + replyDelayMs + 'ms' : ' (no reply)')));
