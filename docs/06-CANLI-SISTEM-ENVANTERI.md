# 06 — CANLI SİSTEM ENVANTERİ (Faz 2a.2 / 2a.3 / 2a.4 çıktısı)

> Tarih: 30.09.2026. Canlı sistem **salt okunur** taranarak üretildi.
> Ham çıktılar `BuildLog\envanter\` altındadır (git dışı; yeniden üretim
> komutları bu belgede verilmiştir). Bu belge Faz 2b/2c iş emirlerinin
> temelidir.

---

## 1. CANLI DEPLOYMENT HARİTASI (kritik keşifler)

| Konum | Ne? | Hattımız için anlamı |
|-------|-----|----------------------|
| `C:\Axion Mu Mobile\4.MuServer\Sub-1\` | **Canlı SPK sunucusu** (GameServer.exe 6.979.072 B, 30.09.2026) | REFERANS/HEDEF |
| `...\Sub-1\GameServer\GameServer.map` | Canlı derlemenin **tam sembol haritası** (1,8 MB, 16.079 satır, 19.09.2026) | 2a.1 modül envanterinin birincil kaynağı; kopya: `BuildLog\envanter\GameServer_canli.map` |
| `C:\Axion Mu Mobile\Client and Tools\1Client\` | **Gerçek SPK istemcisi** (Engine.exe 9.201.152 B, 19.09.2026 + Launcher.exe + SPK.ini + Data\SPK) | İstemci paritesinin asıl hedefi |
| `C:\AxionMu\` | **Farklı fork** (Kas-Ara 2025; ChatServer, StartServer, EXDataServer, DB, MuEditor; GS'ler vcruntime140**d** = debug runtime) | PARİTE HEDEFİ DEĞİL — sadece altyapı fikri |
| `C:\Axion Mu Source\ClientBuild_192.168.99.200\` | MUIG-hat istemci paketi (Main.exe 12.003.328 B) | Bizim Main hattımızla aynı varyant; SPK değil |

### Kritik düzeltme (H-006)
Faz 1'de GS için "canlı referansla birebir" denmişti — **yanlış**: 10.689.536 B
eşleşmesi `MuServer\4.GameServer\...` içindeki 28.04.2026 tarihli ESKİ
referansla yapılmıştı. **Canlı GS 6.979.072 B** ve klasöründeki
msvcp100/msvcr100 DLL'lerinden anlaşıldığı üzere **v100 (VS2010) toolset'iyle**
derlenmiştir. Bizim v143 derlememizin boyut uyumu geçerli ama karşılığı
MuServer referans hattıdır, canlı değil.

---

## 2. FAZ 2a.2 — CANLI GAMESERVER SINIF ENVANTERİ

Yöntem: `GameServer_canli.map` içinden C++ sembolleri (`?Sınıf@...@@`)
çözümlendi → 242 benzersiz sınıf/namespace (std/ATL hariç). Her sınıf bizim
`Source\4.GameServer` (281 cpp + 283 h) ve MUIG donor
`.../Source/Source/GameServer` kaynaklarında grep ile arandı.

Yeniden üretim:
```bash
grep -oE "\?[A-Za-z_][A-Za-z0-9_]{1,30}@[A-Za-z_][A-Za-z0-9_]{1,30}@@" \
  BuildLog/envanter/GameServer_canli.map | sed 's/^?//; s/@@$//' \
  | awk -F'@' '{print $2}' | sort | uniq -c | sort -rn > BuildLog/envanter/map_siniflar.txt
grep -rFwf BuildLog/envanter/sinif_isimleri2.txt -oh --include="*.h" --include="*.cpp" \
  "/c/Axion Mu Source/Source/4.GameServer" | sort -u
