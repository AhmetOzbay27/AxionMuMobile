// Sentinel degerleri -> offset eslemesi.
// Uso: node map_sentinel.js baseline.bmd variant.bmd
const fs = require('fs');
const dec = (f) => { const b = Buffer.from(fs.readFileSync(f)); for (let i = 0; i < b.length; i++) b[i] ^= 0x20; return b; };
const A = dec(process.argv[2]);
const B = dec(process.argv[3]);

const numSentinels = {
	MaxGameInstances: 21, ReconnectTime: 22, CameraDefault: 23, AntiPort: 24,
	DefaultFPS: 25, SkillManaPet: 26,
	ButtonCharracter: 31, TabInfoStats: 32,
	BtnWcoinC: 33, BtnWcoinP: 34, BtnWcoinG: 35, BtnJwBless: 36, BtnJwSoul: 37, BtnJwChaos: 38, BtnZens: 39,
	Ranking1: 41, Ranking2: 42, Ranking3: 43, Ranking4: 44, Ranking5: 45, Ranking6: 46, Ranking7: 47,
	MaxLvDanhHieu: 48, MaxLvQuanHam: 49, MaxLvTuChan: 50, MaxLvHonHoan: 51,
	EnableCoinTitle: 52, TextTips3Line: 53, RF_GLOVE: 54, MG_HELM: 55,
	CreateCharSeason: 56, ButtonClassUP: 57, JewelBankTab: 58,
};
for (let i = 1; i <= 20; i++) numSentinels['MENU_BUTTON_' + String(i).padStart(2, '0')] = 100 + i;

const strSentinels = {
	CustomerName: 'RN1', ClientSerial: 'RN2', ClientVersion: '2.02.02',
	IpAddress: '10.20.30.40', WindowName: 'RN4', ClientName: 'RN5.exe',
	ScreenShotPath: 'RN6\\%d.jpg',
	ServerName_1: 'RN7', ServerName_2: 'RN8', ServerName_3: 'RN9', ServerName_4: 'RN10',
};

function findBytes(val) {
	const hits = [];
	for (let i = 0; i < B.length; i++) if (B[i] === val && A[i] !== B[i]) hits.push(i);
	return hits;
}
function findU16(val) {
	const hits = [];
	for (let i = 0; i + 1 < B.length; i++) if (B.readUInt16LE(i) === val && A.readUInt16LE(i) !== B.readUInt16LE(i)) hits.push(i);
	return hits;
}
function findU32(val) {
	const hits = [];
	for (let i = 0; i + 3 < B.length; i++) if (B.readUInt32LE(i) === val && A.readUInt32LE(i) !== B.readUInt32LE(i)) hits.push(i);
	return hits;
}
const hex = (n) => '0x' + n.toString(16);

console.log('--- byte sentineller (ilk 200 fark taramasi) ---');
for (const [k, v] of Object.entries(numSentinels)) {
	const hb = findBytes(v), h16 = [], h32 = findU32(v);
	const parts = [];
	if (hb.length) parts.push('byte@' + hb.map(hex).join(','));
	if (h32.length && !hb.length) parts.push('u32@' + h32.map(hex).join(','));
	if (parts.length) console.log(k.padEnd(18) + '= ' + v + '  ->  ' + parts.join('  '));
	else console.log(k.padEnd(18) + '= ' + v + '  ->  (farkta yok!)');
}

console.log('--- string sentineller ---');
for (const [k, v] of Object.entries(strSentinels)) {
	const hits = [];
	for (let i = 0; i + v.length <= B.length; i++) if (B.toString('latin1', i, i + v.length) === v) hits.push(i);
	// fark bolgeleriyle sinirla
	const diff = hits.filter((i) => { for (let j = i; j < i + v.length; j++) return true; return false; });
	console.log(k.padEnd(18) + '= "' + v + '"  ->  ' + diff.map(hex).join(','));
}
