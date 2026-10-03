# 18 — SPK_CustomItemSetPro (A3) CANLI KANITTAN UYGULAMA

Tarih: 2026-10-02 (12:10–12:55 oturumu)
Kapsam isteği: "2c.2..N — Modül modül: iskelet yaz → GS derle → config dosyasını üret."
(docs/13 satır 35 `| A3 | 37 | CustomItemSetPro | CustomSetDameItem | CustomItemSetPro.xml | sıfırdan | set bonus hasarı |`)

> Kural: docs/12 §4 — parite kararlarında **canlı kanıt donor'un önünde** (bu modülde zaten donor yok).
> Her iddianın yanında canlı adres / exe offset / dosya kanıtı vardır.

---

## 1. Sonuç özeti

| # | Kalem | Sonuç |
|---|-------|-------|
| 1 | Modül dosyası | `Source\4.GameServer\GameServer\SPK\CustomItemSetPro.{cpp,h}` (canlı obje adı `CustomItemSetPro.obj`) |
| 2 | Sınıf | `CustomSetDameItem` + `gCustomSetDameItem` global + `Instance()` (canlı 0x4765D0) |
| 3 | Depolama | `std::vector<ConfigSetDataDamage>` × 2 (canlı this+0 `Item`, this+0Ch `ItemSet`; eleman 0x58 = 22 int) |
| 4 | Config | `Data\SPK\CustomItemSetPro.xml` — canlıdan **bayt-birebir** (50858 B, md5 `9b2d6ef99036b063b105ada65f2296e8`, 92 Item + 84 ItemSet) |
| 5 | Yükleme | `CServerInfo::ReadCustomInfo` → `gCustomSetDameItem.Load(...)` (canlı zincir 0x569B78/0x569BAE) |
| 6 | Oyun kancası | `CObjectManager::CharacterCalcAttribute` karakter zinciri — `CalcMasterSkillTreeOption` sonrası (canlı 0x5424D5 sırası) |
| 7 | Derleme | `Release_EX603` · v143 · Win32 → **0 hata**; exe 10.819.072 → **10.824.704 B** (+5.632 B), md5 `43f086d939575ac34eb373f7d527b8e9` |
| 8 | Sembol kanıtı | Bizim disasm: `?Load@CustomSetDameItem@@QAEXPAD@Z` @0x465F00, `?CalcSlot@...@AAEXPAUOBJECTSTRUCT@@PAUConfigSetDataDamage@@@Z` @0x4FBDF0, `gCustomSetDameItem` ön-belirteç/atexit kayıtları |

**Bekleyen:** E2E (istatistik uygulama testi) + SPK GUI editörü (kapsam dışı kararı) + parite farkı yok iddiasının E2E ile teyidi.

---

## 2. Canlı kanıt (CustomItemSetPro.obj)

Kaynaklar: `BuildLog\envanter\live_disasm.txt`, `BuildLog\envanter\GameServer_canli.map`,
canlı `Data\SPK\CustomItemSetPro.xml`. Arşiv: `BuildLog\2c1\A3_*.txt`.

### 2.1 Sınıfın yüzeyi (canlı map)

```
?Instance@CustomSetDameItem@@SAPAV1@XZ                       004765D0  CustomItemSetPro.obj
?Load@CustomSetDameItem@@QAEXPAD@Z                           00476640
?Save@CustomSetDameItem@@QAEXPAD@Z                           00477360
?CalcCharacter@CustomSetDameItem@@QAEXPAUOBJECTSTRUCT@@_N@Z  00478BF0
?SPK_CustomItemSetProProc@@YGHPAUHWND__@@IIJ@Z               004796A0  (SPK GUI editörü — kapsam dışı)
?_Xlength@?$vector@UConfigSetDataDamage@@...                 0x9A00    (vector<ConfigSetDataDamage>)
```

Canlı yükleme zinciri (ReadCustomInfo): `CustomHarmony → SystemItemChanger(CustomItemPro) →
MoveOptionNew(CustomCongHuong) → **CustomSetDameItem(CustomItemSetPro.xml)** → CCustomBuyVip →
CustomDameItem` — yol literali `SPK\CustomItemSetPro.xml` @0x63C968.
Karakter zinciri (tek çağrı noktası 0x5424D5, `push 0`): `CalcSocketItemOption →
CalcMasterSkillTreeOption → **CustomSetDameItem::CalcCharacter** → CalcCustomPetOption →
CCustomStartItemDame → CCustomStartSetItemDame → DanhHieu/QuanHam/TuLuyen/HonHoan → ...`

### 2.2 Load @0x476640 (iki döngü, pugixml)

