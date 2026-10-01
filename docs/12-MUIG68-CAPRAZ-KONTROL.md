# 12 — 2b.3: MUIG-ÖZEL MODÜLLERİN CANLI ENVANTERLE ÇAPRAZ KONTROLÜ

> **Tarih/Saat:** 01.10.2026 · tarama başlangıç **06:27**, tarama bitiş **06:36**,
> rapor **06:56**. Yöntem ve ham veri: `BuildLog\envanter\`
> (`muig68_donor_only.txt`, `muig68_capraz_kontrol.csv`,
> `muig68_kanit_detay.txt`, `canli_obj_listesi.txt`; tarama betiğinin gövdesi
> CHANGELOG kaydında saklanır).
> Canlı sistem **salt okunur** tarandı (Sub-1 config + exe string + map + PDB + log).

---

## 1. 68 SAYISININ NETLEŞTİRİLMESİ (sayım düzeltmesi)

"68 MUIG-özel modül" ilk kez `C:\Axion Mu Mobile\analiz\SPK-KAYNAK-ANALIZ-20260930.md`
(§9, satır 268) kullanıldı: MUIG GS **562** dosya ↔ SPK GS **564** dosya
karşılaştırmasında MUIG'e özgü modül sayısı olarak (örnekler: AUTOHP,
AntiSkillDelay, BlackList, BotTrade, CGMHardwareId, CGMEarringManager,
CGMFlagNatManager, CMixGoblinExpansion, CharacterAdvance, APIGameGuard).

Bu turda kesin sayım yeniden üretildi (donör kök dizin ↔ bizim
`Source\4.GameServer\GameServer` kök dizin, ad-paritesi):

| Ölçüm | Değer |
|-------|-------|
| Donör GS kök cpp+h | 558 (562 vcxproj kaydı; lua\ + Release_License alt klasör farkı) |
| Donör cpp | 277 — **canlı PDB cpp sayısıyla (277) birebir** |
| Bizim GS kök cpp+h | 566 |
| Ortak ad | 496 |
| **Donör-özel dosya** | **62** (29 cpp + 33 h) |
| − 2b.0'da zaten alınmış 8 dosya (4 modül × cpp+h, bizde `SPK\` altında) | AUTOHP, CustomJewelBank, PC_AddBuff, SkillDamage |
| **Net 2b.3 inceleme birimi** | **29 cpp modülü** (54 dosya) |

> "68" orijinal analiz raporunun tarihsel etiketi olarak kalır; bu raporun
> tamamında kesin sayı 62 dosya / 29 modül kullanılır.

**Ham liste (29 modül):** APIGameGuard, AUTOHP✅, AntiSkillDelay, B_MocNap,
BlackList, BotTrade, CGMEarringManager, CGMFlagNatManager, CGMHardwareId,
CGMPetManager, CMixGoblinExpansion, CharacterAdvance, ChatManager, ConsoleDebug,
CustomExchangeCoin, CustomJewelBank✅, EventFindPath, EventGvG, GMHolyItem,
LogToFile, MasterResetTable, MultiLanguage, MyTimer, PC_AddBuff✅, SendMessage,
SkillDamage✅, SkillDamageConfig, ThuongDanhBoss, WindowsConsole
(✅ = 2b.0'da alınmış; bu turda yeniden doğrulandı).

---

## 2. YÖNTEM — HER MODÜL İÇİN 6 KANIT HATTI

1. **Donör derleme durumu:** `GameServer.vcxproj` ClCompile kaydı (29/29 EVET).
2. **Sınıf adları:** donör cpp+h'den `class/struct` yüzeyi çıkarıldı.
3. **Canlı `GameServer.map`** (kopya: `GameServer_canli.map`): `?Sınıf@` sembol
   araması + `.obj` adı araması (606 benzersiz obj → `canli_obj_listesi.txt`) +
   ham-ad taraması.
4. **Canlı PDB cpp yolları** (`pdb_cpp_yollari.txt`, 277 cpp).
5. **Canlı exe string dump** (`c.s`) ↔ bizim exe string dump (`b.s`): donör
   kaynak ayırt edici literal'ları (≥8 karakter) iki dump'ta arandı.
6. **Canlı config/log:** `Sub-1\Data\` ağacında donör config adı araması +
   `Sub-1\GameServer\SPK\` canlı loglarında modül izi.

Sonuç CSV'si (her satır: `modul;donor_derleniyor;siniflar;map_vurusu;pdb_yolu;
canli_string;bizim_string;donor_config;canli_config;canli_log`):
`BuildLog\envanter\muig68_capraz_kontrol.csv` + ayrıntı dökümü
`muig68_kanit_detay.txt`.

---

## 3. SONUÇ — A GRUBU: CANLIDA VAR, PARİTE TAMAM (7 modül)

| Modül (donör adı) | Canlı karşılık | Kanıt zinciri | Bizim durum |
|---|---|---|---|
| **AUTOHP** (CAUTOHP) | `SPK\SPK_AutoHp.cpp` | PDB yolu VAR; map obj `SPK_AutoHp.obj`; istemci `Btn_AutoHp.spk` (docs/06 §4) | ✅ 2b.0 (SPK\AUTOHP.cpp) + H-007 opcode 0x35 |
| **PC_AddBuff** (AddBuffer) | `SPK\SPK_AddBuff.cpp` | PDB yolu VAR; map obj `SPK_AddBuff.obj` (254 sembol) | ✅ 2b.0 (SPK\PC_AddBuff.cpp); config `SPK\AddBuff.txt` |
| **CustomJewelBank** (CCustomJewelBank) | `SPK\CustomJewelBank.cpp` | PDB yolu VAR; map obj VAR; canlı config `SPK\CustomJewelBank.xml` | ✅ 2b.0 (SPK\CustomJewelBank.cpp); define JEWELBANKVER2 |
| **SkillDamage** (CSkillDamage) | `SkillDamage.cpp` (kök) | PDB yolu VAR; map obj VAR | ✅ 2b.0 (SPK\SkillDamage.cpp) |
| **B_MocNap** (gBMocNap) | `SPK\B_MocNap.cpp` | **PDB yolu VAR**; map `B_MocNap.obj` (DATA_SYSTEM_MOCNAP / MESSAGE_INFO_CMOCNAP izleri); canlı log `SPK\LOG_MOC_NAP\` AKTİF | **Bizim paralelim VAR:** MocNap.cpp/h + CB_AutoNapGame.cpp/h (CoinMocNap/CoinNhan/GiaTriNap/ConfigMocNap akışı, `CustomMocNap.xml` okur). Canlı revizyon muhtemelen bu ikisinin birleşimi → 2c'de karşılaştırılacak |
| **BotTrade** (ObjBotTrader) | `SPK\BotTradeMix.cpp` + BotThuMua | map `BotTradeMix.obj` (TRADEMIX_REQITEM/REWARDITEM); PDB yolu VAR; canlı log `LOG_OUT_TRADEBOT` 10 gün aktif; canlı config `BotTrader.xml` + `ThuMuaDoExc.txt` | ✅ kapsanıyor: bizim `BotTrader.cpp/h` + `ThuMuaDoExc.cpp/h` tabanda (E-11 → docs/09) |
| **SkillDamageConfig** (SkillDamageRate) | SkillDamage'nin rate katmanı | donor SkillDamage.cpp DamageRateUser/Monster okuması; canlıda ayrı modül izi YOK | ✅ bizim SPK\SkillDamage.cpp aynı satırlarda (25/66/68/108/112) birebir paralel |

> A grubu kararı: **7/7 için ayrı iş kalemi YOK** — 4'ü 2b.0'da alınmış
> durumda, 3'ü (B_MocNap, BotTrade, SkillDamageConfig) bizim tabanda
> paralel/parçalı mevcut; canlı revizyon farkları docs/09 tarzı E-kalem
> karşılaştırmasıyla 2c'de işlenir (B_MocNap ↔ MocNap/CB_AutoNapGame
> birleştirmesi en önemlisi).

---

## 4. SONUÇ — B GRUBU: CANLIDA İŞLEVSEL KARŞILIĞI VAR, DONÖR KODU YANLIŞ KAYNAK (3 modül → 2c)

| Modül (donör) | Canlı kanıt | Bizim taban | 2c kalem |
|---|---|---|---|
| **EventGvG** (CGvGEvent) | canlı ServerInfo.obj'te **7 config anahtarı**: `EventGvGSwitch, EventGvGNpc, EventGvGNpcMap, EventGvGNpcX, EventGvGNpcY, EventGvGMinUsers, EventGvGMaxUsers` — modül canlıda VAR | bizim ServerInfo.cpp'te EventGvG **0** vuruş → canlıda olan bu blok bizde eksik | **YENİ 2c kalemi (2c.1-B2 TAMAMLANDI 26.10.01 21:43):** motor donor'dan birebir alındı — canlı map yeniden tarandı, **CGvGEvent sınıf sembolleri canlıda YOK** (motor canlı exe'de derli değil, sadece config okuma bloğu var); donor canlısında da `Event\\GvGEvent.dat` deploy'suz + `/startgvg` komut satırı yok → kapalı modül; bizde donor birebir + load guard + inert GvGEvent.dat (Switch=0/BLANK) — donor canlı davranışı korunur |
| **AntiSkillDelay** (CSkillDelayManager) | canlıda işlev `SkillManager.obj → CSkillManager::CheckSkillDelay` içinde; ayrı modül değil | bizim SkillManager.cpp'te CheckSkillDelay **VAR** | İşlev zaten parite; donör dosyası çöp → ayrı kalem YOK |
| **ThuongDanhBoss** | donor'da derleniyor (ClCompile VAR) ama canlıda SIFIR iz (map/PDB/string/config/log) → MUIG-özel, SPK'ya hiç geçmemiş | bizde yok | Canlıda olmadığı için **parite kapsamı DIŞI**; istenirse v2 özelliği olarak değerlendirilir |

> Not: canlı `DGCommandMasterResetRecv` (CommandManager.obj) — "Master Reset"
> komutu canlıda VAR. Donör MasterResetTable modülü bunun tablosal desteği
> olabilir, ancak donor MasterResetTable.cpp canlı sembolleriyle
> (MASTER_RESET_TABLE_INFO vb.) eşleşmiyor → ilişki 2c'de canlı map'ten
> çözülecek (bkz. §6 açık kalemler).

---

## 5. SONUÇ — C GRUBU: CANLIDA YOK → MUIG-ÖZEL, OFF BAYRAĞI (19 modül)

Aşağıdaki 19 modülün canlı GS'de (map sembol + obj + PDB yolu + exe string +
config + log — altı hat birden) **hiçbir izi yoktur**. Faz 2 paritesi kapsamı
dışındadır; kaynakla **gelecek bayrakla** taşınır (varsayılan KAPALI) ya da
hiç taşınmaz. Tek tek gerekçeler:

| # | Modül | Sınıf(lar) | Neden canlıda-yok kararı (arıza izleri) |
|---|-------|-----------|------------------------------------------|
| C-01 | APIGameGuard | APIGameGuard | map `?APIGameGuard@` 0; `GameGuard` ham-adı 0; PDB yolu yok. (Canlı koruma kendi hattında: HackCheck/MHP) |
| C-02 | CGMHardwareId | CGMHardwareId | map `HardwareId` 1 vuruş → `CustomerHardwareId` stringi **ServerInfo.obj**'te (farklı işlev, ServerInfo anahtarı; bizim ServerInfo.cpp'te zaten VAR). Modül sembolü 0 |
| C-03 | CGMEarringManager | CGMEarringManager | map `Earring` 0 |
| C-04 | CGMFlagNatManager | CGMFlagNatManager | map `FlagNat` 0 |
| C-05 | CGMPetManager | CGMPetManager | map `PetManager` 0 (bizde CustomPet hattı var, canlı CustomPet.txt) |
| C-06 | CMixGoblinExpansion | CMixGoblinExpansion | map `MixGoblin`/`GoblinExpansion` 0; SDHP_MIX_GOBBLIN stringi canlıda yok |
| C-07 | CharacterAdvance | CCharacterAdvance | map `Advance` vuruşları CRT/SkyEvent metni (yalan-pozitif); STATS_ADVANCE 0 |
| C-08 | ChatManager | CChatManager | map `ChatManager` 0; donör "Message" literal'ı jenerik (yalan-pozitif) |
| C-09 | ConsoleDebug | CMuConsoleDebug | map `ConsoleDebug` 0 |
| C-10 | CustomExchangeCoin | CCustomExchangeCoin | map `ExchangeCoin` vuruşu yalnız `DGLuckyCoinExchangeRecv` (LuckyCoin — başka modül) |
| C-11 | EventFindPath | CEventFindPath | map `FindPath` vuruşları CMapPath/CMonsterAIUtil (jenerik) |
| C-12 | GMHolyItem | CGMHolyItem | map `HolyItem` 0 |
| C-13 | LogToFile | CLogToFile | map `LogToFile` 0 (canlı log altyapısı SPK_ToolKitMain) |
| C-14 | MasterResetTable | CMasterResetTable | modül sembolü 0 (canlı MasterReset komutunun bağlantısı 2c'de çözülür — §4 notu) |
| C-15 | MultiLanguage | CMultiLanguage | map `MultiLanguage` 0 |
| C-16 | MyTimer | CTimer, CTimer2 | map `CTimer` sembolü 0 (donör sınıfları) |
| C-17 | SendMessage | CSendMessage | map vuruşları user32 `SendMessageW/A` import'ları (yalan-pozitif) |
| C-18 | WindowsConsole | CConsoleWindow | map `WindowsConsole` 0 |
| C-19 | BlackList (donör CBlackList) | CBlackList | modül sembolü 0; canlı IP-blok işlevi `CIpManager::AddBlacklist/IsBlacklisted` içinde (IpManager.obj — ortak 218'de, bizde de VAR). Canlı `Data\BlackList.txt` bu yüzden bizim IpManager hattıyla okunacak; donör CBlackList'in config formatı farklı (BlackList_Block.txt) |

> C-19 önemli netleştirme: canlı BlackList.txt kanıtı (önceki turlarda not
> edilmişti) donör BlackList modülünün kanıtı DEĞİL — canlı bunu IpManager
> üzerinden kullanıyor. SocketManager bağımlılık notundaki "BlackList"
> (2b.2-P0, docs/11) IpManager anlamındadır; donör modülü kapsam dışıdır.

---

## 6. AÇIK KALEMLER (2c'ye devreden)

1. **EventGvG modülü (YENİ 2c kalemi):** canlı ServerInfo 7 anahtar kanıtlı;
   bizim ServerInfo.cpp'te yok. Canlı config şeması (ServerInfo.ini / SPK
   bölümü) 2c'de çözülüp modül yazılacak. Bu, 05 §3'ün 59 listesine
   **dışarıdan gelen 60. keşif** olabilir (map desenine yakalanmamıştı çünkü
   tamamen ServerInfo-string'li, kendi .obj'si küçük).
2. **B_MocNap ↔ MocNap/CB_AutoNapGame birleştirmesi:** canlı `SPK\B_MocNap.cpp`
   + aktif `LOG_MOC_NAP` → canlı revizyon 2c'de canlı config şemasından
   (`CustomMocNap.xml`, coin/nạp hattı) bizim iki dosyaya taşınacak.
3. **MasterReset bağlantısı:** canlı `DGCommandMasterResetRecv` hangi tabloyu
   okuyor — donör MasterResetTable mı, canlı-özel mi — 2c'de map + string
   zinciriyle çözülecek.
4. **C grubu 19 modül:** OFF-bayrak kararı kullanıcı onayına bırakıldı;
   varsayılan plan taşımamak (canlıda olmadığı için pariteyi etkilemez).

---

## 7. KABUL

- 2a.1/2a.2 envanteriyle çapraz kontrol **tamamlandı**: 29/29 modül için altı
  kanıt hattı tarandı, CSV + kanıt dökümü `BuildLog\envanter\`'a yazıldı.
- Faz 2b kapanışı için engel yok: A grubu 7/7 zaten kapsanıyor (4'ü 2b.0
  entegre), B grubu 3 kalem 2c iş emrine eklendi (EventGvG, B_MocNap
  birleştirmesi, MasterReset çözümü), C grubu 19 modül canlıda-yok kararıyla
  OFF (varsayılan: taşınmaz).
- **Yeni keşif:** EventGvG canlıda VAR (7 ServerInfo anahtarı) ve bizde eksik —
  05 envanterine sonradan eklenecek tek modül.

**Doğrulama:** bu adım yalnızca salt-okunur tarama + dokümantasyondur; kod
değişikliği yoktur → GS derlemesi etkilenmez (son bilinen: 10.776.576 B,
febde56b1). CSV/kanıt çıktıları `BuildLog\envanter\` altındadır (BuildLog
klasörü repo kuralı gereği git dışıdır — bkz. docs/06).

**Commit** — `6b509e2be` · **Tamamlandı** — 01.10.2026 07:00 (commit 07:05)
