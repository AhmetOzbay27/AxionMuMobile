# 24 — 2e.4 SPK-FIRST VARLIK ÇÖZÜMLEYİCİ + YOL PARİTESİ

> Adım: **2e.4** ([02-YOL-HARITASI.md](02-YOL-HARITASI.md) Faz 2e; keşif: docs/22 §4).
> Tarih: 02.10.2026.
> Amaç: istemcinin MUIG kalıbındaki veri yollarını (`Data\Local\*`, `World%d`,
> `Data\Object*`) canlı SPK paketinin **gerçek** düzenine bağlamak; dağıtım
> betiğindeki geçici yol eşlemesine bağımlılığı kaldırmak (H-012) ve bu yolla
> istemciyi canlı sunucuya bağlanma aşamasına kadar diyalogsuz çalıştırmak.

## 1. Özet

- **Yeni modül:** `Source\5.Main\source\SPKAsset.cpp` (+ `SPKData.h` bildirimleri):
  `SPK_ResolveAssetPath()` / `SPK_AssetExists()` — SPK-first yol çözümleyici.
- **20+ çağrı noktası** çözümleyiciye geçirildi (aşağıda tam liste).
- **Harita/nesne yol paritesi (H-015):** canlı Engine string kanıtına göre
  `Map\World%d` + `Data\Map\Object*` düzenine geçildi (WorldName üreten 4 dosya +
  `CLoadData::AccessModel` / `OpenTexture` / `LoadBitmap` merkezi noktaları).
- **Kapanan hatalar:** H-011 (CSprite boş doku AV), H-012 (MUIG↔SPK varlık düzeni),
  H-014 (VIPCharRank fatal diyalogu — yeni), H-015 (harita/nesne yol uyumsuzluğu — yeni).
- **Kanıt serisi run11→run17:** run14'te istemci **hiçbir diyalog açmadan** ve
  çökmeden **canlı sunucuya bağlanma aşamasına** ulaştı
  (`45.87.120.29:44405`, t=5,2 s, SynSent). Kaynaktaki 26 bozuk EUC-KR satırı
  geri konulup yeniden derlendikten sonra run15–17 ile doğrulama yinelendi
  (bkz. §7 onarım notu).

## 2. SPK-first çözümleyici (`SPKAsset.cpp`)

`SPK_ResolveAssetPath(const char* requested)` isteği aşağıdaki kurallarla canlı
paketteki karşılığına çevirir (statik tampon döner); kural uymazsa **isteği aynen
döndürür** (eski davranış koşulsuz korunur):

| MUIG isteği | Canlı SPK karşılığı |
|---|---|
| `Data\Local\<Ad>.bmd` | `Data\SPK\Config\<Ad>.bmd` |
| `Data\Local\<Lang>\<Ad>_<Lang>.<ext>` | `Data\SPK\Config\<Ad>.<ext>` |
| `Data\Local\<Lang>\NpcName(<Lang>)` | `Data\SPK\Config\NpcName.txt` |
| `Data\Gate.bmd` | `Data\SPK\Config\Gate.bmd` |
| küçük harf + `/` ayraç varyantları | aynı kurallar (normalize edilir) |

- `SPK_AssetExists()` — çağıran tarafın tablo yoksa **ölümcül olmayan atlama**
  yapması için (tooltip yükleyicileri: log + `return`).
