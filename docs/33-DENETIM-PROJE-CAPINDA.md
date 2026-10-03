# 33 — PROJE ÇAPINDA DENETİM (amaç ↔ kaynak ↔ kanıt)

> **Tarih:** 04.10.2026 02:00 · **İstek:** "Bütün projeyi ve amaçları baştan aşağı tara;
> eksik/yanlış yapılmış bir şey var mı kontrol et."
> **Yöntem:** 33 doküman + git durumu + taze derleme (4 sunucu + Main) + canlı DB sorguları +
> ikili md5 karşılaştırması + H-018 layout betiğinin yeniden koşumu + belge sayım denetimi.
> **Ham kanıt:** `BuildLog/denetim/` (`build_server.log`, `build_main.log`,
> `viewport_layout_fresh.txt`, `check_json.js`, `edit_docs_*.js`).

---

## 0. ÖZET

| Ağırlık | Bulgu | Durum |
|---|---|---|
| 🔴 K1 | Dağıtım ikilileri bayat; EX803 çıktıları dağıtım yoluna akmıyor | **Açık — karar gerekiyor** |
| 🔴 K2 | Main derlemesi kırık (3C.0 işi, commit edilmemiş) | **Açık — sahibi: 3C.0 ajanı** |
| 🟠 O1 | Durum belgeleri (00/02/03) gerçekten geride | Bu turda güncellendi |
| 🟠 O2 | docs/30-31 sayımları kendi tablolarıyla uyuşmuyor | Bu turda düzeltildi + not düşüldü |
| 🟠 O3 | GetMainInfo ikilisi çalışma ağacında yeniden derlenmiş, commit'siz | Kayıt altına alındı |
| 🟠 O4 | H-018 kaynakta kapalı; çalıştırılabilir çiftte değil + denetleyici eşleme boşluğu | Açık |
| 🟡 H1-H4 | Hijyen: izlenmeyen çıktılar, §5.1 ikili yolu, dağınık açık kalemler, .gitignore | Öneri |

**Genel kanaat:** Mimari ve kanıt disiplini güçlü (2e.7-2e.9 kanıtları sağlam, docs/32
opcode sayımı birebir doğrulandı). Kırılma noktası **üretim hattı ile dağıtım yolu
arasındaki kopukluk** ve **durum belgelerinin güncellenmemesi**. "Yeşil" sayılan kalemlerin
bir kısmı yalnız kaynak düzeyinde; çalışan çift (Main.exe + GameServer.exe) hâlâ H-018 öncesi.

---

## 1. DOĞRULANAN İDDİALAR (04.10.2026 taze ölçüm)

| İddia | Ölçüm | Sonuç |
|---|---|---|
| Sunucu hattı derleniyor | GS/DS/JS/CS `Release_EX803` MSBuild → `EXIT=0` ×4 | ✅ |
| DB'de 14 tablo | `MuOnlineS6` sorgusu → 14/14 | ✅ |
| `DataNapGame`'de STT yok | kolonlar: Account, Name, TienNap, Checking, Status | ✅ |
| `gcoin` hiçbir DB'de yok (B6) | `MuOnline`=0, `MuOnlineS6`=0 | ✅ |
| docs/32 opcode envanteri | `Protocol.cpp` benzersiz `case 0x..` = **166** | ✅ |
| H-018 4/4 hizalı | Elle ofset karşılaştırması: PLAYER 49, CHANGE 51, MONSTER 21, SUMMON 31 — istemci yapıları aynı sırada (`WSclient.h:560-665`, `HAISLOTRING=1` → `Defined_Global.h:78`) | ✅ (kaynak) |
| 2e.9 doğrulaması | `BuildLog/2e9/e2e_result.txt` 59/0 + üretim ikilisi logu (`[DataStore] … 14/14`) | ✅ |
| 2e.8 tarama kapsamı | `scan_ignored_returns_after.txt` + ölçülmüş sınır (102 çağrı, docs/28 §8) | ✅ ölçülü |
| Pano/commit zinciri | `sohbet.json` 15 kayıt, JSON geçerli, CR=0; `HEAD == origin/main` | ✅ |
| Sunucu yığını kapalı (§5.1) | 4 süreç yok (`tasklist`) | ✅ |

---

## 2. KRİTİK BULGULAR

### K1 — Dağıtım ikilileri bayat; EX803 çıktıları dağıtım yoluna akmıyor

Ölçüm (stat + md5 + `vcxproj` OutDir):

