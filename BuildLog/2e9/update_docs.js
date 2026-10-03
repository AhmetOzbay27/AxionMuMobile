const fs=require('fs'),path=require('path');
const R=path.resolve(__dirname,'..','..');
const DD=path.join(R,'Dashboard','data');
const TS='2026-10-03 22:55';
let fail=0;

{ // CHANGELOG
  const p=path.join(R,'docs/CHANGELOG.md');
  let s=fs.readFileSync(p,'utf8');
  const anchor='\n## [26.10.03 21:40]';
  const i=s.indexOf(anchor);
  if(i<0){console.log('HATA CHANGELOG girisi yok');fail++;}
  else{
    const entry=`
## [26.10.03 22:55] 2e.9 — 14 tablo için kalıcı veri katmanı + MEMB_INFO tutarlılığı doğrulaması (docs/29)

**Ne yapıldı**

\`CDataStore\` (DataStore.h/.cpp, 284+775 satır): 14 tablo için tipli kayıt
yapıları, hazırlanmış ifade (prepared statement) ve parametre bağlama. Hesap/
karakter adları artık SQL metnine gömülmüyor. Açılışta \`SchemaCheck()\` 14
tabloyu yoklar; eksikse sunucu bozuk semada sessizce çalışmaz. Yeni kullanıcı
DSN'i \`MuOnlineS6\` + DataServer.ini buna çevrildi.

**Önce düzeltilen varsayım:** JoinServer ile DataServer arasında protokol
akışı **yok** — JoinServer ConnectServer'a (63001), DataServer GameServer'dan
(63002) besleniyor. Ortak yüzey aynı veritabanındaki \`MEMB_INFO\`. Kullanıcı
kararıyla o doğrulandı.

**Bulunan ve düzeltilen GERÇEK KUSURLAR**

1. **DataNapGame.STT yok** — \`CB_AutoNapGame.cpp\` semada olmayan \`STT\`
   kolonunu okuyordu ve iki UPDATE'de \`and STT='%d'\` kullanıyordu; canlı DB'de
   \`Msg 207 Invalid column name 'STT'\`. Satır artık (Account,Name,Checking)
   üçlüsüyle tanımlanıyor.
2. **BindInt ölü adres** (uçtan uca test buldu) — parametre \`&val\` yerel
   kopyasına bağlanıyordu; ODBC tamponu \`SQLExecute\` anında okuduğu için
   **bütün tam sayı parametreleri bozuk** gidiyordu. \`m_ParamInt[]\` üyesine
   kopyalandı.

**Doğrulama**

* \`BuildLog/2e9/dstest/ds_e2e.exe\` gerçek MuOnlineS6 + MuOnline üzerinde:
  **TEST_EXIT=0 · 59 GECTI · 0 KALDI** (BuildLog/2e9/e2e_result.txt).
  14 tablo INSERT→LOAD→UPDATE→LOAD→DELETE; MEMB_INFO için iki sunucunun
  sorgusunun aynı değeri okuduğu, yazmaların birbirini gördüğü, olmayan
  hesapta aynı davrandığı doğrulandı.
* DataServer derlemesi **MSBUILD_EXIT=0** (DataServer.exe 1.062.912 B).

**AÇIK KALANLAR (düzeltilmedi)**
* \`gcoin\` kolonu ne MuOnline'da ne MuOnlineS6'da yok →
  \`CB_AutoNapGame.cpp\` gcoin sorguları çalışma anında hata veriyor.
* \`MuOnlineS6.Character\` boş → \`CustomNpcQuest\` FK testi ATLANDI.
* Mevcut 391 \`ExecQuery\` çağrısının 14 tabloya dokunan kısmı henüz katmana
  taşınmadı.

**Kanıt:** docs/29-14-TABLO-KALICI-VERI-KATMANI.md · BuildLog/2e9/
`;
    s=s.slice(0,i+1)+entry+s.slice(i);
    fs.writeFileSync(p,s,'utf8');
    console.log('CHANGELOG guncellendi');
  }
}