- İki vektör de başta `clear()` edilir (0x476683/0x476699: end=begin).
- Hata yolu: parse başarısızsa `ErrorMessageBox("File %s load fail. Error: %s", ...)` + `int 3` (string @0x637F34).
- `child("Item")` + `next_sibling("Item")` → **22 attribute**: Section, Index, Effect, Lv, Opt, Dmg,
  AddSD, AddHP, AddMP, Def, ExlDmgRate, CriDmgRate, DoubleDmgRate, ResistDouble, ResistIgnoreDef,
  ResistIgnoreSD, ResistCrit, ResistExl, ResistStun, Reflect, Time, SetOption → `m_vItemData`.
- `child("ItemSet")` + `next_sibling("ItemSet")` → **Section okunmaz** (0x476D8B'de alan açıkça 0'lanır),
  kalan 21 alan okunur → `m_vItemSetData`.
- Attribute okuma canlıda `strtol(value,NULL,0)` semantiği; bizdeki `.as_int(0)` (A1 deseni) bu dosyadaki
  tüm değerlerde birebir aynı sonucu verir (yalnız ondalık değerler var).

### 2.3 Save @0x477360

`ItemData` kökü: `Item[]` (22 attribute, Section dahil) + `ItemSet[]` (21 attribute, Section'sız);
canlı `fopen(path,"wb")` + `xml_document::save(xml_writer_file)` — pugixml varsayılan biçimi
(`\t` girinti, `<?xml version="1.0"?>`), canlı dosya biçimiyle aynı. Bizde `file.save_file(path)`.

### 2.4 CalcCharacter @0x478BF0 + lambda @0x478C90

- `flag != 0` → **erken dönüş** (0x478BF6: `cmp byte [ebp+0Ch],0` / `jne` epilog). Canlıda tek çağrı
  noktası `push 0` — uncalc yolu no-op.
- 1. döngü: her `Item` satırı kendi Section'ıyla uygulanır.
- 2. döngü: her `ItemSet` satırı için kopya alınır, **ilk alan (Section) 7..11 yapılır** ve 5 kez
  uygulanır (0x478C30-0x478C80: `edi=7`, `mov [ebp-60h],edi`, `inc edi`, `cmp edi,0Bh`, `jle`).
  Yani ItemSet = "helm/armor/pants/gloves/boots (section 7..11) için aynı Index'e uygulanan set bonusu".
  Config'teki `Effect=205` + `Time=1800` + `SetOption=5` satırları bu genişletmeye girer; Effect/Time
  CalcCharacter'da kullanılmaz (editör/görünüm alanları).
- Lambda (bizim `CalcSlot`): envanter wear taraması **slot 2..11** — canlı offset `1F8h..0AD4h`, adım `0FCh`
  (= CItem boyu). Taban `[obj+0x3A8]` (inventar işaretçisi) doğrulaması: `C380ItemOption::Calc380ItemOption`
  @0x404A21 aynı tabanı `add ebx,0B8h` + 12 yuvaya `add ebx,0FCh` ile tarar (aynı obje şeması).
  Yani tarama **weapon (0) ve shield (1) slotlarını kapsamaz**; config'teki 16 `Section="6"` (kalkan)
  satırı canlıda da eşleşmez — bu davranış parite gereği birebir korundu.
- Karşılaştırma sırası (hepsi geçmeli): `m_Index == Section*512+Index` (canlı `shl 9`) →
  `m_Level >= Lv` (canlı `[item+6]`) → `m_NewOption >= Opt` (canlı byte `+9F`) →
  `m_SetOption == SetOption` (canlı byte `+0B6`).
- Eşleşen **her yuva** için (aynı satır birden fazla item'a, birden fazla satır aynı item'a uygulanabilir):
  `Dmg` altı hasar alanına (Physi/Magic/Curse Min+Max) ve `AddSD→AddShield, AddHP→AddLife,
  AddMP→AddMana, Def→Defense, ExlDmgRate→ExcellentDamageRate, CriDmgRate→CriticalDamageRate,
  DoubleDmgRate→DoubleDamageRate, ResistDouble→ResistDoubleDamageRate,
  ResistIgnoreDef→ResistIgnoreDefenseRate, ResistIgnoreSD→ResistIgnoreShieldGaugeRate,
  ResistCrit→ResistCriticalDamageRate, ResistExl→ResistExcellentDamageRate, ResistStun→ResistStunRate,
  Reflect→DamageReflect` alanlarına EKLENİR. (Canlı hedef offsetleri 2A8..2E0/134/124/128/31C/8CC..900/988
  — bizim OBJECTSTRUCT alanlarıyla 1:1 semantik eşleme.)

### 2.5 Alan eşleme çıkarımları (E2E ile teyit edilecek)

| Config alanı | Canlı item byte | Bizim alan | Gerekçe |
|---|---|---|---|
| `Opt` (≥, tüm satırlar 63) | +9F | `m_NewOption` | 63 = 6/6 excellent bitmask üst sınırı; canlı `XuLyItemTrade` @0x41898C-0x41899D aynı byte'ı "min" eşiği olarak kullanır |
| `SetOption` (==, 0 ve 5) | +0B6 | `m_SetOption` | canlı `C380ItemOption` @0x404A35 item +0B8'i (380 alanı; bizim `m_ItemOptionEx`) okur — +0B6 bu alanın 2 bayt öncesi |

---

## 3. Bizim uygulama

- `SPK/CustomItemSetPro.h/.cpp`: `ConfigSetDataDamage` (22 int, canlı alan sırası), `CustomSetDameItem`
  (`Instance`, `Load`, `Save`, `CalcCharacter`, private `CalcSlot`), global `gCustomSetDameItem`.
- `GameServer.vcxproj`: `ClInclude SPK\CustomItemSetPro.h` + `ClCompile SPK\CustomItemSetPro.cpp`.
- `ServerInfo.cpp`: `#include` + `ReadCustomInfo` içinde `gCustomSetDameItem.Load(gPath.GetFullPath("SPK\\CustomItemSetPro.xml"))`
  (canlı yol birebir; canlı 0x569B78/0x569BAE).
- `ObjectManager.cpp`: `#include` + `CharacterCalcAttribute` zincirinde `gMasterSkillTree.CalcMasterSkillTreeOption`
  sonrası `gCustomSetDameItem.CalcCharacter(lpObj, 0)` (canlı 0x5424D5 sırası).
- Kapsam dışı: SPK GUI editörü (`SPK_CustomItemSetProProc`) — dosya yazma biçimi Save'de korundu;
  bizde Save referanssız (linker COMDAT eleme ile exe'den düşebilir; canlıda editör çağırıyor).

### 3.1 Derleme + sembol kanıtı

```
CustomItemSetPro.cpp + ObjectManager.cpp + ServerInfo.cpp derlendi
577 of 19019 functions ( 3.0%) were compiled, the rest were copied ... 54 functions were new
GameServer.vcxproj -> ...\MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe
```

Bizim disasm (`BuildLog\2c1\A3_bizim_disasm.txt`, 17.1 MB):

```
1328:    dynamic initializer for 'gCustomSetDameItem'
119266:  ?Load@CustomSetDameItem@@QAEXPAD@Z:            (0x465F00; komsu disasm 0x465F41/0x466607 gCustomSetDameItem)
343649:  0x525BA0: call ?Load@CustomSetDameItem@@QAEXPAD@Z   (= ServerInfo ReadCustomInfo)
293117:  ?CharacterCalcAttribute@CObjectManager@@QAEXH@Z
295351:  0x4FBE14: call ?CalcSlot@...  (Item dongusu; 0x4FBDE0 mov esi,[gCustomSetDameItem])
295400:  0x4FBEE3: call ?CalcSlot@...  (ItemSet dongusu; 0x4FBE80 mov ecx,7 / 0x4FBEF7 cmp ecx,0Bh)
```

`CalcCharacter` gövdesi CharacterCalcAttribute'a inline edildi (tek çağrı, sabit flag=0) — davranış birebir;
canlıda da zincir içindeki çağrı 0x5424D5'dir. `Save` referanssız olduğu için exe'de sembol çıkmaz (kapsam dışı UI).

---

## 4. Config üretimi

```
cp "/c/Axion Mu Mobile/4.MuServer/Sub-1/Data/SPK/CustomItemSetPro.xml" \
   "MuServer/4.GameServer/Sub 1/Data/SPK/CustomItemSetPro.xml"
md5: 9b2d6ef99036b063b105ada65f2296e8 (kaynak ve hedef aynı)
92 Item (Section 12:24, 13:52, 6:16) + 84 ItemSet satırı; Opt=63 tüm satırlarda; SetOption 0/5
```

---

## 5. Bilinen sınırlar / sonraki adımlar

1. **E2E yapılmadı** — istatistik ekleme testi (ör. Section 12/13 wing + ItemSet satırı ile karakter
   stat karşılaştırması) ve alan eşleme çıkarımının (§2.5) saha teyidi sıradaki iş.
2. Kalkan (Section 6) satırları canlıda da taranmayan slotta — kasıtlı parite; canlı sahibi isterse
   ayrı karar (slot 1 dahil edilsin mi).
3. `Save` yalnız UI içindir; bizde çağrılmıyor (editör portlanmadı).
4. `Effect`/`Time` alanları okunur/yazılır ama oyun mantığında kullanılmaz (canlı birebir).

---

**Hazırlayan:** Buffy (Codebuff) · **2026-10-02 12:55** · Commit atılmadı.