| Bileşen | Güncel derleme (EX803) | Nereye yazıyor | MuServer'daki ikili | md5 |
|---|---|---|---|---|
| GS | 03.10 21:12 · 11.294.720 B | `MuServe Classic 5.2 Lorencia\GameServer` (izlenmiyor) | 02.10 12:45 · 10.824.704 B | farklı (`e327e349…` vs `43f086d9…`) |
| DS | 03.10 22:48 · 1.062.912 B | `Source\2.DataServer\DataServer\Release\` | 30.09 · 1.030.656 B | farklı (`dd6f1138…` vs `916e9625…`) |
| JS | 03.10 21:11 · 943.616 B | `Source\3.JoinServer\JoinServer\Release\JoinServer_EX803\` | 30.09 · 943.616 B | farklı (`1fa62a47…` vs `91ee93d7…`) |
| CS | 03.10 21:11 · 103.936 B | `Source\1.ConnectServer\ConnectServer\Release\ConnectServer_EX803\` | 30.09 · 103.936 B | farklı (`35a4b736…` vs `4c1f91bf…`) |

**Etki:**
1. `docs/00 §5.1` ve `BuildLog/2e3/deploy_server_test.sh` (`SRC=MuServer`) ile açılan test
   yığını, 2e.6-2e.9 düzeltmelerini (H-018 hizası, `ReadExact`, `CDataStore`, 14 tablo)
   **içermiyor**; eski kodu çalıştırır.
2. H-018 sonrası **yeniden derlenmiş istemci ikilisi yok**: `ClientFile/Main.exe`
   03.10 01:00 (2e.5) tarihli, yani H-018 öncesi. Kaynak istemci EX803 düzeni okuyor,
   dağıtımdaki GS EX603 düzeni (02.10) — kaynak çifti ile ikili çifti ayrıştı.
3. GS EX803 çıktısı proje kökünde `MuServe Classic 5.2 Lorencia\` adlı **izlenmeyen**
   klasöre düşüyor (vcxproj'da devralınmış OutDir); yani en güncel GS ikilisi hiçbir
   resmî/izlenen konumda değil.

**Kök neden:** 2e.6'dan sonra dağıtım konfigürasyonu EX803'e geçti; ancak OutDir yolları
EX603'te `MuServer\...`'a, EX803'te eski/dış yollara bakıyor. Derleme çıktısını dağıtıma
taşıyan adım (2e.3 betiği benzeri) 2e.6'dan sonra hiç koşulmadı.

**Öneri (karar):** "Dağıtım = EX803" kararı sabitse ya EX803 OutDir'leri `MuServer\...`'a
çevrilmeli ya da her tur sonunda `Release_EX803` çıktıları `MuServer\...`'a kopyalanıp
commit'lenmeli. Ardından H-018 sonrası Main.exe derlenip paket testi (2e.2/2e.5 betikleri)
tekrarlanmalı.

### K2 — Main derlemesi kırık (3C.0 işi, commit edilmemiş)

`MSBuild Source/5.Main/Main.vcxproj "Global Release"` → **EXIT_Main=1**
(`BuildLog/denetim/build_main.log`).

| # | Dosya | Hata | Kök neden |
|---|---|---|---|
| 1 | `SPKData.h:40` | `SPK_CAMERA_FPS_OFFSET` tanımsız (C2065) + `memcpy` 2 arg (C2660, `SPKData.cpp:203`) | İki `#define` **tek satıra yapışmış**; ikincisi `// ...` yorumunun içinde kalmış (eksik satır sonu) |
| 2 | `SPKMenuBar.cpp:158` | `DisplayWidth` tanımsız (C2065) | Değişken projede hiçbir başlıkta yok |
| 3 | `SPKMenuBar.cpp:181` | `g_pBCustomMenuInfo` tanımsız (C2065) | `NewUISystem.h` include edilmemiş (makro orada: `NewUISystem.h:366`) |
| 4 | `BuildLog\5Main\vc143.pdb` | C1033 açılamıyor (06:31 tarihli) | Paralel derleme artığı kilitli/bozuk PDB |

Kanıt: `BuildLog/3c0/build_3c0.log` (03.10 06:31 — aynı 4 sözdizimi hatası) + bu turun
taze koşumu. Dosyalar 03.10 06:25–06:31 tarihli ve **commit edilmemiş**
(`M SPKData.cpp/.h`, `?? SPKMenuBar.cpp/.h`, `M CBInterface.cpp`, `M Main.vcxproj*`).
Bu iş docs/31 §5'teki **3C.0** kalemidir.
**Düzeltilmedi:** sahibi başka ajan; dosyalarına dokunulmadı (kullanıcı kuralı).

**Sonuç:** Faz 2 çıkış kriteri ("GS/Main/CS/DS/JS derlemeleri hatasız") **şu an sağlanmıyor**.

---

## 3. ORTA BULGULAR

### O1 — Durum belgeleri gerçekten geride (bu turda güncellendi)

