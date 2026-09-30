# 01 — ARAŞTIRMA REHBERİ (bilgi bankası)

> Projeye ait tüm mimari bilgilerin derlemesi. Yeni kalıcı bilgi edinildiğinde
> bu dosyaya eklenir; sohbet hafızasına güvenilmez.

---

## 1. PROJE KİMLİĞİ

- **Proje:** Axion Mu Source — tek birleşik kaynak kod (PC + Android cross-platform)
- **Kök:** `C:\Axion Mu Source\` (eski konum `C:\Axion Mu Mobile\New Source Code\Axion Mu Source\` taşındı/silindi)
- **Git:** main dalı; user "Axion Mu" / admin@axionmu.com
  - `f530df5` ilk commit (97.114 dosya) → `dd2fb78` ClientBuild dışlandı →
    `eb87f89` Resource.h onarımı → `c28026f` yeni GameServer.exe+pdb →
    `1339a290` Faz 1 tamam (Main + GetMainInfo) · **etiket:** `faz1-tamamlandi`
- `.gitignore`: Debug/Release/obj/pdb/.vs/ipch/pch/BuildLog/ClientBuild/android build çıktıları hariç.
  MuServer klasörü (218 MB) repoda.

## 2. İKİ KAYNAK HATTI

| Hat | Konum | Rol |
|-----|-------|-----|
| **SPK / Takumi** | Tabanımız (`Source\` altında); kanıt: canlı PDB yolları `D:\Mu-Mobile\MuSPK\Source\ExGameServer\GameServer\` (+SPK\ alt klasörü, 228 dosya) | **REFERANS/HEDEF** — canlı sunucu bu hattın derlemesi |
| **MUIG** | `C:\Axion Mu Mobile\New Source Code\Source\Source\` (Main5.2 + ConnectServer/DataServer/JoinServer/GameServer + Encoder) | **DONÖR** — 161 ortak dosyada daha yeni sürüm + 68 özel modül (SkillDamage, CustomJewelBank, AUTOHP, CGMHardwareId, APIGameGuard vb.) |

- Paylaşılan dosyalar PC+Android ortak yazılmış: `Platform\` (PlatformDefs,
  MobileTime, gl_compat, MuThreadPool), `Scenes\` (MainScene, SceneManager,
  SceneCore), `android_main.cpp` (10.840 satır), `android\` alt modüller.
- Android tarafı sokol (`sokol-master\`) + SDL tarzi tipler (Uint64 vb.) kullanır.

## 3. CANLI SUNUCU GERÇEKLERİ

- Canlı GS: `C:\Axion Mu Mobile\4.MuServer\Sub-1\GameServer\GameServer.exe`
  (6.979.072 B, 30.09.2026) — **ASLA DOKUNULMAZ**.
- Canlı PDB boyutları (referans kıyas noktası):
  CS 103.936 · DS 1.030.656 · JS 943.616 · GS 10.689.536 — yeni derlemlerimizle birebir.
- Canlı PDB, 57 modülün hiçbir kaynak setinde olmadığını gösterdi
  (SPK_* 20 modül dahil) → Faz 2c'de yeniden yazılacak (565 dosya etkisi).
- Canlı test istemcisi: `ClientBuild_192.168.99.200\` (Main.exe 12.003.328 B,
  md5 56b7bdee...; gömülü IP 192.168.0.150; Data\Local altında CBGetMain.bin,
  CBTextInfo.bin, License.json, Main.json dağıtılır — GetMainInfo.exe YOK).

## 4. DERLEME BİLGİSİ (Faz 1 sonuçları)

- **Araç:** MSBuild VS2022 Community: `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe`
- **Ortak parametreler:** `-p:Platform=Win32 -p:PlatformToolset=v143 -m -v:q -nologo`
- **Konfigler:** CS/DS/JS/GS → `Release_EX603`; GetMainInfo → `Release`; Main → `"Global Release"` (ad boşluklu!)
- MSYS2 bash: `export MSYS2_ARG_CONV_EXCL='*'` zorunlu.
- MFC gerekmez (UseOfMfc=false her yerde); ATL kurulu (atltime.h için).
- Çıktı konumları: MuServer\1./2./3./4. klasörleri, `ClientFile\Main.exe`, `GetMain\GetMainInfo.exe`.
- Main linker hatalarının kökleri: vcxproj eksik TU (ScenePerfTelemetry.cpp)
  ve inline tanımın başka TU'dan çağrılması (TERRAIN_ATTRIBUTE).
- GetMainInfo boyut farkı (3,69 MB vs 369 KB) varyant farkı — düzeltme DEĞİL (bkz. 03 liste).

## 5. KAYNAK DİLİ / KOD KURALLARI (donor ortam paritesi)

- `windows.h` **NOMINMAX OLMADAN** include edilir (MUIG StdAfx.h böyle);
  ~100 çıplak `min()`/`max()` çağrısı windows.h makrolarına dayanır.
- Yeni katman (SPK) `std::min/std::max` kullanır ama `(std::max)` parantezli
  stile yazılmıştır → makro varken de derlenir. **NOMINMAX ekleme!**
- `Uint64`: Android'de SDL'den gelir; PC dalında stdafx.h'de
  `typedef unsigned long long Uint64;` tanımlı (stdafx.h, !__ANDROID__ guard'lı).
- `MU_MobilePerfNow/Frequency/...`: Android'de sokol_time; PC'de
  `Platform\MobileTime.h` içinde inline QPC implementasyonu (bizim eklediğimiz).
- `GetMessage`: windows.h ANSI makrosu (`#define GetMessage GetMessageA`)
  CustomMessage.h sınıf üyesini bozuyordu → sınıftan önce `#undef GetMessage`,
  sınıfta `GetMessageA` alias'ı; Win32 mesaj döngüsünde açık `GetMessageA`.
