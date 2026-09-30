# 04 — HATA GÜNLÜĞÜ

> Açık hata yapmayın: bulduğunuz her hata önce buraya AÇILIR, çözülünce
> KAPATILIR (kök neden + çözüm + doğrulama yöntemi ile). Kapananlar
> sayfada kalır — bilgi bankası görevi görür.

Durum: 🔴 AÇIK · 🟢 ÇÖZÜLDÜ · 🟡 ERTELENDİ

---

## AÇIK HATALAR

| ID | Tarih | Bileşen | Hata | Durum |
|----|-------|---------|------|-------|
| H-004 | 30.09.2026 | Main (Client) | Kaynak içine gömülü IP `171.235.182.88` canlıdaki `192.168.0.150` ile uyuşmuyor; ayrıca config'ten okuma düzeni net değil | 🔴 → Faz 2e.1'de çözülecek |
| H-005 | 30.09.2026 | GetMainInfo | Bizim derleme (3,69 MB) canlı istemci akışıyla ilişkisiz varyant; 369 KB SPK GetEngine referansıyla format farkı | 🔴 → Faz 2a.5/2d karar sonrası |
| H-006 | 30.09.2026 | GameServer (parite) | Faz 1'de GS "canlıyla birebir" sanıldı — yanlış: eşleşme MuServer'daki ESKİ referansla (10.689.536 B, 28.04.2026); CANLI GS 6.979.072 B ve v100 toolset (msvcp100/msvcr100 kanıtı) | 🔴 → anlayış düzeltildi; parite hedefi işlevsel olacak, boyut değil (bkz. 06 envanter §1) |

## ÇÖZÜLEN HATALAR (arşiv)

| ID | Tarih | Bileşen | Hata | Kök neden | Çözüm | Doğrulama |
|----|-------|---------|------|-----------|-------|-----------|
| H-001 | 30.09.2026 | GameServer | `Resource.h` UTF-16 hasarlı; `IDM_INVASION12+` tanımları yok → C2051 | Bozuk kodlama kaybı | UTF-8 dönüşümü + 101 orijinal tanım kurtarıldı, eksik IDM_/ID_FAKEONLINE_ tanımları eklendi | GS `Release_EX603\|Win32` derlendi, boyut 10.689.536 B (canlı PDB ile birebir) |
| H-002 | 30.09.2026 | Main | C2535 `GetMessageA` redefinition (CustomMessage.h) | windows.h `#define GetMessage GetMessageA` makrosu sınıf üyesini genişletiyor | Sınıftan ÖNCE `#undef GetMessage` + sınıfta `GetMessageA` alias; `Winmain.cpp` mesaj döngüsünde açık `GetMessageA` | Main derlemesi bu hatasız geçti |
| H-003 | 30.09.2026 | Main | C3861 `min`/`max` + `Uint64` + `MU_MobilePerfNow` + LNK2001 `g_mainScenePerfSnapshot`/`TERRAIN_ATTRIBUTE` | a) yanlış eklenen `NOMINMAX` (donor ortam makro bekliyor) b) PC dalında SDL tipi yok c) MobileTime Win32 dali yok d) vcxproj'da ScenePerfTelemetry.cpp eksik e) inline tanım başka TU'dan çağrılıyor | NOMINMAX kaldırıldı; `Uint64` typedef eklendi; `Platform/MobileTime.h` QPC dali; ScenePerfTelemetry.cpp vcxproj'a; TERRAIN_ATTRIBUTE dış bağlantı | Main.exe 12.023.808 B derlendi; canlı ile ~%99,7 string paritesi |

### Hata ekleme şablonu
```
| H-XXX | TT.AA.YYYY | Bileşen | Hata özeti | Durum |
```
Çözümde arşiv tablosuna şu sütunlarla taşınır: Kök neden / Çözüm / Doğrulama.