- Resolver `Data\SPK\Minimap\Minimap_<World>_<Lang>.bmd` yolu için de
  `NewUIMiniMap::LoadImages` içinde devrede (önce SPK, yoksa eski `Data\Local`
  fallback'i).

## 3. Çağrı noktaları (çözümleyiciye geçen dosyalar)

`ZzzOpenData.cpp` (OpenDialogFile, OpenItemScript, 3 tooltip yükleyicisi,
OpenMoveReqScript, OpenMonsterScript, QuestScript, OpenSkillScript, SocketItem,
OpenGateScript, OpenFilterFile, OpenNameFilterFile, OpenMonsterSkillScript,
MasterSkillTreeData, MasterSkillTooltip, GlobalText.Load), `ServerListManager.cpp`,
`QuestMng.cpp` (NPCDialogue/QuestProgress/QuestWords), `CreditWin.cpp`,
`ItemAddOptioninfo.cpp`, `w_PetProcess.cpp`, `NewUISlideWindow.cpp` (Slide_),
`UIJewelHarmony.cpp` (JewelOfHarmonyOption_), `w_BuffScriptLoader.cpp`
(BuffEffect_; küçük harf `data/local/<Lang>/...` varyantı dahil), `CSItemOption.cpp`
(ItemSetType/ItemSetOption).

## 4. H-011 — CSprite boş doku AV guard

`Sprite.cpp` → `CSprite::Create`: `FindTexture` sonrası doku bulunamazsa
(`m_nTexID > -1 && m_pTexture == NULL`) log yazıp `m_nTexID = -1` yapılır; mevcut
`else` dalı temiz kurulum yapar → erişim ihlali yok. run12/13/14'te KEN.txt'deki
`LoadBitmap Failed: ...` satırlarına rağmen **hiç AV/çökme olmadı** (önceki run9
AV'sinin tersi) — fix doğrulandı.

## 5. H-014 — VIPCharRank fatal diyalogu (run12 → run13)

`CBInterfaceVIPChar.cpp` PC dalı, `Data\Custom\VIPCharRank.txt` yoksa
`CMemScript` `ErrorMessageBox` → `ExitProcess` yapıyordu. Dosya **canlı pakette de
yok** (audit `LIVE-`). Fix: PC dalında dosya varlığı kontrol edilir; yoksa log +
`return` (Android dalının davranışıyla aynı; fatal değil). run12'de diyalog
`[Data\Custom\VIPCharRank.txt] Could not open file` idi; run13'te bu diyalog
**yok** (yeni frontier'a geçildi).

## 6. H-015 — Harita/nesne yol paritesi (run13 → run14)

Canlı `Engine.exe` string tablosundan kanıt:
`Map\World%d` + `Data\%s\EncTerrain%d.map` çifti ve `Data\Map\Object%d\`,
`Data\Map\Object1..67` yolları. Canlı pakette haritalar `Data\Map\World74\...`,
nesneler `Data\Map\Object*\...` altında; bizim kaynak `Data\World74\...` /
`Data\Object*` istiyordu → run13'te
`Data\World74\EncTerrain74.map file corrupted1 (74/-1)` (ölümcül, WM_DESTROY).

Yapılanlar:

1. **WorldName üretimi** (`"World%d"` → `"Map\\World%d"`):
   `MapManager.cpp` (1262), `GMBattleCastle.cpp` (92), `GMCrywolf1st.cpp` (154),
   `GM_Kanturu_3rd.cpp` (78). `SaveWorld` (ölü kod, editör) bilinçli değiştirilmedi.
2. **Merkezi noktalar** (tüm çağrıları tek yerden kapsar):
   - `CLoadData::AccessModel`: `Data\Object*` → `Data\Map\Object*`
   - `CLoadData::OpenTexture`: `Object<N>\` alt klasörü → `Map\Object<N>\`
   - `LoadBitmap(ZzzTexture.cpp)`: `Data\Object*` → `Data\Map\Object*`
     (canlıda kökte `Data\Object*` yok — çakışma riski yok)
3. **Minimap:** önce `Data\SPK\Minimap\Minimap_<World>_<Lang>.bmd`, yoksa eski
   `Data\Local\<Lang>\Minimap\...` (canlı dosya listesi SPK\Minimap altında:
   World1/2/3/4/11/32/34/35/38/39/42/52; World74 yok → run14'te sessiz atlama).
4. **Deploy betiği:** `--with-assets` artık giriş sahnesi harita alt kümesini
   (`Data\Map\World74` + `Data\Map\Object74`, ~7 MB) da kurar; tam `Data\Map`
   ağacı ~562 MB olduğundan disk sınırı nedeniyle kopyalanamaz.

Doğrulama izleri (run14 KEN.txt): `LoadBitmap Failed: Data\Map\Object8\drop01.jpg`
satırı artık **yeni yolu** gösteriyor (nesne yolu çözümlemesi devrede; dosya
pakette yok — zararsız log).

## 7. Test serisi (run11–run17)

| Run | Derleme md5 | Sonuç / frontier |
|---|---|---|
| run11 | `e0790162…` | `data/local/Eng/BuffEffect_Eng.bmd - File not exist.` → küçük harf/slash desteği eklendi |
| run12 | `001246e8…` | BuffEffect aşıldı; `VIPCharRank.txt` fatal diyalogu (H-014) |
| run13 | `e6298085…` | VIPChar dialoğu **yok**; `EncTerrain74.map file corrupted1` (H-015) |
| **run14** | **`b386bd65c4fbdc1aec840adfcfb8af8a`** (12.029.952 B) | **Diyalog yok, çökme yok, kalıntı süreç yok**; pencere 0,9 s; t=5,2 s `45.87.120.29:44405` **SynSent** (canlı sunucuya bağlanma aşaması) |
| run15 | `6dc52f5f…` | Onarım sonrası derleme: diyalog yok, çökme yok, kalıntı süreç yok; pencere 0,9 s (40 sn gözlemde TCP görülmedi) |
| run16 | `6dc52f5f…` | Aynı sonuç (45 sn gözlem; bağlantı girişimi gözlem penceresi dışında kaldı) |
| **run17** | **`6dc52f5fb63746cb51575ab1370f7e6c`** (12.029.952 B) | **Diyalog yok, çökme yok**; pencere 0,8 s; t=21,9 s `45.87.120.29:44405` **SynSent** (70 sn gözlem) |

**Onarım notu (run14 → run15):** 2e.4 düzenlemeleri sırasında dosya araçları,
kaynaklardaki 26 Korece (EUC-KR) satırı UTF-8 U+FFFD'ye çevirmişti (ör.
`GMBattleCastle.cpp` `c->ID` sabitleri, `ZzzOpenData.cpp` ses/model adları,
`MapManager.cpp` yorumları). `git diff --cached` taramasıyla tespit edilip
`git show HEAD` baytlarıyla birebir geri konuldu (`BuildLog\2e4\fix_encoding.js`),
kaynak yeniden derlendi: md5 `b386bd65…` → `6dc52f5f…`. run14'ün davranış kanıtı
eski derlemeye aittir; **nihai çıktının kanıtı run15–17'dir**. KEN not: run14'ün
canlı sunucuya bağlanma girişimi t≈5 s'de, onarım sonrası koşularda t≈22 s'de
görüldü — girişim zamanlaması koşudan koşuya değişiyor (canlı 44405 o an yanıtsız;
`Test-NetConnection` False).

Kanıt dosyaları: `BuildLog\2e4\results\{run11-spkresolver.json,
run12-spkresolver2.json, run13-vipchar.json, run14-mappath.json,
run15-rebuild.json, run16-recheck.json, run17-tcp70.json}`,
`BuildLog\2e4\shots\` (13 PNG), `BuildLog\2e4\evidence\`
(`KEN_run14.txt`, `KEN_run15.txt`).

## 8. Sınırlar / kalan işler (dürüst durum)

1. **İçerik/şema eşlemesi (2e.4'ün ikinci yarısı):** `itemtooltip_Eng.bmd`,
   `itemleveltooltip_*`, `itemtooltiptext_*` canlı `Data\SPK\Config`'de **aynı adla
   yok**; tooltip içeriği `Text.bmd` / `ToolTipText.txt` / `MasterSkillTooltip.bmd`
   içinde farklı şemada. Yol katmanı hazır; okuyucu/şema işi açık.
2. `JewelOfHarmonySmelt*.bmd` canlı pakette hiçbir biçimde yok (UI canlıda kapalı
   olabilir).
3. Tam `Data\Map` ağacı (~562 MB) ve tam `Data` (~1,6 GB) disk sınırı nedeniyle
   paketlenemedi; testler alt kümelerle (World74/Object74 + görsel aileler) yapıldı.
4. `SPK_CRCFILE.ini` içindeki `SPK_MEXE` hâlâ `Engine.exe`'yi işaret ediyor
   (Main.exe pakete girerse rapor yeniden üretilmeli).
5. Sunucu tarafı testi bu adımın kapsamı dışındadır (2e.3 smoke'u ayrı).
6. **Açık gözlem (KEN model adı baytları):** `KEN_run14`'te bir model adı ham
   EUC-KR baytlarıyla (`B0 F8 BC BA C0 FC`) düşerken run15–17'de aynı alan
   `U+FFFD` dizisine dönüşmüş görünüyor (ad, modelin iç alanından okunuyor;
   diskte ham dizi hiçbir BMD'de bulunamadı). Yol/diyalog/çökme davranışı etkilenmiyor (tüm koşularda
   aynı asset yolları, aynı pencere/TCP sonucu); içerik/şema turunda (B-08)
   incelenecek.

## 9. Değişen dosyalar

- Yeni: `Source\5.Main\source\SPKAsset.cpp`; `Source\5.Main\Main.vcxproj` girdisi.
- Düzenlenen (5.Main): `SPKData.h`, `Sprite.cpp`, `CBInterfaceVIPChar.cpp`,
  `ZzzOpenData.cpp`, `QuestMng.cpp`, `CreditWin.cpp`, `ItemAddOptioninfo.cpp`,
  `ServerListManager.cpp`, `w_PetProcess.cpp`, `NewUISlideWindow.cpp`,
  `UIJewelHarmony.cpp`, `w_BuffScriptLoader.cpp`, `CSItemOption.cpp`,
  `MapManager.cpp`, `GMBattleCastle.cpp`, `GMCrywolf1st.cpp`,
  `GM_Kanturu_3rd.cpp`, `LoadData.cpp`, `ZzzTexture.cpp`, `NewUIMiniMap.cpp`.
- Araç: `BuildLog\2e2\deploy_spk_package.sh` (Map alt kümesi eklendi).
- Çıktı: `ClientFile\Main.exe` — **nihai md5 `6dc52f5fb63746cb51575ab1370f7e6c`**
  (12.029.952 B; onarım sonrası derleme). run14'ün derlemesi `b386bd65…` idi
  (bkz. §7 onarım notu).
- Ek: `Source\5.Main\source\Utilities\Log\` (ErrorReport + muConsoleDebug +
  WindowsConsole; vcxproj bunları zaten listeliyordu ama git'te izlenmiyordu —
  derleme bütünlüğü için commit'e alındı).

## 10. Yeniden üretim komutları

```bash
# derleme (Global Release|Win32)
MSBuild.exe Source/5.Main/Main.vcxproj -p:Configuration="Global Release" -p:Platform=Win32 -m

# paket (canlıdan DOKUNMADAN test klasörü) + saf SPK-first koşul
bash BuildLog/2e2/deploy_spk_package.sh --with-assets
rm -rf BuildLog/2e2/deploy/Data/Local            # SPK-first zorlaması

# istemci koşusu (diyalog/pencere/TCP/kalıntı gözlemi)
powershell -NoProfile -ExecutionPolicy Bypass -File BuildLog/2e2/run_client_test2.ps1 \
  -Label run17-tcp70 -ObserveSeconds 70 \
  -ClientDir "C:\Axion Mu Source\BuildLog\2e2\deploy" \
  -OutDir  "C:\Axion Mu Source\BuildLog\2e4\results" \
  -ShotDir "C:\Axion Mu Source\BuildLog\2e4\shots"
```