{ // pano
  const SP=path.join(DD,'sohbet.json');
  const s=JSON.parse(fs.readFileSync(SP,'utf8'));
  s.updated=TS;
  s.entries.unshift({ts:TS,kim:'ajan',text:
`14 tablo için kalıcı veri katmanı eklendi, JoinServer/DataServer tutarlılığı uçtan uca doğrulandı. (istek: "yeni oluşturulan 14 tablo için kalıcı veri katmanı ekleyip JoinServer ve DataServer arasındaki senkron akışını uçtan uca doğrula")

ÖNCE DÜZELTİLEN VARSAYIM
 İki sunucu arasında protokol akışı YOK. JoinServer ConnectServer'a (127.0.0.1:63001)
 bağlanıyor, DataServer GameServer'dan 63002'den istek alıyor. Ortak yüzey aynı
 veritabanındaki MEMB_INFO. (MuOnlineJoin bir veritabanı değil, MuOnline'a
 işaret eden bir DSN.) Kullanıcı kararı: MEMB_INFO veri tutarlılığı doğrulanacak.

VERİ KATMANI — CDataStore (DataStore.h 284 + DataStore.cpp 775 satır)
 • 14 tablo için tipli kayıt yapıları — semadan birebir (BuildLog/2e9/schema_14.txt)
 • Hazırlanmış ifade + parametre bağlama: hesap/karakter adları SQL'e gömülmüyor
 • SchemaCheck() açılışta 14/14 yoklama; eksikse sunucu bozuk semada çalışmaz
 • 7 yeni Delete yolu (karakter silme): CardPhone, EquipInventory, EventInventory,
   MuunInventory, MuRummyData, PcPointData, SNSData
 • Yeni kullanıcı DSN'i MuOnlineS6 → WIN-4TMUUQ42DNH\SQLEXPRESS / MuOnlineS6
 • DataServer.ini → MuOnlineS6

BULUNAN VE DÜZELTİLEN GERÇEK KUSURLAR
 1) DataNapGame.STT YOK — CB_AutoNapGame.cpp semada olmayan kolonu okuyor ve iki
    UPDATE'de "and STT='%d'" kullanıyordu → canlı DB: Msg 207 Invalid column name
    'STT'. Satır artık (Account,Name,Checking) üçlüsüyle tanımlanıyor.
 2) BindInt ölü adres bağlıyordu (uçtan uca testin bulduğu kusur): parametre &val
    yerel kopyasına bağlanıyordu, ODBC tamponu SQLExecute anında okuduğu için
    BÜTÜN tam sayı parametreleri bozuk gidiyordu. m_ParamInt[] üyesine kopyalandı.

DOĞRULAMA — TEST_EXIT=0 · 59 GECTI · 0 KALDI
 • BuildLog/2e9/dstest/ds_e2e.exe gerçek MuOnlineS6 + MuOnline üzerinde
 • 14 tablo: INSERT → LOAD → UPDATE → LOAD doğrula → DELETE
 • MEMB_INFO: iki sunucunun sorgusu aynı şifreyi okuyor; yazmalar birbirini
   görüyor; olmayan hesapta aynı davranış
 • DataServer derlemesi MSBUILD_EXIT=0 (DataServer.exe 1.062.912 B)
 • Ölçülen bilgi: COLLATE Latin1_General_BIN nedeniyle hesap araması BÜYÜK/küçük
   harf duyarlı; iki sunucu da aynı ifadeyi kullandığı için tutarlı

AÇIK KALANLAR
 • gcoin kolonu ne MuOnline'da ne MuOnlineS6'da yok → CB_AutoNapGame.cpp gcoin
   sorguları çalışma anında hata veriyor (düzeltilmedi)
 • MuOnlineS6.Character boş → CustomNpcQuest FK testi ATLANDI
 • 391 ExecQuery çağrısının 14 tabloya dokunan kısmı henüz katmana taşınmadı

Kanıt: docs/29-14-TABLO-KALICI-VERI-KATMANI.md · BuildLog/2e9/`});
  if(s.entries.length>15) s.entries=s.entries.slice(0,15);
  fs.writeFileSync(SP,JSON.stringify(s,null,2),'utf8');
  const RP=path.join(DD,'sonuc.json');
  const r=JSON.parse(fs.readFileSync(RP,'utf8'));
  r.updated=TS; r.status='ok'; r.text=s.entries[0].text;
  fs.writeFileSync(RP,JSON.stringify(r,null,2),'utf8');
  for(const f of [SP,RP]) fs.writeFileSync(f,fs.readFileSync(f,'utf8').replace(/\n+$/,''),'utf8');
  console.log('pano guncellendi · kayit='+s.entries.length);
}
console.log(fail===0?'### DOKUMAN+PANO TAMAM':`### ${fail} HATA`);
process.exit(fail===0?0:1);
