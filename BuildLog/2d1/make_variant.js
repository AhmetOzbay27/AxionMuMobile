// 2d.1 kontrollü deney: canlı GetMainInfo.exe'yi sentinel girdilerle koşturup
// ini->ServerData/ConnectIP bayt eşlemesini çıkarmak için varyant ini üretir.
// Kullanım: node make_variant.js A|B
//   A: tüm sayısal anahtarlara benzersiz sentinel değerler
//   B: tüm metin anahtarlarına benzersiz sentinel dizeler
const fs = require('fs');
const path = require('path');

const sandbox = path.join(__dirname, process.argv[3] || 'sandbox', 'GetMain');
const src = fs.readFileSync(path.join(sandbox, 'GetEngine.ini.before'), 'utf8');
const round = process.argv[2] || 'A';

let text = src;

function setKey(src, key, value) {
	// 'key = deger' satirini bul (yorumlari koru); yok ise sona ekle
	const re = new RegExp('^(\\s*' + key.replace(/[.*+?^${}()|[\]\\]/g, '\\$&') + '\\s*=\\s*)([^\\r\\n;]*)(;?[^\\r\\n]*)$', 'm');
	if (!re.test(src)) { console.error('ANAHTAR YOK: ' + key); return src; }
	return src.replace(re, (m, p1, p2, p3) => p1 + value + (p3 || ''));
}

if (round === 'A') {
	// sayisal sentineller (kucuk, benzersiz)
	const nums = {
		MaxGameInstances: 21, ReconnectTime: 22, CameraDefault: 23, AntiPort: 24,
		DefaultFPS: 25.0, SkillManaPet: 26,
		ButtonCharracter: 31, TabInfoStats: 32,
		ButtonShopWcoinC: 33, ButtonShopWcoinP: 34, ButtonShopWcoinG: 35,
		ButtonShopJwBless: 36, ButtonShopJwSoul: 37, ButtonShopChaos: 38, ButtonShopZens: 39,
		Ranking1: 41, Ranking2: 42, Ranking3: 43, Ranking4: 44, Ranking5: 45, Ranking6: 46, Ranking7: 47,
		MaxLevelDanhHieu: 48, MaxLevelQuanHam: 49, MaxLevelTuChan: 50, MaxLevelHonHoan: 51,
		EnableCoinTitle: 52, TextTips3Line: 53, RF_GLOVE: 54, MG_HELM: 55,
		CreateCharSeason: 56, ButtonClassUP: 57, JewelBankTab: 58,
	};
	for (let i = 1; i <= 20; i++) nums['MENU_BUTTON_' + String(i).padStart(2, '0')] = 100 + i; // 101..120
	for (const [k, v] of Object.entries(nums)) text = setKey(text, k, String(v));
} else if (round === 'B') {
	const strs = {
		CustomerName: 'RN1', ClientSerial: 'RN2', ClientVersion: '2.02.02',
		IpAddress: '10.20.30.40', WindowName: 'RN4', ClientName: 'RN5.exe',
		ScreenShotPath: 'RN6\\%d.jpg',
		ServerName_1: 'RN7', ServerName_2: 'RN8', ServerName_3: 'RN9', ServerName_4: 'RN10',
	};
	for (const [k, v] of Object.entries(strs)) text = setKey(text, k, v);
}

fs.writeFileSync(path.join(sandbox, 'GetEngine.ini'), text);
console.log('round ' + round + ' yazildi: ' + (text.length - src.length) + ' bayt fark');