| Belge | Bulgu |
|---|---|
| `docs/00` durum panosu | 2b'de kalmıştı: 2c.1-A1/A2/A3, B1/B2/B3, 2d, 2e.2-2e.9 ve docs/30-32 yansımıyordu |
| `docs/02` yol haritası | 2c.1 "⬜" yazıyordu (gerçekte tamam); 2e.6-2e.9 hiç yoktu; çıkış kriteri yeşil değilken Faz 3.1/3.2 başlatılmıştı |
| `docs/03` eksik listesi | C-01b hâlâ ⬜ (2c.1-B2 ile kapandı); A-02 hâlâ "2c bekliyor" (6 kalem tamam); B-02/B-03/D-01/D-02 denetlenmemiş |

**Etki:** "Nerede kaldık?" sorusunun tek güvenilir cevabı artık CHANGELOG + pano + docs/13/30;
yeni bir ajan docs/00-03'e bakarsa yanlış yönlenir. Bu turda üçü de güncellendi.

### O2 — docs/30/31 sayımları kendi tablolarıyla uyuşmuyordu (düzeltildi)

| Belge | İddia | Tablo sayımı | İşlem |
|---|---|---|---|
| docs/30 §1 | GS 11/60 | **10** (6 ana + §3'teki 4) | 10'a çekildi + denetim notu |
| docs/30 §1 | İstemci 8/60 | **7** (A1/A8/B3/C47/C39/C41/C49; "core MU" parantezliler hariç) | 7'ye çekildi + not |
| docs/30 §1 | "~31 kutu" | config dışlanmış görünüyor (10+7+6+6=29); kural belirsiz | Not düşüldü |
| docs/30 §0/§5 | Ham veri yolu bozuk (`BuildLogenvanter…`) | dosya var: `BuildLog\envanter\parite_ozellik_kaniti.tsv` (2.816 B) | Yol düzeltildi |
| docs/31 §1 | "6'sı (%30); 14'ü" | **5** ✅ (01/05/06/07/08); **15** yazılmamış | 5'e çekildi + not |

Ayrıca **docs/30 ile docs/31 çelişiyor:** A16 (`B_MocNap`) satırında istemci karşılığı
"YOK" yazıyor, ama `CB_AutoNapGame` istemcide var ve docs/31 slot 08'de sayıyor. docs/30
notuna işlendi; istemci sayısı A16 düzeltilirse 8'e çıkar.

### O3 — GetMainInfo ikili sapması (kayıt altına alındı)

- `GetMain/GetMainInfo.exe` çalışma ağacında **03.10 15:02'de yeniden derlenmiş**
  (md5 `1991c037…`), commit edilmemiş.
- HEAD sürümü md5 `c480e0ba…` — docs/20'nin iddiası bu sürüme ait ve doğru.
- Yeniden derlemenin kim/neden olduğu hiçbir yerde kayıtlı değil (2d.1 D4/D5 hâlâ açıkken
  ilgisiz bir ara derleme olabilir). **Karar:** commit edilmedi, olduğu gibi bırakıldı;
  sahibi çıkarsa commit'lensin, çıkmazsa `git checkout --` ile HEAD'e döndürülsün.

### O4 — H-018: kaynakta kapalı, çalıştırılabilir çiftte değil

- **Kaynak doğrulaması sağlam:** 4/4 yapı elle doğrulandı; sunucu (803/1) ve istemci
  yapıları arasında bayt farkı yok (bkz. §1 satırı).
- **Denetleyici script zayıf noktası:** `viewport_layout.js` 803/1 koşumunda `attribute`,
  `level`, `MaxHP`, `CurHP` alanları için "istemcide karşılığı yok" yazıp yine "HIZALI"
  diyor (eşleme tablosunda bu adlar yok; karşılaştırma yalnız eşlenen çapalara dayanıyor).
  Çıktı bu hâliyle yanıltıcı; eşlemeler eklenmeli.
- **Runtime doğrulaması yok:** H-018 sonrası Main hiç başarıyla derlenmedi; `ClientFile/Main.exe`
  H-018 öncesi. Yani "çalışan çift" (Main.exe + MuServer GS) hâlâ H-018 öncesi davranışta;
  kaynak çifti (yeni Main + EX803 GS) ise hiç paketlenmedi.
- **Kabul edilen fark:** EX803 dağıtımı canlıdan PLAYER/CHANGE 13, MONSTER 1, SUMMON 11 bayt
  farklı (kullanıcı kararı). Canlı bayt paritesi istenirse EX603 + `HAISLOTRING=0` yeterli.

---

## 4. DÜŞÜK / HİJYEN

- **H1 — İzlenmeyen üretim artıkları:** `MuServe Classic 5.2 Lorencia/` (GS EX803 çıktısı),
  `Source/*/Release/` (CS/DS/JS EX803 çıktıları), `BuildLog/{3c0,5Main,Getmain,github}`.
  Depo durumu her derlemede kirleniyor; `.gitignore`'a `*.iobj`, `*.ipdb`, `*.pch`,
  `*.tlog/`, `*.recipe` ve `Source/*/Release/` eklenmeli (veya bilinçli commit kuralı yazılmalı).
- **H2 — `docs/00 §5.1` ikili yolu:** yol listesi MuServer'daki EX603 ikililerini işaret ediyor;
  K1 kapanana kadar bu bölüme "hangi konfigürasyon/ne zaman güncellendi" notu şart.
- **H3 — Açık kalemler dağınık:** docs/03 · 04 · 17 §6 · 28 §8 · 29 §8 · 30 §3 · 31.
  Tek merkezî liste yok; aşağıda derlendi (§5).
- **H4 — Süpervizör kuralı ilk kez uygulandı:** docs/30 §4 "kanıt denetimi olmadan kalem
  yeşile dönmez" diyor; bu rapor o işlevin ilk örneğidir. Rutine bağlanmalı (her tur sonunda
  en az bir bağımsız ölçüm).

