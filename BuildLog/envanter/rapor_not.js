const fs = require('fs');
const p = 'C:/Axion Mu Source/docs/14-SPK-PAKET-BASLIK-TARAMASI.md';
let t = fs.readFileSync(p, 'utf8');
const anchor = '**Bilinen limitler (rapordaki güven derecelerinin sebebi):**\r\n';
const bullet = '- Eşleşme isim+imza iledir; **sınıf adları farklı olabilir** (bizde `CB_BotTrader` ↔ canlı `CBotMixSystem`, `CCustomVongQuay` ↔ `CCustomLuckySpin`, `CCTCmini` ↔ `CastleStartGuild`) — aynı modülün yeniden adlandırılmış hali varsayımı; dayanak: fonksiyon gövde şekli ve problem alanı.\r\n';
if (!t.includes(anchor)) { console.log('HATA: capa bulunamadi'); process.exit(1); }
if (t.includes('sınıf adları farklı olabilir')) { console.log('zaten ekli'); process.exit(0); }
t = t.replace(anchor, anchor + bullet);
fs.writeFileSync(p, t);
console.log('eklendi; satir:', t.split('\r\n').length);
