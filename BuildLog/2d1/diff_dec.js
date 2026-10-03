// decode (XOR 0x20) sonrasi iki dosyanin fark bolgelerini listeler.
// Kullanim: node diff_dec.js A.bmd B.bmd [maxRanges]
const fs = require('fs');
const dec = (f) => { const b = Buffer.from(fs.readFileSync(f)); for (let i = 0; i < b.length; i++) b[i] ^= 0x20; return b; };
const a = dec(process.argv[2]);
const b = dec(process.argv[3]);
const maxN = parseInt(process.argv[4] || '200', 10);

if (a.length !== b.length) { console.log('BOYUT FARKI', a.length, b.length); process.exit(1); }

let ranges = [], start = -1;
for (let i = 0; i < a.length; i++) {
	const d = a[i] !== b[i];
	if (d && start < 0) start = i;
	if (!d && start >= 0) { ranges.push([start, i - 1]); start = -1; }
}
if (start >= 0) ranges.push([start, a.length - 1]);

let total = 0; for (const [s, e] of ranges) total += e - s + 1;
console.log('boyut ' + a.length + ' | fark bolge=' + ranges.length + ' | fark bayt=' + total);

const show = (buf, s, e) => {
	let r = [];
	for (let i = s; i <= Math.min(e, s + 15); i++) r.push(buf[i].toString(16).padStart(2, '0'));
	return r.join(' ');
};

for (const [s, e] of ranges.slice(0, maxN)) {
	console.log('  ' + s.toString(16) + '-' + e.toString(16) + ' (' + (e - s + 1) + 'B)');
	console.log('    A: ' + show(a, s, e) + (e - s + 1 > 16 ? '...' : '') + '  |  B: ' + show(b, s, e));
	// hizalama icin: bolge dword sinirlarina denk geliyorsa dword yorumu
	const ds = s - (s % 4);
	const de = e - (e % 4);
	if (ds % 4 === 0 && de - ds <= 16) {
		let la = [], lb = [];
		for (let o = ds; o <= de; o += 4) { la.push('0x' + a.readUInt32LE(o).toString(16).toUpperCase()); lb.push('0x' + b.readUInt32LE(o).toString(16).toUpperCase()); }
		console.log('    A dword@' + ds.toString(16) + ': ' + la.join(' ') + '  |  B: ' + lb.join(' '));
	}
}
