// fix_scope_claim.js - CHANGELOG 2e.8 girdisine (a) tarayici negatif kontrolu,
// (b) "OKUMA 0" iddiasinin olculmus kapsam siniri eklenir.
const fs=require('fs'),path=require('path');
const R=path.resolve(__dirname,'..','..');
const p=path.join(R,'docs/CHANGELOG.md');
let s=fs.readFileSync(p,'utf8');
const anchor='**Kanıt:** docs/28-OKUMA-MESAJ-HATASI-TARAMASI.md';
const i=s.indexOf(anchor);
if(i<0){console.log('HATA: anchor yok');process.exit(1);}
if(s.slice(0,i).includes('scanner_selfcheck.js')){console.log('zaten uygulanmis');process.exit(0);}
const add=`**Tarayıcının kendisi de sınandı (negatif kontrol)**
\`BuildLog/2e8/scanner_selfcheck.js\` bilerek kaçak bir \`ReadFile\` ve bir
\`SendMessage\` yerleştirip sayımın her birini +1 arttırdığını, buna karşılık
atama / \`(void)\` / \`if\` biçimlerini atladığını (toplam +2, +5 değil)
doğruluyor → **EXIT=0**. "0/0" sonucunun bozuk bir tarayıcıdan gelmediği kanıtı.

**"OKUMA 0 · MESAJ 0" ifadesinin ölçülmüş sınırı**
Bu sonuç tarayıcının tanımlı API aileleri içindir. Aynı kural, listelerde
olmayan okuma ailesine uygulandığında (\`read_gap_probe.js\`) **102** çağrı
daha çıkıyor: \`GetPrivateProfileString\` 88 (sistematik sessiz ayar hatası —
\`ReadFile\` kusuruyla aynı sınıf), \`UuidCreateSequential\` 4 (gerçek kusur:
\`UUID\` başlatılmıyor, donanım kimliği stack artığından türetiliyor),
\`localtime\` 4 / \`localtime_s\` 2 / \`ungetc\` 4 (kusur değil). Ayrıntı docs/28 §8.

`;
s=s.slice(0,i)+add+s.slice(i);
fs.writeFileSync(p,s,'utf8');
console.log('CHANGELOG kapsam notu eklendi');
