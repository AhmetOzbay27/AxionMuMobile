# 03 — EKSİK İÇERİK VE ENTEGRASYON LİSTESİ

> Canlı SPK'da VAR, bizim kaynakta EKSİK olan her şey + nereden entegre
> edileceği. Her satır: kaynak → hedef → durum. Tamamlananlar ✅ ile
> işaretlenir ve CHANGELOG'a kaydedilir.

Durum kodları: ⬜ eksik · 🔄 işlemde · ✅ entegre · ❓ araştırılacak

---

## A. SUNUCU (GameServer) — 58 eksik modül (map tabanlı kesin sayı)

> Kaynak kanıtı: canlı `GameServer.map` sembol analizi (bkz.
> [06-CANLI-SISTEM-ENVANTERI.md](06-CANLI-SISTEM-ENVANTERI.md)). Ham listeler:
> `BuildLog\envanter\canlida_bizde_yok.txt`, `muigden_alinabilir.txt`,
> `sifirdan_yazilacak.txt`.

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| A-01 | 4 MUIG donor modülü: AddBuffer, CAUTOHP, CCustomJewelBank, CSkillDamage | MUIG donor GS | `Source\4.GameServer\` | ✅ Faz 2b.0 (30.09.2026) — derlendi + PDB doğrulandı |
| A-02 | 54 SPK-özel modül — tam liste 06 envanter §2: SPKViet (CThanMaChien, cTuLuyen, cQuanHam, cHonHoan, cDanhHieu, cRanking, cSkyEvent, cSystemGuildUpgrade, cMessageNew, cCustomNameColor, cZenDrop, cCItemLevel, CSystemMocNap, CReiDoMU, TaiSinh, ExWinQuestSystem, CustomHarmony) · Bot (BotTradeMixCore, CBotMixSystem, ObjBotOnline) · Event (EventMainManager, CActiveInvasions, CEventHideAndSeek, CEventRunAndCatch, CEventKillAll, CastleStartGuild) · UI (CCommandUI, CEffectManagerUI, CEventItemBagUI, CEventItemBagManagerUI, CMapManagerUI, CShopManagerUI, CLogErrorForm) · Item/Dmg (CustomDameItem, CustomSetDameItem, CCustomStartItemDame, CCustomStartSetItemDame, CItemExOptionRate, CItemSubMix, CustomNewBuff, CheckItemVip, ItemPassLocker, SystemItemChanger, MoveOptionNew) · Karakter (CCustomCharOption, CCustomChangeClass, CCustomRenameChar, CCustomLuckySpin, CCustomGreatPK, CPartySetPass, CResetChange, ResetLitmitLock, CSGetInfoCharacter, ViewItemPlayer) · Ekonomi (CExtendShop, NewCashShop, VPDameBoss) · Diğer (CSocketMaker, SPK_ToolKitMain) | ❓ yok — sıfırdan (map + PDB + canlı config analizi) | `Source\4.GameServer\` | ⬜ Faz 2c (öncelikler 05 dokümanında hazır) |
| A-03 | MuServer config seti (canlı 450 dosya; 24 SPK-özel config dahil) | Canlı `Sub-1\Data\` (SALT OKUNUR kopya) | `MuServer\...\Data\` | ⬜ modül modül 2c ile |

### A-03 alt kırılım — Data\SPK config tanınırlığı
- **Bizim GS zaten tanıyor (22):** BotTrader.xml, ChangeItem.xml, CustomBuyVip.txt, CustomCombo.txt, CustomCongHuong.xml, CustomDeathMessage.txt, CustomJewel.txt, CustomMix.txt, CustomMocNap.xml, CustomMonster.txt, CustomMonsterSkill.txt, CustomMove.txt, CustomNpcQuest.txt, CustomPet.txt, CustomPick.txt, CustomQuest.txt, CustomStartItem.txt, CustomTop.txt, CustomVongQuay.xml, CustomWing.txt, CustomWingMix.txt, ZenDrop.xml
- **MUIG donor'da var (4):** AddBuff.txt (AddBuffer), CongHuong.txt, CustomJewelBank.xml (CCustomJewelBank), ThuMuaDoExc.txt (BotThuMua)
- **Hiçbir kaynakta yok (24 — sıfırdan modül config'leri):** BotOnline.txt, BotTradeMix.txt, ChangeClass.xml, CharOption.xml, CreationQuestData_1.ini, CreationQuestSystem.xml, CustomCommandSocket.xml, CustomHarmony.xml, CustomItemPro.xml, CustomItemSetPro.xml, CustomNameColor.ini, CustomNewBuff.xml, CustomShop.xml, CustomStartItemDame.txt, CustomStartSetItemDame.txt, DanhHieu.xml, DungLuyen.xml, ExtendShop.xml, GuildUpgrade.txt, HonHoan.xml, QuanHam.xml, Relife.xml, ResetChange.txt, TuLuyen.xml

## B. İSTEMCİ (Main)

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| B-01 | **SPK istemci format katmanı**: ConnectIP.bmd, ServerData.bmd, SPK.ini, Data\SPK okuma | `Client\Engine.exe` + `Data\SPK\Config\*.bmd` analizi (yeni adım **2d.0**) | `Source\5.Main\` | ✅ 2d.0 (02.10.2026; tablo docs/19 — Engine.exe kabul testi 2d.2'de) |
| B-02 | SPK client içerik varlıkları (Btn_AutoHp.spk, Btn_AutoPK.spk, ai_newui_skill*.ozj, Config\Info\) | `Client\Data\SPK\` | istemci paketi | ⬜ 2d.0 sonrası |
| B-03 | GetMainInfo varyant birleşimi — **tasarım tamam** ([08 dokümanı](08-GETMAININFO-TASARIM.md), D1-D9 iş planı) | SPK GetEngine benimsendi; MUIG legacy bayraklı | `Source\6.GetMainInfo\GetMainInfo\SPK\` | 🔄 2d.1 (1/2): D1-D3/D6/D7 ✅ canlı araçla bayt-birebir (docs/20); D4/D5 tam jeneratör kaldı |
| B-04 | Gömülü IP / config okuma düzeni (H-004) | Canlı değer: **45.87.120.29:44405** (ConnectIP.bmd; `192.168.0.150` varsayımı yanlıştı) | `Source\5.Main\` | ✅ 2e.1 (02.10.2026): gömülü değer hizalandı + ConnectIP 0x20/0x22 çalışma anında uygulanıyor (docs/22 §2) |
| B-06 | **Paket/dağıtım içeriği:** DLL kapanışı (`wzAudio.dll`→`ogg.dll`+`vorbisfile.dll`), APICB/FreeImage importlarının kaldırılması, `Data\SPK\Config` kurulumu | Canlı `Client\` kökü + `Data\SPK\` | istemci paketi | ✅ 2e.2 (02.10.2026): `BuildLog\2e2\deploy_spk_package.sh`; istemci pakette pencere açtı (docs/22 §3) |
| B-07 | **SPK-first varlık çözümleme katmanı** — istemcinin `Data\Local\*` sabitleri ↔ paketin `Data\SPK\Config\*` tabloları (12 tablo; dil ekli `_<Lang>` kalıbı; `JewelOfHarmonySmelt` canlıda hiç yok) | Kanlı paket (`Data\SPK\Config`) + audit `BuildLog\2e2\asset_audit.txt` | `Source\5.Main\` | ✅ 2e.4 (02.10.2026): `SPKAsset.cpp` (`SPK_ResolveAssetPath`/`SPK_AssetExists`) 20+ çağrı noktasına bağlandı; deploy betiğindeki yol eşlemesi artık yalnız yedek (docs/24) |
| B-08 | **SPK içerik/şema eşlemesi (2e.4'ün ikinci yarısı):** `itemtooltip_<Lang>`/`itemleveltooltip`/`itemtooltiptext` (canlı `SPK\Config`'de aynı adla yok), `Text*`/`ToolTipText.txt`/`MasterSkillTooltip.bmd` şemaları, Minimap içeriği (canlıda yalnız belirli world'ler; World74 yok) | canlı `Data\SPK\Config` + `Data\SPK\Minimap` | `Source\5.Main\` | ✅ **2e.5 (03.10.2026)** — `ToolTipText.txt` → istemci tooltip tablosu yükleyicisi yazıldı ve runtime kanıtlandı (KEN.txt: `[SPK] ToolTipText: 19 kayit`; exe `c49383bf…`). Text/MasterSkillTooltip katmanları çözümleyici üzerinden mevcut. **Açık kalan:** item başına tooltip *bileşim* tablosu (canlı pakette binary şema yok) → docs/25 §7 B-08b |
| B-05 | MUIG 68 özel modülünün canlı karşılığı kontrolü — **TAMAMLANDI**: 29 modül 6 kanıt hattıyla sınıflandırıldı (7 parite-tamam / 3 iş-kalemi 2c'ye / 19 canlıda-yok OFF); rapor [12](12-MUIG68-CAPRAZ-KONTROL.md), ham veri BuildLog\envanter\muig68_* | MUIG donor | — | ✅ 2b.3 (01.10.2026) |

## C. ORTAK / ALTYAPI

| # | Kalem | Kaynak | Hedef | Durum |
|---|-------|--------|-------|-------|
| C-01 | MUIG'in 161 daha yeni ortak dosyası | MUIG donor | `Source\` (SPK taban) | ✅ 2b.1 matrisi + 2b.2 7 dalga (66 alındı / 147 korundu) |
| C-01b | **YENİ (2b.3 keşfi): EventGvG modülü** — canlı ServerInfo 7 config anahtarı kanıtlı (EventGvGSwitch/Npc/NpcMap/NpcX/NpcY/MinUsers/MaxUsers), bizim ServerInfo.cpp'te yok | canlı kanıttan sıfırdan | `Source\4.GameServer\` | ⬜ 2c (05 listesine 60. kalem) |
| C-02 | DB şeması (DB_SQL_12.bak) ile GS beklentileri uyumu | `ServerTools\DB_SQL_12.bak` | test DB | 🟡 2e.3 (02.10.2026): yerel `MuOnlineS6`'ya restore edildi (55 tablo); tam şema uyumu kontrolü Faz 3.1'de |

## D. BİLİNEN KÜÇÜK SAPMALAR (parite, düşük risk)

| # | Kalem | Detay | Durum |
|---|-------|-------|-------|
| D-01 | String sapmaları (Main) | `FCBad item index.` vs `Bad item index.` gibi ön-ek farkları — revizyon kaçağı; 2b'de donor revizyonu alınca çoğu kapanır | ⬜ |
| D-02 | `7 SO BAO MAT` stringi | Yalnız bizim derlemede (MUIG VN kaynak izi); canlıda yok | ⬜ |

---

### KAYNAK KONUMLARI (hızlı erişim)
- MUIG donor: `C:\Axion Mu Mobile\New Source Code\Source\Source\` (+Main5.2, +Encoder)
- SPK GetEngine referans binary: `C:\Axion Mu Mobile\Client and Tools\GetMain\`
- **SPK istemcisi (gerçek):** `C:\Axion Mu Mobile\Client and Tools\Client\` (Engine.exe + SPK.ini + Data\SPK; eski adı Client (eski adı 1Client))
- Canlı sunucu (salt okunur): `C:\Axion Mu Mobile\4.MuServer\Sub-1\` (GameServer.map dahil)
- Farklı fork (parite hedefi DEĞİL): `C:\AxionMu\`
- Eski analiz raporları: `C:\Axion Mu Mobile\analiz\`

---

## ✅ 2a.5 KARARI (30.09.2026) — TEK HAT: SPK GETENGINE

**Kanıtlar:**
- `GetMain\GetEngine.ini` (UTF-8): IpAddress=45.87.120.29, IpAddressPort=44405,
  ClientVersion=1.03.34 (= SPK.ini MainCode), ClientName=Engine.exe,
  WindowName=Axion Mu, AntPort=58584, MaxAttackSpeed*=67000, DefaultFPS=24,
  MENUBUTTON_* Vietnamca menü etiketleri (Relife/Reset/DanhHieu/Spin/MocNap/
  HonHoan/X-Shop...), JewelBankTab=2 — **Türkçe yorumlu** (Axion'un kendi konfig'i).
- `Data\SPK\ConnectIP.bmd` (36 B): `XOR 0x20("45.87.120.29") + 0x20 pad + CRC4`
  → GetEngine.ini IP ile birebir; GetMainInfo'nun ürettiği format.
- `Data\SPK\ServerData.bmd` (1.089.576 B): aynı XOR/pad tekniği + veri blokları.
- `GetMain\SPK_CRCFILE.ini`: GetMainInfo'nun bütünlük raporu ("Code by SuperHung";
  SPK_*.bmd CRC'leri + FOUND/NOT FOUND listesi) → araç doğrulayıcı da.

**KARAR: A varyantı — SPK GetEngine hattı benimsenir.**
1. `Source\6.GetMainInfo` → GetEngine davranışına geliştirilecek: GetEngine.ini
   oku → ConnectIP.bmd + ServerData.bmd üret (XOR 0x20 + CRC4) → SPK_CRCFILE.ini
   raporu. Mevcut MUIG Custom* veri işleme görevi KORUNUR (GetMain\Data\ altı).
2. `Source\5.Main` → SPK format okuma katmanı (adım **2d.0**): ConnectIP.bmd
   (XOR 0x20) + ServerData.bmd + SPK.ini; Main.exe Engine.exe deployment'ına
   paralel (ClientName config'ten okunur).
3. MUIG MainInfo/CBGetMain hattı → kod korunur ama bayrakla devre dışı
   (OfflineMode çatallanma mantığı gibi); v2'de gerekiyse yeniden açılır.
4. H-005: karar alındı → uygulama Faz 2d.0/2d.1'de; kapanış orada yapılır.

**GetEngine.ini'den gelen ek istemci kanıtları (B-01 kapsamı):**
ClientVersion zinciri (1.03.34), AntiPort 58584, MaxAttackSpeed 67000 seti,
DefaultFPS, MENUBUTTON_ UI dizisi (SPK modüllerin istemci karşılıkları),
JewelBankTab, Rank from FSP, MaxLevel (DanhHieu 20 / Synham 21 / Tuchanh 32 /
Honhoan 50) → 2d.0'da SPK.ini/GetEngine.ini okuma ile Main'e taşınacak.