---

## 5. AÇIK KALEM ENVANTERİ (04.10.2026)

| # | Kalem | Durum | Kayıtlı yer |
|---|---|---|---|
| 1 | K1 dağıtım ikilileri + H-018 sonrası istemci paketi | 🔴 Açık | **docs/33 §2** |
| 2 | K2 Main derlemesi (3C.0) | 🔴 Açık | docs/33 §2, docs/31 §5 |
| 3 | B-03 GetMainInfo D4/D5 (tam jeneratör + RenderEffect) | 🔄 1/2 | docs/03, docs/20 |
| 4 | B-02 SPK istemci içerik varlıkları | ⬜ | docs/03 |
| 5 | D-01/D-02 string sapmaları | ⬜ | docs/03 |
| 6 | A-02 kalan modüller (54'ün 48'i) + A3 E2E | 🔄 | docs/03, docs/13, docs/18 |
| 7 | B3 ActiveInvasions 4 parite farkı (void overload + push + `SendThongTinSauKhiVaoGame` bizde fazla; respawn `monster_add(true)` eksik) | ⬜ | docs/17 §6 |
| 8 | `gcoin` kolonu yok — `CB_AutoNapGame` sorguları hata verir | ⬜ | docs/29 §8 |
| 9 | 102 okuma çağrısı (88 `GetPrivateProfileString` adayı + 4 `UuidCreateSequential`) | ⬜ | docs/28 §8 |
| 10 | 14 tabloya dokunan ~103 `ExecQuery` çağrısı katmana taşınmadı | ⬜ | docs/29 §8 |
| 11 | A3 `CustomNpcQuest` FK testi atlandı (boş Character) | ⏭ | docs/29 §8 |
| 12 | 3.2b login/karekter akışı — etkileşimli oturum (H-017) | 🟡 | docs/02, docs/04 |
| 13 | Faz 4 Android / Faz 5 canlı geçiş | ⬜ | docs/02 |
| 14 | docs/30 §3: 2b.0'ın 4 özelliği (GS+CFG ✅, istemci/E2E ⬜) | ⬜ | docs/30 |

---

## 6. ÖNERİLEN SIRADAKİ ADIMLAR

1. **K2:** 3C.0 sahibi `SPKData.h:40` satır sonunu ve `SPKMenuBar.cpp` include/tanım
   eksiklerini düzeltip Main'i derlesin; `BuildLog/5Main/vc143.pdb` temizlensin.
2. **K1 kararı:** EX803 OutDir'leri `MuServer\...`'a yönlendirilsin **veya** tur sonu
   "dağıtım kopyalama + commit" rutini kurulsun. Ardından Main.exe + paket E2E (2e.2/2e.5)
   yeniden koşulsun → H-018 ilk kez çalışan çiftte doğrulanır.
3. **Açık kalem listesi** (§5) tek dosyada tutulsun; her tur sonu güncellensin.
4. **docs/30/31** sonraki güncellemede sayım kuralını sabitlesin (hangi sütun "yeşil" sayılır).

---

## 7. KANIT DİZİNİ

| Dosya | İçerik |
|---|---|
| `BuildLog/denetim/build_server.log` | GS/DS/JS/CS EXIT=0 (Release_EX803) |
| `BuildLog/denetim/build_main.log` | Main EXIT=1 + C1033 kanıtı |
| `BuildLog/denetim/viewport_layout_fresh.txt` | H-018 layout koşumu (803/1, EXIT=0) |
| `BuildLog/denetim/check_json.js` | Pano JSON bütünlüğü (CR/BOM/parse) |
| `BuildLog/denetim/edit_docs_*.js` | Bu turdaki belge düzeltmelerinin betikleri |