```

### Sonuç özeti
| Ölçüm | Değer |
|-------|-------|
| Canlı GS sınıf sayısı (map) | **242** |
| Bizim GS kaynağında da var | **175** (%72) |
| Canlı-özel (bizde yok) | **67** |
| — içinden CRT/obfuscation artığı | 9 (`_00CNPNBAHC, _11LOCGONAA, __FrameHandler3, __crt_*×5, _expandlocale...`) |
| **Gerçek eksik modül** | **58** |
| — MUIG donor'da bulunan | **4** (`AddBuffer, CAUTOHP, CCustomJewelBank, CSkillDamage`) |
| — **Hiçbir kaynak setinde yok (sıfırdan)** | **54** |

> Not: Önceki oturumda tahmini "57 modül" denmişti; map tabanlı kesin sayı
> **58 eksik (54 sıfırdan)**. Eski analizdeki "PDB SPK\ alt klasörü 228 dosya"
> bilgisi kaynak dosya yollarından geliyordu; sınıf envanteriyle 2a.1'de
> birleştirilecek.

### 58 eksik modülün tam listesi (kategori ile)

**SPKViet özgü modüller (config adlarıyla birebir örtüşür):**
CThanMaChien, cTuLuyen, cQuanHam, cHonHoan, cDanhHieu, cRanking, cSkyEvent,
cSystemGuildUpgrade, cMessageNew, cCustomNameColor, cZenDrop, cCItemLevel,
CSystemMocNap, CReiDoMU, TaiSinh, ExWinQuestSystem, CustomHarmony

**Bot sistemi:** BotTradeMixCore, CBotMixSystem, ObjBotOnline

**Event sistemi:** EventMainManager, CActiveInvasions, CEventHideAndSeek,
CEventRunAndCatch, CEventKillAll, CastleStartGuild

**UI/Manager katmanı:** CCommandUI, CEffectManagerUI, CEventItemBagUI,
CEventItemBagManagerUI, CMapManagerUI, CShopManagerUI, CLogErrorForm

**Item/Damage özelleştirmeleri:** CustomDameItem, CustomSetDameItem,
CCustomStartItemDame, CCustomStartSetItemDame, CItemExOptionRate, CItemSubMix,
CustomNewBuff, CheckItemVip, ItemPassLocker, SystemItemChanger, MoveOptionNew

**Karakter/hesap özellikleri:** CCustomCharOption, CCustomChangeClass,
CCustomRenameChar, CCustomLuckySpin, CCustomGreatPK, CPartySetPass,
CResetChange, ResetLitmitLock, CSGetInfoCharacter, ViewItemPlayer

**Ekonomi/shop:** CExtendShop, NewCashShop, VPDameBoss

**Diğer:** CSocketMaker, SPK_ToolKitMain

### Canlı GS toolset kanıtı
- Canlı klasör: `msvcp100.dll + msvcr100.dll` (+ msvcr120d.dll artığı) → **v100**.
- Bizim MuServer referansı (10.689.536 B) ve bizim derlememiz: v143 statik CRT.
- Sonuç: Canlıyı hedeflerken boyut paritesi BEKLENMEZ; işlevsel parite esastır.

---

## 3. FAZ 2a.3 — CANLI MUSERVER CONFIG ENVANTERİ

### Hacim
- `Sub-1\Data\`: **450 dosya, ~10 MB** (bizim `MuServer\4.GameServer\Data`:
  437 dosya; yol farkı: 175 sadece canlıda, 162 sadece bizde).
- `Sub-1\GameServer\SPK\`: **225 dosya** — `SPK_ToolKitMain` modülünün
  **canlı logları** (2026-09-20 … 2026-09-30 + LOG_CASH_SHOP) → modülün
  canlıda aktif olduğunun doğrudan kanıtı.

### `Sub-1\Data\` klasör dökümü
| Klasör | Dosya sayısı | Not |
|--------|--------------|-----|
| (kök) | 8 | BlackList, Command, Effect, EventItemBagManager, MapManager, MapServerInfo.ini, Message.xml, ShopManager |
| EventItemBag | 187 | Event drop torbaları |
| Terrain | 84 | Harita attribute |
| Custom | 26 | BotSystem/, WinQuest/ + Custom*.txt |
| **SPK** | **25** | **SPK modül config'leri** (aşağıda) |
| Shop | 26 | |
| Item | 20 | |
| Event | 24 | |
| Monster | 16 | |
| Hack | 5 | HackPacketCheck/HackSkillCheck/MHPServer.ini + Enc/Dec dat |
| Util | 8 | ExperienceTable, Filter, FilterRename, GameMaster.xml, Notice, RESET.ini, ResetTable, Skill |
| CashShop 2, Character 1, Move 3, Quest 3, QuestWorld 9, Skill 4 | | |

### `Data\SPK\` (25 dosya) — SPK modül config'leri
AddBuff.txt, ChangeClass.xml, ChangeItem.xml, CharOption.xml, CustomBuyVip.txt,
CustomCommandSocket.xml, CustomCongHuong.xml, CustomHarmony.xml,
CustomItemPro.xml, CustomItemSetPro.xml, CustomJewelBank.xml, CustomMocNap.xml,
CustomMonsterSkill.txt, CustomNameColor.ini, CustomShop.xml, CustomVongQuay.xml,
DanhHieu.xml, DungLuyen.xml, ExtendShop.xml, GuildUpgrade.txt, HonHoan.xml,
QuanHam.xml, Relife.xml, ResetChange.txt, TuLuyen.xml

### Config tanınırlık tablosu (bizim GS / MUIG GS kaynak taraması)

**A) Bizim GS kaynağımız zaten tanıyor (22):**
BotTrader.xml, ChangeItem.xml, CustomBuyVip.txt, CustomCombo.txt,
CustomCongHuong.xml, CustomDeathMessage.txt, CustomJewel.txt, CustomMix.txt,
CustomMocNap.xml, CustomMonster.txt, CustomMonsterSkill.txt, CustomMove.txt,
CustomNpcQuest.txt, CustomPet.txt, CustomPick.txt, CustomQuest.txt,
CustomStartItem.txt, CustomTop.txt, CustomVongQuay.xml, CustomWing.txt,
CustomWingMix.txt, ZenDrop.xml

**B) Bizde yok ama MUIG donor GS'de var (4):**
AddBuff.txt (AddBuffer), CongHuong.txt, CustomJewelBank.xml (CCustomJewelBank),
ThuMuaDoExc.txt (BotThuMua)

**C) Hiçbir kaynakta tanınmıyor — SPK-özel, sıfırdan yazım config'leri (24):**
BotOnline.txt, BotTradeMix.txt, ChangeClass.xml, CharOption.xml,
CreationQuestData_1.ini, CreationQuestSystem.xml, CustomCommandSocket.xml,
CustomHarmony.xml, CustomItemPro.xml, CustomItemSetPro.xml,
CustomNameColor.ini, CustomNewBuff.xml, CustomShop.xml,
CustomStartItemDame.txt, CustomStartSetItemDame.txt, DanhHieu.xml,
DungLuyen.xml, ExtendShop.xml, GuildUpgrade.txt, HonHoan.xml, QuanHam.xml,
Relife.xml, ResetChange.txt, TuLuyen.xml

> Bu tablo Faz 2c önceliklendirmesinin iş iskeletidir: her C grubu config'i,
> 2. bölümdeki sınıfla eşleşir (ör. TuLuyen.xml ↔ cTuLuyen,
> CustomHarmony.xml ↔ CustomHarmony, Relife.xml ↔ TaiSinh/Relife sistemi).

---

## 4. FAZ 2a.4 — CANLI İSTEMCİ ENVANTERİ

### Gerçek SPK istemcisi: `Client and Tools\1Client\`
| Öğe | Değer |
|-----|-------|
| Motor | **Engine.exe** — 9.201.152 B, **19.09.2026** (canlı GS.map tarihiyle aynı gün) |
| Launcher | Launcher.exe — 1.770.496 B, 16.08.2026 |
| Konfig | **SPK.ini** (`MainCode = 1.03.34`, Font/Audio/SPK bölümleri) |
| Bağımlılıklar | msvcp100/msvcr100 (+ debug varyantları), glew32, ogg, vorbisfile, wzAudio, ntdll.dll (lokal), GameVoice.dll, OpenGL32.dll (lokal override), iU.spk |
| Veri | `Data\SPK\`: `Btn_AutoHp.spk` (**AUTOHP istemci UI varlığı**), `Btn_AutoPK.spk`, `Btn_A_Inv.spk`, `ai_newui_skill*.ozj`, `Config\*.bmd` (Item/Gate/Mix/Skill/Dialog/BuffEffect/Credit/Filter…), `Config\Info\` |

**Engine.exe string kanıtları:** `ConnectIP.bmd: 1`, `ServerData.bmd: 1`,
`Data\SPK: 36`, `MuSPK: 1` (PDB yolu izi) — istemci, sunucuyla aynı
SPK/GetEngine veri formatını konuşuyor.

### Hat ayrımı (hangi istemci hangi sunucuyla?)
| İstemci | Hat | Kanıt |
|---------|-----|-------|
| `1Client\Engine.exe` | **SPK** | ConnectIP.bmd/ServerData.bmd/Data\SPK/SPK.ini/MuSPK izi |
| `ClientBuild_192.168.99.200\Main.exe` | MUIG | CBGetMain.bin + License.json hattı |
| `C:\AxionMu\Main.exe` (32 MB, 27.09.2026) | 3. hat (paketli/string görünmez) | ASCII+UTF-16 taramada hiçbir bilinen iz yok |
| **Bizim `ClientFile\Main.exe`** | MUIG | CBGetMain + License.json (kaynağımızda SPK izi: 0 dosya) |

### İstemci tarafı eksik (bizim 5.Main'de yok)
- `ConnectIP.bmd` / `ServerData.bmd` okuma (SPK sunucu listesi formatı)
- `Data\SPK\` asset/config erişimi (`Config\*.bmd`, `Btn_*.spk`)
- `SPK.ini` okuma (MainCode, font, çözünürlük)
- AUTOHP / AutoPK istemci UI modülleri
- "Engine.exe" mimarisi (bizimkisi Main.exe; isimlendirme/deployment farkı)

> **Karar kutusu (Faz 2e öncesi netleşmeli):** SPK sunucu paritesi
> hedefleniyorsa istemcinin de SPK veri formatını konuşması gerekir. İki yol:
> (a) 5.Main'e SPK okuma katmanı eklemek (ConnectIP.bmd/ServerData.bmd
> format çözümlemesi 1Client referansından yapılır), (b) mevcut MUIG-hat
> istemciyle SPK sunucu arasına protokol köprüsü. Önerilen: (a).

---

## 5. SONUÇLAR → SONRAKİ ADIMLARA ETKİSİ

1. **2a.1 artık çok daha kolay:** canlı `GameServer.map` elimizde; 05-SPK-MODUL-ENVANTERI.md
   bu map + PDB kaynak yolları + config tablosunun birleşimiyle yazılacak.
2. **Faz 2c iş yükü netleşti:** 54 sıfırdan + 4 MUIG transfer modülü (GS tarafı);
   bunlara karşılık 24 SPK-özel config formatı da üretilecek.
3. **Faz 2b kapsamı doğrulandı:** MUIG donor'un 4 eksik modülü (AddBuffer,
   CAUTOHP, CCustomJewelBank, CSkillDamage) 2b ile gelir; 161 ortak dosya
   güncellemesi ayrı iş kalemi olarak sürer.
4. **İstemci tarafı parite Faz 2e.0 ile açılıyor:** SPK istemci formatları
   (ConnectIP.bmd, ServerData.bmd, Data\SPK, SPK.ini) 5.Main'e öğretilecek;
   referans binary: `1Client\Engine.exe`.
5. **H-006:** Faz 1 GS "birebir" iddiası düzeltildi — MuServer referans hattı
   ile eşleşme, canlı GS (v100, 6.979.072 B) ile DEĞİL.
