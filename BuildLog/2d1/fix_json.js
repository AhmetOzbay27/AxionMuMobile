// Pano JSON'larinda ters bolu kacislarini '/' ile degistir (gecerli JSON icin).
const fs = require('fs');

const fixes = [
	'client\\ClientName',
	'client\\Engine.exe',
	'client\\Data\\Player\\player.bmd',
	'Config\\Info',
];

for (const f of ['Dashboard/data/sonuc.json', 'Dashboard/data/sohbet.json']) {
	let s = fs.readFileSync(f, 'utf8');
	let total = 0;

	for (const bad of fixes) {
		const good = bad.split('\\').join('/');
		const parts = s.split(bad);
		total += parts.length - 1;
		s = parts.join(good);
	}

	fs.writeFileSync(f, s);
	console.log(f + ': ' + total + ' duzeltme');
}
