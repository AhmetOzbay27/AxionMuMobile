# 14 — SPK PAKET BAŞLIK TARAMASI (canlı exe ↔ bizim exe)

> **Tarih:** 02.10.2026 06:50 · **Tür:** salt-okunur tarama — **kod değişikliği YOK**
> **İstek (kullanıcı):** "canlı ve bizim exe'de istemci paket tabanını karşılaştır:
> F3 98/99 dışında taslakta yanlış başlıkla kalmış diğer SPK paketlerini tara ve
> raporla."
> **Bilinen referans (bu taramadan önce düzeltildi):** B3 ActiveInvasions
> `C1 10 F3 99` / `C2 F3 98` (commit `c7d0b464e`, docs/12 §4).
> **Kural:** parite kararında canlı kanıt donor'un önündedir (docs/12 §4).

---

## 0. YÖNETİCİ ÖZETİ

- Tarama, iki exe'deki **320 canlı + 331 bizim** paket-üreten fonksiyonu
  eşleştirdi; 138'i birebir aynı, **67'sinde alan farkı** çıktı. Bunların
  **9'u SPK klasöründe** (canlı map → obj eşlemesiyle).
- **Ham disasm bayt kanıtıyla KESİN 7 bulgu** (hepsi "taslakta yanlış başlık"):
  1. `CSystemMocNap::UserSendClientInfo` → bizde **D3 9A**, canlı **D3 16**
  2. `CSystemMocNap::SendListNhanThuong` → bizde **D3 9B**, canlı **D3 17**
  3. `CustomHarmony::SendListItemPoint` → bizde **D3 24**, canlı **D3 0C**
  4. `CCustomJewelBank::GCCustomJewelBankInfoSend` → bizde **C1 30 F3 F5 (48 B)**,
     canlı **C2 0084 F3 F5 (132 B, 30 slot)** — başlık tipi + struct farkı
  5. `CBotMixSystem::SendDataIsTrade` → bizde **D3 2E**, canlı **D3 23**
  6. `CCustomLuckySpin::MakeItem` + `ActionVongQuay` → bizde **D3 8C**, canlı **D3 21**
  7. `CastleStartGuild::SendKillCTCMini` → bizde **F3 33**, canlı **F3 43**
     (B4 SPK_CastleEvent ön bulgusu)
- **Orta güven (eksik/ek paket) 4 aday:** Harmony `ProcMix` (canlı ek `F7 04`
  + JewelBank yenileme çağrısı), Harmony `SetStateInterface` (canlı `F3 13`),
  Harmony `SendInfoItemCache` (canlı `D3 00`), Store `OnPShopBuyItemRecv`
  (canlı ek `C1 20 18 06`).
- **1 uyarı:** bizim buff paketi `F3 13` kullanıyor (EffectManager.cpp:1875);
  canlıda `F3 13` **başka bir paketin** alt-kodu (Harmony arayüz paketi,
  24 B) — istemci yönlendirmesi çakışır. Canlı buff paketi `C1 40 F3 26` (64 B).
- SPK dışı 58 fark (BloodCastle/DevilSquare/ItemManager/… çoğu yeniden-kurma
  gürültüsü) tam liste: `paket_diff.txt`; SPK filtresi: `paket_diff_spk.txt`.

---

## 1. YÖNTEM

**Girdiler**

| Artefakt | İçerik |
|----------|--------|
| `BuildLog\envanter\live_disasm.txt` | canlı exe `dumpbin /disasm` (40,7 MB) |
| `BuildLog\envanter\bizim_disasm.txt` | bizim Release_EX603 exe `/disasm` (25,2 MB) |
| `BuildLog\envanter\GameServer_canli.map` | canlı sınıf → .obj eşlemesi |
| `BuildLog\envanter\canli_spk_klasoru.txt` | canlı SPK klasörü dosya listesi (57) |

