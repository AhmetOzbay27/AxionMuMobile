const fs = require('fs');
const BS = String.fromCharCode(92);
const CRLF = '\r\n';
const J = a => a.join(CRLF);
const file = 'docs/00-PROJE-HARITASI.md';
let s = fs.readFileSync(file, 'utf8');
const a = '### 5.1 Sunucu yığınını açma / kapatma (test için)' + CRLF + CRLF + 'Açma (sırayla, kendi klasörlerinde):';
if (!s.includes(a)) throw new Error('5.1 anchor yok');
const note = J([
  '### 5.1 Sunucu yığınını açma / kapatma (test için)',
  '',
  '> **Denetim notu (04.10.2026):** Aşağıdaki yollar MuServer ağacındaki ikilileri çalıştırır.',
  '> Bunlar 30.09-02.10 tarihli **EX603** derlemeleridir ve 2e.6-2e.9 düzeltmelerini (H-018 hizası,',
  '> `ReadExact`, `CDataStore`) **içermez**. Güncel çalışma **Release_EX803** üretir; çıktıları',
  '> GS için `MuServe Classic 5.2 Lorencia' + BS + 'GameServer`, DS/JS/CS için `Source' + BS + '*' + BS + 'Release' + BS + '*_EX803` altına düşer.',
  '> İkililerin hizalanması **docs/33 K1** olarak açıktır; kapanana kadar bu bölümdeki yığın eski kodu koşar.',
  '',
  'Açma (sırayla, kendi klasörlerinde):',
]);
s = s.replace(a, note);
fs.writeFileSync(file, s, 'utf8');
console.log('5.1 notu eklendi', s.length);