- Kaynak dosyaların kodlamaları karışık (UTF-8 / UTF-16LE / CP949 / CP1252) —
  düzenlerken `file`/`iconv` ile kontrol et; GameServer.rc UTF-16LE'dir.
- Resource.h (GS): UTF-16 hasarlıydı; 101 tanım kurtarıldı + eksikler eklendi
  (IDM_INVASION14-21=161-168, IDM_STARTBSV=169, IDM_EVENTS_CTCMINI=170,
  IDM_EVENTS_BOSSGUILD=171, ID_FAKEONLINE_*=32800-32802, _APS_NEXT_COMMAND_VALUE=32803).

## 6. ARAYÜZ/VERİ FORMATLARI (canlı istemciden okunan kanıtlar)

- Canlı Main.exe: CBGetMain.bin + License.json hattı (MUIG varyantı) — string analizi ile doğrulandı.
- SPK GetEngine referansı (`Client and Tools\GetMain\GetMainInfo.exe`, 369 KB):
  `GetEngine.ini`, `..\Client\Data\SPK\ConnectIP.bmd`, `ServerData.bmd`,
  `SPK_CRCFILE.ini`, `..\Client\Data\SPK\Config\Info\Custom*.bmd` yolları.
- String-parite yöntemi: PE içinden `[\x20-\x7E]{10,}` ASCII dizileri çıkarılıp
  küme karşılaştırması (PowerShell + comm). Araç çıktıları BuildLog\ altında.

## 7. DİSK VE ORTAM KISITLARI

- C: ~60 GB, ~5 GB boş (VS, Client.rar 0.9 GB, MuDevs/MuEditor 312 MB korunur).
- `C:\ProgramData\Package Cache` (1.2 GB) VS onarımı için korunur.
- VS pre-check tam workload 12,8 GB ister — azaltılmış set (v143+SDK+ATL) yeterli.
- MSYS2'de strings/objdump YOK → PowerShell regex ile binary analiz.
