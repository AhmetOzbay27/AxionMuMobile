# 05 — SPK MODÜL ENVANTERİ (Faz 2a.1 çıktısı — FAZ 2c İŞ EMRİ)

> Tarih: 30.09.2026 (temiz yeniden yazım). Kaynak kanıtları:
> canlı `GameServer.map` (242 sınıf) + canlı `GameServer.pdb` (367 cpp yolu,
> 277 GS proje dosyası) + 06 envanteri config eşlemeleri. Ham veriler:
> `BuildLog\envanter\` (canli_gs_s.txt, bizim_gs_s.txt, canli_ek_dosyalar.txt,
> ortak_dosyalar.txt, bizim_ek_dosyalar.txt, canli_spk_s.txt,
> pdb_cpp_yollari.txt, pdb_linker_cmd.txt).

---

## 1. CANLI GS PROJE YAPISI (PDB + map'ten kesin)

- Canlı derleme: `D:\Mu-Mobile\MuSPK\Source\ExGameServer\GameServer\` —
  VS2022 14.44.35207 (**v143**), /LTCG /OPT:REF /OPT:ICF, SPKThemeManiFest.xml.
- **277 cpp** derleniyor; SPK modülleri `GameServer\SPK\` alt klasöründe (57).
- Ara yapı: `D:\BuildMU\Android\ExGameServer\` (spk_messlang.obj izi →
  SPK_MessLang alt birimleri içerebilir).
- 1 dosya proje dışı: `MuSPK\Source\Include\Math.cpp`.

## 2. KARŞILAŞTIRMA ÖZETİ

| Ölçüm | Değer |
|-------|-------|
| Canlı GS cpp | 277 |
| Bizim GS cpp | 281 |
| Ortak (isim paritesi) | 218 |
| **Sadece canlıda (YENİ yazılacak)** | **59** (45 SPK\ altında + 14 kökte) |
| **SPK\ altında olup bizde de olan (SPK revizyonuyla EZİLEN)** | **12** |
| Sadece bizde (bizim dallanma; 2b.1'de değerlendirilir) | 63 |

**Toplam iş kapsamı: 59 yeni dosya + 12 SPK revizyon taşıması = 71 birim.**
(map tabanlı sınıf analizi 58 demişti; PDB dosya analizi 59'a çıkardı —
SkillDamage.cpp map deseninde yakalanamamıştı.)

---

## 3. 59 YENİ MODÜL — ÖNCELİK SIRALI İŞ EMRİ

Öncelik tanımları:
- **P1 = Çekirdek oyun/ekonomi:** event iskeleti, karakter gelişim sistemleri
  (TuLuyen/QuanHam/HonHoan/DanhHieu/Relife/DungLuyen), item güçlendirme
  (Harmony), buff, ödeme (MocNap), reset sistemi, dil mesajları, SPK altyapısı,
  MUIG donor'dan hazır gelenler.
- **P2 = Önemli içerik:** shop/ekonomi genişlemesi, bot sistemi, event'ler,
  komut/UI katmanı, ağ altyapı modernizasyonu, karakter/ek ipuçları.
- **P3 = Tamamlama:** salt UI formları, küçük QoL, opsiyonel/lisans.

### P1 — 19 modül (Faz 2c ilk dalga)
| # | Dosya (konum) | Sınıf (map) | Canlı config | Kaynak |
|---|---------------|-------------|--------------|--------|
| 1 | SPK_EventMainManager.cpp (SPK\) | EventMainManager | — | sıfırdan |
| 2 | SPK_AddBuff.cpp (SPK\) | AddBuffer | AddBuff.txt | **MUIG donor** ✅ 2b.0 entegre |
| 3 | SPK_AutoHp.cpp (SPK\) | CAUTOHP | — | **MUIG donor** ✅ 2b.0 entegre |
| 4 | SPK_Harmony.cpp (SPK\) | CustomHarmony | CustomHarmony.xml | sıfırdan |
| 5 | SPK_TuLuyen.cpp (SPK\) | cTuLuyen | TuLuyen.xml | sıfırdan |
| 6 | SPK_QuanHam.cpp (SPK\) | cQuanHam | QuanHam.xml | sıfırdan |
| 7 | SPK_HonHoan.cpp (SPK\) | cHonHoan | HonHoan.xml | sıfırdan |
| 8 | SPK_DanhHieu.cpp (SPK\) | cDanhHieu | DanhHieu.xml | sıfırdan |
| 9 | SPK_Relife.cpp (SPK\) | TaiSinh | Relife.xml | sıfırdan |
| 10 | SPK_DungLuyen.cpp (SPK\) | cDungLuyen | DungLuyen.xml | sıfırdan |
| 11 | SPK_MessLang.cpp (SPK\) | cMessageNew | Message.xml (yeni bloklar) | sıfırdan |
| 12 | ZzzToolKit.cpp (SPK\) | SPK_ToolKitMain | — (SPK\ log çıktısı) | sıfırdan |
| 13 | B_MocNap.cpp (SPK\) | CSystemMocNap | CustomMocNap.xml | sıfırdan |
| 14 | CharOption.cpp (SPK\) | CCustomCharOption | CharOption.xml | sıfırdan |
| 15 | ResetChange.cpp (SPK\) | CResetChange | ResetChange.txt | sıfırdan |
| 16 | ResetLimiter.cpp (SPK\) | ResetLitmitLock | — | sıfırdan |
| 17 | ReiDoMu.cpp (kök) | CReiDoMU | — | sıfırdan |
| 18 | SkillDamage.cpp (kök) | CSkillDamage | — | **MUIG donor** ✅ 2b.0 entegre |
| 19 | CustomJewelBank.cpp (SPK\) | CCustomJewelBank | CustomJewelBank.xml | **MUIG donor** ✅ 2b.0 entegre |

### P2 — 28 modül (Faz 2c ikinci dalga)
| # | Dosya (konum) | Sınıf (map) | Canlı config | Kaynak |
|---|---------------|-------------|--------------|--------|
| 20 | SPK_ExtendShop.cpp (SPK\) | CExtendShop | ExtendShop.xml | sıfırdan |
| 21 | SPK_NewXShop.cpp (SPK\) | NewCashShop | CustomShop.xml | sıfırdan |
| 22 | SPK_ItemTrader.cpp (SPK\) | BotThuMua(er) | ThuMuaDoExc.txt | sıfırdan |
| 23 | SPK_MonsterSkill.cpp (SPK\) | CCustomMonsterSkill | CustomMonsterSkill.txt | sıfırdan ✅ **2c.1-A2 TAMAM** (canlı singleton+vector; veri `Data\SPK\CustomMonsterSkill.txt` byte-birebir — docs/16) |
| 24 | SPK_CmdSocket.cpp (SPK\) | CCommandUI | CustomCommandSocket.xml | sıfırdan |
| 25 | SPK_CustomNameColor.cpp (SPK\) | cCustomNameColor | CustomNameColor.ini | sıfırdan |
| 26 | SPK_StatsInfo.cpp (SPK\) | CSGetInfoCharacter | CharOption.xml (paylaşımlı) | sıfırdan |
| 27 | SPK_CastleEvent.cpp (SPK\) | CastleStartGuild | — | sıfırdan |
| 28 | BotOnline.cpp (SPK\) | ObjBotOnline | BotOnline.txt | sıfırdan |
| 29 | BotTradeMix.cpp (SPK\) | BotTradeMixCore | BotTradeMix.txt | sıfırdan |
| 30 | CBotMixSystem.cpp (SPK\) | CBotMixSystem | BotTradeMix.txt (paylaşımlı) | sıfırdan |
| 31 | ActiveInvasions.cpp (SPK\) | CActiveInvasions | — | sıfırdan; **02.10 E2E ✅** (docs/17) — 4 zincir bayt-birebir; 4 parite farkı: void `send_list_to_client()` + giriş push'u + `SendThongTinSauKhiVaoGame` canlıda YOK (canlı = F7 pull), `ObjectSetStateProc` respawn `monster_add(true)` bizde EKSİK |
| 32 | BEventThanMa.cpp (SPK\) | CThanMaChien | — | sıfırdan |
| 33 | SkyEvent.cpp (SPK\) | cSkyEvent | — | sıfırdan |
| 34 | TEventGreatPK.cpp (SPK\) | CCustomGreatPK | — | sıfırdan |
| 35 | CustomDameBoss.cpp (SPK\) | VPDameBoss | — | sıfırdan |
| 36 | CustomItemPro.cpp (SPK\) | SystemItemChanger | CustomItemPro.xml | sıfırdan |
| 37 | CustomItemSetPro.cpp (SPK\) | CustomSetDameItem | CustomItemSetPro.xml | sıfırdan; **02.10 A3 ✅** (docs/18) — `SPK\CustomItemSetPro.{cpp,h}`; Load @0x476640 (pugixml `Item[]`+`ItemSet[]`), Save @0x477360, CalcCharacter @0x478BF0 (ItemSet → Section 7..11 varyantı; flag != 0 no-op), CalcSlot wear 2..11; config canlıdan bayt-birebir (50858 B, md5 9b2d6ef9); E2E bekliyor |
| 38 | CustomNewBuff.cpp (SPK\) | CustomNewBuff | CustomNewBuff.xml | sıfırdan |
| 39 | CustomLuckySpin.cpp (SPK\) | CCustomLuckySpin | CustomVongQuay.xml | sıfırdan |
| 40 | GuildUpgrade.cpp (SPK\) | cSystemGuildUpgrade | GuildUpgrade.txt | sıfırdan |
| 41 | RankingServer.cpp (SPK\) | cRanking | — | sıfırdan |
| 42 | ChangePass.cpp (SPK\) | ChangePassOption | — | sıfırdan |
| 43 | PassLock.cpp (SPK\) | ItemPassLocker | — | sıfırdan |
| 44 | SocketConnection.cpp (kök) | CConnection | — | sıfırdan |
| 45 | SocketManagerModern.cpp (kök) | CSocketManager (modern) | — | sıfırdan |
| 46 | ViewInfoItem.cpp (SPK\) | ViewItemPlayer | — | sıfırdan |
| 47 | CongHuong.cpp (SPK\) | CongHuong (bot yön) | CongHuong.txt | sıfırdan |

### P3 — 12 modül (Faz 2c tamamlama dalgası)
| # | Dosya (konum) | Sınıf (map) | Canlı config | Kaynak |
|---|---------------|-------------|--------------|--------|
| 48 | PartySetPass.cpp (kök) | CPartySetPass | — | sıfırdan |
| 49 | ChecklevelVip.cpp (kök) | CheckItemVip | — | sıfırdan |
| 50 | ExWinQuestSystem.cpp (kök) | ExWinQuestSystem | CreationQuestSystem.xml + CreationQuestData_1.ini | sıfırdan |
| 51 | CustomRenameChar.cpp (SPK\) | CCustomRenameChar | — | sıfırdan |
| 52 | CustomReadGuildServer.cpp (SPK\) | (guild veri okuma) | — | sıfırdan |
| 53 | EventItemBagUI.cpp (kök) | CEventItemBagUI (+ManagerUI) | EventItemBagManager.txt | sıfırdan |
| 54 | ShopManagerUI.cpp (kök) | CShopManagerUI | ShopManager.txt | sıfırdan |
| 55 | CustomStartItemDame.cpp (kök) | CCustomStartItemDame | CustomStartItemDame.txt | sıfırdan |
| 56 | CustomStartSetItemDame.cpp (kök) | CCustomStartSetItemDame | CustomStartSetItemDame.txt | sıfırdan |
| 57 | ItemExcellentOptionRate.cpp (kök) | CItemExOptionRate | — | sıfırdan |
| 58 | LogErrorForm.cpp (kök) | CLogErrorForm | — | sıfırdan |
| 59 | GetLicenseID.cpp (kök) | (lisans) | — | sıfırdan (opsiyonel) |

**Toplam denetim: 19 (P1) + 28 (P2) + 12 (P3) = 59 ✓** — kesin liste
`BuildLog\envanter\canli_ek_dosyalar.txt` ile birebir.

### 2b.3 eklemesi (01.10.2026): 60. kalem — EventGvG + 2c bağlantı kalemleri
2b.3 çapraz kontrolü ([12-MUIG68-CAPRAZ-KONTROL.md](12-MUIG68-CAPRAZ-KONTROL.md))
MUIG-özel modüllerin canlı karsılıklarını sınıflandırdı ve bu listeye
**dışarıdan gelen 1 yeni kalem** çıkardı:
- **#60 — EventGvG (CGvGEvent):** canlı ServerInfo.obj'te 7 config anahtarı
  kanıtlı (EventGvGSwitch/Npc/NpcMap/NpcX/NpcY/MinUsers/MaxUsers); bizim
  ServerInfo.cpp'te yok. Öncelik: P2 (event). Kaynak: sıfırdan (canlı config
  şemasından). Ayrıca 2c bağlantı kalemleri: B_MocNap ↔ bizim MocNap +
  CB_AutoNapGame birleştirmesi (P1, canlı LOG_MOC_NAP aktif); MasterReset
  tablosu çözümü (canlı DGCommandMasterResetRecv). Detay: docs/12 §6.

### 2c.1 yeniden dizimi (01.10.2026 14:05)
60 modül, **oyun akışı etkisine göre** (core > event > QoL > anticheat >
kosmetik) yeniden sıralandı: **A=24 çekirdek · B=9 event · C=21 QoL ·
D=3 anticheat · E=3 kosmetik**. Uygulama iş emri:
[13-2C1-ONCELIK-IS-EMRI.md](13-2C1-ONCELIK-IS-EMRI.md) (P1/P2/P3 dizimi
içerik-temelli referans olarak korunur).

---

## 4. 12 EZİLEN DOSYA — SPK REVİZYON TAŞIMASI

Bu dosyalar bizim kaynakta da VAR ama canlıda `SPK\` altındaki sürümleriyle
derleniyor. Bizim sürümler ya eski ya farklı dallanma; parite için canlı
davranış çözümlenip bizim dosyalara taşınacak (dosya adı değişmez).
**Dosya dosya karşılaştırma + uygulama sırası: [09-EZILEN-12-DOSYA-KARSILASTIRMA.md](09-EZILEN-12-DOSYA-KARSILASTIRMA.md) (2b.0-E, 30.09.2026).**

| # | Dosya | Sınıf (map) | Canlı davranış kanıtı / config |
|---|-------|-------------|-------------------------------|
| E-01 | BossGuild.cpp | CBossGuild (29 sembol) | — |
| E-02 | BotAlchemist.cpp | ObjBotAlchemist | "BotAlchemist data load error %s" (canlı string) |
| E-03 | BotBuffer.cpp | ObjBotBuffer | buffer bot |
| E-04 | ChangeClass.cpp | CCustomChangeClass | ChangeClass.xml |
| E-05 | CustomBuyVip.cpp | CCustomBuyVip | CustomBuyVip.txt |
| E-06 | CustomEventTime.cpp | CCustomEventTime | — |
| E-07 | CustomJewel.cpp | CCustomJewel | CustomJewel.txt |
| E-08 | CustomRankUser.cpp | CCustomRankUser | — |
| E-09 | OfflineMode.cpp | OfflineMode | — |
| E-10 | Reconnect.cpp | CReconnect | GetEngine.ini ReconnectTime=1 |
| E-11 | ThuMuaDoExc.cpp | BotThuMua(er) | ThuMuaDoExc.txt |
| E-12 | ZenDrop.cpp | cZenDrop | ZenDrop.xml |

---

## 5. MODÜL BAŞINA UYGULAMA AKIŞI (her birim için)

1. **Çözümleme:** map sembolleri (sınıfın public yüzü) + canlı config dosyası
   (`Sub-1\Data\...`) + varsa canlı log izleri (`GameServer\SPK\*.txt`) +
   istemci karşılığı (Main/Engine.exe string'leri, MENUBUTTON_* etiketleri).
2. **İskelet:** header + cpp; sınıf adı map ile aynı; config okuma canlı
   formatla birebir (XML/INI/txt yapısı korunur).
3. **Entegrasyon:** çağrı noktaları (User.cpp/Protocol.cpp/GameMain.cpp vb.)
   eklenir; her dokunuş CHANGELOG'a yazılır.
4. **Derleme:** GS tam derleme hatasız (Release_EX603|Win32).
5. **Doğrulama:** yeni derlemenin .map'inde modül sembolleri belirir;
   config açılışta hatasız okunur.
6. **Kayıt:** CHANGELOG kaydı + git commit (tek birim = tek commit).

## 6. KABUL KRİTERLERİ (Faz 2c kapanışı)

- 59/59 yeni modül derleniyor ve map'te görünüyor; 12/12 ezilen dosya
  SPK davranışıyla uyumlu.
- `Sub-1\Data\` SPK config setinin tamamı (25 + Custom/BotSystem) okunuyor.
- GetEngine.ini MENUBUTTON_* dizisindeki her istemci özelliğinin sunucu
  karşılığı çalışır durumda (Faz 2e istemci testiyle birleşir).

## 7. AÇIK SORULAR

- SPK_MessLang: `spk_messlang.obj` izi → tek dosyadan fazla birim olabilir;
  D-dalga sırasında netleşecek.
- 63 "sadece bizde" dosya: 2b.1 diff matrisinde SPK tabanıyla karşılaştırılır
  (silme yok).
- `FilterRaname.cpp` (canlıdaki yazım hatalı ad) bizim kaynakta `FilterRename`
  olarak var — bu 59 listesinde değil, ortak 218'de; isim paritesi kararının
  2b'ye bırakıldı.
- EventMainManager'ın 293 sembolü: tek dosya değil, event çekirdeğinin
  tamamı — en büyük tek birim; 2c ilk dalgada en çok zaman alacak kalem.