**Araçlar (bu tur eklendi, `BuildLog\envanter\`)**

| Script | İş |
|--------|----|
| `paket_triaj.js` | İki header dosyasını isim+imza ile eşler → `paket_diff.txt` (67 fark, ham kanıt satırlarıyla) |
| `paket_spk_filtre.js` | Farkları canlı .obj'ye eşler → `paket_diff_spk.txt` (9 SPK-içi) |
| `paket_imm.js` / `paket_imm2.js` | Fonksiyon bazında **tüm** immediate farkı + tek-bayt near-miss (`D3 9A ↔ D3 16` tipini yakalar) |
| `paket_imm_spk.js` | Near-miss taramasını SPK objelerine indirger |
| `paket_nearmiss.js` | Yeniden kurulan kayıtlarda tek-alan farkı (yapısal triyaj) |

**Yeniden kurma (paket_header_*.txt):** `mov byte/word/dword ptr [base+disp],NNh`
store'ları aynı base'de ardışık bloklara ayrıştırılıp `C1/C2/C3/C4 + size + head
+ sub` başlığı kurulur (canlı 320, bizim 331 fonksiyonda başlık çıktı).

**Bilinen limitler (rapordaki güven derecelerinin sebebi):**
- Eşleşme isim+imza iledir; **sınıf adları farklı olabilir** (bizde `CB_BotTrader` ↔ canlı `CBotMixSystem`, `CCustomVongQuay` ↔ `CCustomLuckySpin`, `CCTCmini` ↔ `CastleStartGuild`) — aynı modülün yeniden adlandırılmış hali varsayımı; dayanak: fonksiyon gövde şekli ve problem alanı.
- Blok birleştirme, **inlined çağrıları** çağıran fonksiyona yazabilir
  (ör. Harmony `ProcMix` içindeki `F7 04`). Orta güvenli maddeler bundandır.
- `size=39952`/`size=8720` gibi kayıtlar (BloodCastle/ChaosCastle
  `GiveUserRewardExperience`, `ItemGet`) **araç artefaktı** — başlık alanı
  taşması; gerçek fark değil.
- Kesin karar her zaman ham disasm baytıyla verildi (adresler aşağıda).

---

## 2. KESİN FARKLAR — ham disasm bayt kanıtı

### 2.1 MocNap — iki paketin alt-kodu yanlış (D3 9A/9B → 16/17)

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?UserSendClientInfo@gBMocNap@@QAEXH@Z` | `?UserSendClientInfo@CSystemMocNap@@QAEXH@Z` |
| Kanıt | `0x4D9BA4: C6 85 F4 EF FF FF C2` + `0x4D9BB1: 66 C7 85 F7 EF FF FF **D3 9A**` | `0x449584: C6 85 F4 EF FF FF C2` + `0x449591: 66 C7 85 F7 EF FF FF **D3 16**` |
| Kaynak | `GameServer\MocNap.cpp:159` → `pMsg.header.set(0xD3, 0x9A, 0)` | — |
| Gövde | C2 zarfı, `bl`'den size, `DataSend` — **birebir aynı kod şekli** | " |

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?SendListNhanThuong@gBMocNap@@QAEXHH@Z` | `?SendListNhanThuong@CSystemMocNap@@QAEXHH@Z` |
| Kanıt | `0x4DA008: 66 C7 84 24 8B 01 00 ... **D3 9B**` | `0x4496DD: 66 C7 44 24 4B **D3 17**` |
| Kaynak | `MocNap.cpp:254` → `pMsg.header.set(0xD3, 0x9B, 0)` | — |

> Bizim `NhanThuongMocNap` (C1 07 F3 40) iki tarafta **aynı** — sorun yalnız
> iki `UserSendClientInfo`/`SendListNhanThuong` alt-kodunda.

### 2.2 Harmony `SendListItemPoint` — alt-kod 0x24 → 0x0C

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?SendListItemPoint@CustomHarmony@@QAEXHH@Z` | aynı |
| Kanıt | `0x49E767: C7 44 24 0C **C2 00**` + `0x49E778: C6 44 24 10 **24**` | `0x49577C: C7 85 E4 EF FF FF **C2 00**` + `0x495786: C6 85 E8 EF FF FF **0C**` |
| Kaynak | [SPK_Harmony.cpp:385](../Source/4.GameServer/GameServer/SPK/SPK_Harmony.cpp#L385) → `set(0xD3, 0x24, sizeof)` | — |

**Not:** Kaynak yorumu (satır 380) canlıyı zaten `C2 0xD3:0C00C2` olarak not
düşmüş, ama kod `BCustomVIPChar` deseni `0x24`'te kalmış — tam "taslakta kalmış
başlık" örneği. Canlıda `sizeof` = 12 = alt-kod ile aynı değer.

### 2.3 CustomJewelBank InfoSend — zarf tipi + slot sayısı (C1 30 → C2 0084)

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?GCCustomJewelBankInfoSend@CCustomJewelBank@@QAEXPAUOBJECTSTRUCT@@@Z` | aynı |
| Başlık | `0x49AEF8: C7 44 24 10 **C1 30**` + `0F5F330C1h` → **C1 30 F3 F5** | `0x47DD69: C7 85 70 FF FF FF **C2 00 84 F3**` + `0x47DD79: C6 85 74 FF FF FF **F5**` → **C2 0084 F3 F5** |
| Boy | `sizeof(pMsg)` = **48** (PSBMSG_HEAD + `int ItemBank[10]` + 3×BYTE) | memset `push 84h` = **132** = 8(başlık, hizalı) + 120 + 3 + pad |
| İçerik | 10 slot | `0x47DD8C: mov ecx,1Eh` (30) + `rep movsd` ← `[esi+10F8h]` = **30 slot** |
| Kaynak | [CustomJewelBank.cpp:67](../Source/4.GameServer/GameServer/SPK/CustomJewelBank.cpp#L67); `User.h:1211` → `int ItemBank[10]` | — |

> Tek satırlık `set(...)` düzeltmesi **yetmez**: C2 (WORD size) zarfa geçiş,
> `ItemBank[30]` ve `OBJECTSTRUCT` alan genişliği birlikte ele alınmalı.
> İstemci (donor client) `C2` boyutundan okuduğu için 30 slot beklediği
> uygulama turunda istemci tarafından da teyit edilmeli.

### 2.4 BotMix (bizde CB_BotTrader) `SendDataIsTrade` — alt-kod 0x2E → 0x23

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?SendDataIsTrade@CB_BotTrader@@QAEXHHHH@Z` | `?SendDataIsTrade@CBotMixSystem@@QAEXHHHH@Z` (aynı .obj ailesi: `CBotMixSystem.obj`) |
| Kanıt | `0x43307B: C7 44 24 30 C2 00` + `0x433083: C6 44 24 34 **2E**` | `0x419A7F: C7 44 24 30 C2 00` + `0x419A87: C6 44 24 34 **23**` |
| Kaynak | [CB_BotTrader.cpp:1078](../Source/4.GameServer/GameServer/CB_BotTrader.cpp#L1078) → `set(0xD3, 0x2E, 0)` | — |

### 2.5 LuckySpin (bizde CustomVongQuay) — alt-kod 0x8C → 0x21

| | Bizim | Canlı |
|---|---|---|
| Fonksiyonlar | `MakeItem` (`0x475D52`, `8CD30CC1h`) ve `ActionVongQuay` (`0x476316`, `8CD30CC1h`) | `MakeItem` (`0x48CCDD`, `21D30CC1h`) ve `ActionVongQuay` (`0x48D1C0`, `21D30CC1h`) |
| Değer | **C1 0C D3 8C** (size 12, head D3) | **C1 0C D3 21** |
| Kaynak | [CustomVongQuay.cpp:383](../Source/4.GameServer/GameServer/CustomVongQuay.cpp#L383) ve `:492`; ayrıca liste paketleri `:156 → D3 8A`, `:251 → D3 8B` | canlı modülde (`CCustomLuckySpin`) yakalanan tek alt-kod **0x21** |

### 2.6 CastleEvent (B4) ön bulgusu — `SendKillCTCMini` F3 33 → F3 43

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?SendKillCTCMini@CCTCmini@@QAEXHH@Z` | `?SendKillCTCMini@CastleStartGuild@@QAEXHH@Z` (**`SPK_CastleEvent.obj`** = B4 iş emri) |
| Kanıt | `0x45A971: C7 44 24 14 C1 2C` + `33F32CC1h` → **C1 2C F3 33** | `0x59564A: C7 44 24 14 C1 2C` + `43F32CC1h` → **C1 2C F3 43** |
| Kaynak | [CTCMini.cpp:555](../Source/4.GameServer/GameServer/CTCMini.cpp#L555) | — |

> Boyut (0x2C=44) ve head (F3) aynı; yalnız alt-kod farklı. Ayrıca bizim
> `F3 32` paketi (CTCMini.cpp:658) canlı taramada karşılıksız kaldı —
> B4 uygulanırken birlikte çözülecek.

### 2.7 (Yarı-kesin) EffectManager `GC_BuffInfo` — F3 13/52 B → F3 26/64 B

| | Bizim | Canlı |
|---|---|---|
| Fonksiyon | `?GC_BuffInfo@CEffectManager@@QAEXHH@Z` | aynı |
| Kanıt | `0x48C347: C7 45 C8 **C1 34**` + `13F334C1h` → **C1 34 F3 13** (52 B) | `0x4CBE0C: C7 44 24 18 **C1 40**` + `26F340C1h` → **C1 40 F3 26** (64 B) |
| Kaynak | [EffectManager.cpp:1875](../Source/4.GameServer/GameServer/EffectManager.cpp#L1875) → `h.set(0xF3, 0x13, sizeof)` | — |

> **Çakışma uyarısı:** canlıda `F3 13` = CustomHarmony arayüz paketi
> (`SetStateInterface`, `C1 18` = 24 B, adres `0x494DAC`). Bizim buff paketi
> aynı alt-kodu taşıdığı için istemci bu iki paketi karıştırır.

---

## 3. EKSİK / EK PAKETLER — orta güven (inlined çağrı olabilir)

| Fonksiyon | Canlıda olan | Bizde | Kaynak notu |
|---|---|---|---|
| `CustomHarmony::ProcMix` | `0x4950AF: C1 18 F7 04` (24 B, `DataSend`) + hemen ardından `0x4950D3: call GCCustomJewelBankInfoSend` | GCItemModifySend (`F3 14`) inlined; JewelBank yenilemesi **yok** | SPK_Harmony.cpp:282 |
| `CustomHarmony::SetStateInterface` | `0x494DAC: C1 18 F3 13` (24 B) | paket yok (yalnız `SendListItemPoint`) | SPK_Harmony.cpp:135 |
| `CustomHarmony::SendInfoItemCache` | `0x4959FF: C2 … D3 00` (değişken boy) | GCItemModifySend çağrısı | SPK_Harmony.cpp:396 |
| `CCustomStore::OnPShopBuyItemRecv` | ek `0x4B3104: C1 20 18 06` (32 B) | yok (yalnız `3F 08` / `3F 06` çifti bizde de var) | CustomStore.cpp |

> `F7 04`, canlıda CustomJewelBank.obj'de de aynı değerle geçiyor (GUI yenileme
> paketi). Uygulama turunda canlı `0x4950AF` bloğunun hangi yardımcıdan
> geldiği (inlined mi, sınıf metodu mu) ayrıca çözülmeli.

---

## 4. SPK DIŞI 58 FARK (özet)

Tam liste: `BuildLog\envanter\paket_diff.txt`; sınıf→obj dağılımı:
`paket_diff_spk.txt`. Öne çıkanlar:

- `CCustomPick::ItemGet`: bizim **C3 10 22 FE** (`setE`, şifreli zarf —
  CustomPick.cpp:203/318) ↔ canlı **C1 10 22 FE** (şifresiz zarf). Head/result
  aynı, yalnız zarf tipi farkı; parite kuralında canlı kazanır.
- `CCustomAttack::OnAttackMonsterAndMsgProc`: canlı taraf `C1 06 26 FF`
  (`0x4708A3`); bizde `set(0x26, sizeof)` kaynakta mevcut
  (CustomAttack.cpp:389/410) — fark büyük olasılıkla yeniden-kurma artefaktı,
  uygulama turunda ham disasm ile teyit edilmeli.
- `CGChaosCastleEnterRecv` (canlı `C1 05 AF 01` ↔ bizde `C1 05 C1 05`),
  `DGGensSystemRewardRecv`, `CGItemDropRecv`, `CGTradeOkButtonRecv`,
  `GDCharacterInfoSaveSend` (4604 ↔ 4564 B struct), `DataServerProtocolCore`
  — bunlar SPK kapsamı dışındaki çekirdek modüller; ayrı iş emri konusu.

**Araç artefaktı olduğu doğrulananlar** (peşine düşülmesin): BloodCastle/
ChaosCastle/IllusionTemple `GiveUserRewardExperience` (`size=39952`),
`CCustomPick::ItemGet` (`size=8720`), `DGCharacterInfoRecv` (`size=62344`),
`ManagementCore`/`DataServerProtocolCore` çok-blok birleşmeleri.

---

## 5. ÖNERİLEN DÜZELTME SIRASI

1. **Harmony `D3 24 → 0C`** ve **MocNap `D3 9A/9B → 16/17`** — tek değerlik,
   riski en düşük; istemci dispatcher'ı doğrudan etkiler.
2. **BotMix `D3 2E → 23`**, **LuckySpin `D3 8C → 21`** (+8A/8B karşılığı
   belirsizse canlıdan ek tarama).
3. **CastleEvent `F3 33 → 43`** — B4 (SPK_CastleEvent) uygulamasına girdi.
4. **JewelBank `C1 30 → C2 0084` + `ItemBank[30]`** — struct + User.h + istemci
   teyidi gereken tek büyük kalem.
5. **Buff `F3 13 → F3 26` / 52→64 B** — `F3 13` çakışması nedeniyle öncelikli;
   canlı yapı alanları (64 B) tersine mühendislik ister.
6. Harmony eksik paketleri (`ProcMix F7 04`, `SetStateInterface F3 13`,
   `SendInfoItemCache D3 00`) — canlı blokları çözülüp uygulanmalı.

> Bu turda **hiçbir kaynak dosya değiştirilmedi** (istek: yalnız tara ve
> raporla). Düzeltme turu ayrı bir iş emri olarak yürütülmeli.
