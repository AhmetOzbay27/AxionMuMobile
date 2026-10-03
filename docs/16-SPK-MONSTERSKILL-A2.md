# 16 — SPK_MonsterSkill (A2) CANLI KANITTAN UYGULAMA + E2E

Tarih: 2026-10-02 (09:00–10:00 arası oturum)
Kapsam isteği: "2c.1-A dalgasındaki A2 MonsterSkill kalemini canlı kanıtla uygulamaya al."
(docs/13 satır 34 `| A2 | 23 | SPK_MonsterSkill | CCustomMonsterSkill | CustomMonsterSkill.txt | sıfırdan | savaş mekaniği |`)

> Kural: docs/12 §4 — parite kararlarında **canlı kanıt donor'un önünde**. Bu raporda
> her iddianın yanında canlı adres / exe offset / dosya kanıtı vardır.

---

## 1. Sonuç özeti

| # | Kalem | Önce | Şimdi (canlı parite) |
|---|-------|------|----------------------|
| 1 | Dosya konumu + adı | `GameServer\CustomMonsterSkill.{cpp,h}` | `GameServer\SPK\SPK_MonsterSkill.{cpp,h}` (canlı obje adı `SPK_MonsterSkill.obj`) |
| 2 | Sınıf erişimi | yalnız global `gCustomMonsterSkill` | `static CCustomMonsterSkill* Instance()` + global (A1 `CustomHarmony` deseni) |
| 3 | Depolama | sabit `CUSTOM_MONSTER_SKILL[1000]` + `m_count`, sınır koruması YOK (taşmada bellek bozulması), yüklemede bayat kayıtlar kalıyordu | `std::vector<CUSTOM_MONSTER_SKILL> m_Monster_Skill` + `clear()` (canlı 3-pointer deseni) |
| 4 | Config yolu | `Custom\CustomMonsterSkill.txt` | `SPK\CustomMonsterSkill.txt` (canlı yükleyici 0x5697C7, yol literali 0x63DCB4) |
| 5 | Veri dosyası | 5 satır, canlıdan farklı sınıflar | canlı dosya ile **byte-birebir** (206 B, 16 satır, CRLF, tab) |
| 6 | Canlı log stringi | `[SPK] CustomMonsterSkill configuration saved and reloaded` | `[SPK] CustomMonsterSkill configuration saved and reloaded.` (nokta dahil, exe 2344740) |
| 7 | Tüketici 1 `gObjSetMonster` | zaten parite | disasm ile doğrulandı (aşağıda §2.4) — **değişiklik gerekmedi** |
| 8 | Tüketici 2 `gObjMonsterAttack` | zaten parite | disasm + E2E ile doğrulandı (§2.5, §4) |
| 9 | `Reload()` (2b.2-R bizim ekimiz) | `m_count == 0` sezgisiyle hata tespiti | `m_LoadResult` ile kesin sonuç (başarısız yüklemede eski veri geri gelir) |

Derleme: `Release_EX603` · `Win32` · `v143` → **0 hata**, `GameServer.exe` = **10.819.072 B**
(A2 öncesi 10.817.536 B → +1.536 B).

---

## 2. Canlı kanıt (SPK_MonsterSkill.obj)

Kaynaklar: `BuildLog\envanter\live_disasm.txt` (canlı `/disasm`),
`BuildLog\envanter\GameServer_canli.map`, canlı `GameServer.exe` byte taraması.

### 2.1 Sınıfın tam yüzeyi (canlı map satır 1717-1719)

```
?Instance@CCustomMonsterSkill@@SAPAV1@XZ             004A4D00 f  SPK_MonsterSkill.obj
?Load@CCustomMonsterSkill@@QAEXPAD@Z                 004A4D70 f  SPK_MonsterSkill.obj
?GetSkillMonster@CCustomMonsterSkill@@QAEPAUCUSTOM_MONSTER_SKILL@@H@Z  004A4FC0 f
```

Canlıda `Save`/`Reload` **yok**. Dosyayı yazan kod SPK GUI editörüdür
(`SPK_MonsterSkillProc` / `MonSkillEditSubclass` @0x4A5070+; `[DaTaoHoa]` benzeri
editör ailesi) — **kapsam dışı** (A1 `SPK_HarmonyProc` ile aynı karar). Dosya formatı
kanıtı §2.3'te, yine de kayıt altına alındı.

### 2.2 Singleton + vector

- `Instance()` @004A4D00: Meyers singleton — guard `0213C8D4h`, nesne `0213C8D8h`,
  `atexit` yıkıcı `??__FpInstance@...` @0062C390h.
- Yapıcı 3 dword'ü sıfırlar (`0213C8D8h/DC/E0`) ⇒ depolama `std::vector` deseni
  (`Myfirst`/`Mylast`/`Myend`). Editör kodu satır sayısını `(end-begin)/12` ile bulur
  (`004A5552-004A5581`, imul `2AAAAAABh`) ⇒ **eleman 12 B**.
- `GetSkillMonster` @004A4FC0: `mov eax,[ecx]` (begin) / `mov ecx,[ecx+4]` (end),
  eşitse `xor eax,eax`; döngü `cmp dword ptr [eax],edx` + `add eax,0Ch`; **ilk eşleşme
  kazanır**, bulunamazsa `0` döner.
- Canlıda **üst sınır yok** (bizim eski `MAX_MONSTER_SKILL 1000` yapısı kaldırıldı).

### 2.3 `Load` akışı @004A4D70 (birebir)

1. `operator new(0x318)` ⇒ `CMemScript` (0x318 = canlı CMemScript boyutu; `_memset` 0x100).
2. `SetBuffer(const std::string&)` yolu — bizim portumuzda `SetBuffer(char*)` overload'ı
   aynı işi yapıyor (MemScript.cpp, `MEM_SCRIPT_ERROR_CODE0` yolu).
3. **`vector::clear()`** (004A4E90-004A4E9A: `[esi]`/`[esi+4]` karşılaştırıp `[esi+4]=[esi]`).
4. Dış döngü: `GetToken() == TOKEN_END (2)` ⇒ çık (004A4EA4-004A4EAE).
5. `GetNumber() != 0` ⇒ satırı atla (`continue`, 004A4EDC-004A4EE6) — yani yalnız
   **section 0** okunuyor (bizim eski kodla aynı semantik, farklı şekil).
6. İç döngü: `strcmp("end", GetAsString()) == 0` ⇒ iç döngüden çık (004A4EE8).
7. Üç sayı: `m_MonsterClass = GetNumber()` (004A4F2D), `m_Skill1 = GetAsNumber()` (004A4F3F),
   `m_Skill2 = GetAsNumber()` (004A4F51) → **`GetNumber` (tüketmez) + 2×`GetAsNumber`**
   (bizim MemScript.cpp:282/287 semantiği ile birebir).
8. `push_back` (004A4F5F-004A4F85; büyüme yolu `Emplace_reallocate`, `add [esi+4],0Ch`).
9. `catch(...)` → `ErrorMessageBox(GetLastError())` (004A4F8A-004A4FB2). Dosya bozuksa
   1 sn token zaman aşımı → `MEM_SCRIPT_ERROR_CODE4`
   (`[%s] The file were not configured correctly`, MemScript.h:12).

**Yükleyici**: `0x569791 (?Instance)` + `0x5697C7 (?Load)`, yol literali
`??_C@_0BL@OPFIBIKL@SPK?2CustomMonsterSkill?4txt@` @0x63DCB4 = `SPK\CustomMonsterSkill.txt`
(`strcpy_s(gPath)` + `strcat_s` deseni). Canlı yükleme sırası:
`Message.xml` → `AddBuff (SPK\AddBuff.txt)` → **`CustomMonsterSkill`** →
`CustomShop (SPK\CustomShop.xml)` → `CustomJewelBank (SPK\CustomJewelBank.xml)`.

**Dosya formatı** (canlı editör kaydı 0x4A55B9-0x4A5655, `fprintf` biçimleri):
`"0\n"` + `"//MonsterClass\tSkill1\tSkill2\n"` + `"%d\t%d\t%d\n"` × N + `"end\n"`,
sonra `Instance()->Load(yol)` + `LogAdd("[SPK] CustomMonsterSkill configuration saved and reloaded.")`
+ `MessageBoxW(L"Info", L"Saved and reloaded successfully!")`.

### 2.4 Tüketici 1 — `gObjSetMonster` @0x51FD62

```
0051EC73: movzx eax,word ptr [esi+32Eh]   ; lpObj->AttackType
0051EC7A: test ax,ax
0051EC7D: je  0051FE59                    ; AttackType == 0 ⇒ TÜM blok atlanır (custom skill dahil)
...
0051FD62: call ?Instance
0051FD67: mov ebx,[eax] / mov eax,[eax+4] ; vector begin/end
0051FD74: cmp dword ptr [ebx],edi         ; m_MonsterClass == MonsterClass
0051FD78: add ebx,0Ch
0051FD84: test ebx,ebx / je 0051FE59      ; bulunamadı ⇒ çık
0051FD91: mov edi,[ebx+4]                 ; m_Skill1
0051FD96..0051FDC0: CheckSkillRequire{Level,Energy,Leadership,Class}(lpObj,skill)
0051FDC4: GetSkill(lpObj,skill)
0051FDCD..51FDEC: boş yuva ara ([esi+33Ch], 0x3C yuva, 0x10 bayt) → CSkill::Set(skill,0)
0051FDF1: mov edi,[ebx+8]                 ; m_Skill2 (aynı akış, 0051FE59'a kadar)
```

**Önemli:** Buradaki `CheckSkillRequire*` çağrıları `gObjSetMonster`'ın kendi mantığı
DEĞİL, **`CSkillManager::AddSkill` gövdesinin LTCG inline'ıdır** (bizim
`SkillManager.cpp:798` aynı kod; `lpObj->Type == OBJECT_USER` kapısı canlıda `[esi+64h]==1`,
bizde `[esi+54h]==1`). Canlıda alan `OBJECTSTRUCT::Type` ofseti 0x64, bizde 0x54 —
monster için bu dal ölüdür. Yani canlı kod fiilen:

```cpp
if(lpObj->AttackType != 0) {
    ...
    CUSTOM_MONSTER_SKILL* lpInfo = CCustomMonsterSkill::Instance()->GetSkillMonster(MonsterClass);
    if(lpObj != 0) { gSkillManager.AddSkill(lpObj,lpInfo->m_Skill1,0);
                     gSkillManager.AddSkill(lpObj,lpInfo->m_Skill2,0); }
}
```

**Bizim `Monster.cpp` bloğu zaten bu semantikti** (553. satır) → bu turda yalnız kanıt
yorumu eklendi. Bizim temiz derlemede de aynı inline deseni doğrulandı:
`bizim_disasm3.txt` @0x004DCE2D-004DCEA1 → `[gCustomMonsterSkill]`/`[+4]` begin-end,
`add ebx,0Ch`, `cmp [ebx],edi`, sonra inline AddSkill.

### 2.5 Tüketici 2 — `gObjMonsterAttack` @0x521571

`gObjMonsterAttack` bir Class zinciridir; zincirin **sonunda**:

```
00521567: mov ecx,231h
0052156C: cmp ax,cx
0052156F: je  005215DB              ; Class == 561 (0x231) ⇒ DURATION dalı
00521571: call ?Instance
00521576: movzx ecx,word ptr [esi+9Ch]   ; lpObj->Class
00521580: call ?GetSkillMonster
00521585: test eax,eax
00521587: jne 005215DB              ; custom kayıt varsa AYNI dal
```

`005215DB` dalı: `PMSG_DURATION_SKILL_ATTACK_RECV` (`header.set(0x1E, sizeof)`),
`skillL = (GetLargeRand()%100 >= 25)`, `x/y/dir/angle/dis/MagicKey` doldurulup
`CGDurationSkillAttackRecv(&pMsg, lpObj->Index)`.

Bizim kod (`Monster.cpp:1674`, A2 yorumu eklendi):
`else if(lpObj->Class == 561 || gCustomMonsterSkill.GetSkillMonster(lpObj->Class))`
→ **aynı koşul, aynı hedef branş** (E2E'de class 704 için dal seçimi kanıtlandı).

---

## 3. Uygulama (dosya bazlı)

| Dosya | Değişiklik |
|-------|------------|
| `Source\4.GameServer\GameServer\SPK\SPK_MonsterSkill.h` | **yeni** (eski `CustomMonsterSkill.h` yerine): canlı kanıt başlığı, `Instance()`, `std::vector<CUSTOM_MONSTER_SKILL> m_Monster_Skill`, `Reload()`, `m_Path`, `m_LoadResult` |
| `Source\4.GameServer\GameServer\SPK\SPK_MonsterSkill.cpp` | **yeni** (eski `CustomMonsterSkill.cpp` yerine): `Instance()` (global döner), canlı `Load` akışı (clear + section 0 + end + 3 sayı + push_back + catch), vector `GetSkillMonster`, `Reload` (canlı log stringi) |
| `GameServer.vcxproj` | `ClInclude="SPK\SPK_MonsterSkill.h"`, `ClCompile="SPK\SPK_MonsterSkill.cpp"` (eski kök kayıtları kaldırıldı) |
| `ServerInfo.cpp` | `#include "SPK/SPK_MonsterSkill.h"`; yükleme yolu `SPK\\CustomMonsterSkill.txt` |
| `CommandManager.cpp` | include yolu; `/reload custommonsterskill` yorumu canlı stringle (nokta dahil) |
| `Monster.cpp` | include yolu; iki tüketici noktasına canlı kanıt yorumu (davranış değişmedi) |
| `MuServer\4.GameServer\Sub 1\Data\SPK\CustomMonsterSkill.txt` | **yeni**; canlı dosya ile byte-birebir (`cmp` OK, 206 B) |
| `MuServer\4.GameServer\Sub 1\Data\Custom\CustomMonsterSkill.txt` | **silindi** (canlıda böyle bir dosya yok; yol artık `SPK\`) |

`Reload()` bizim eklentimizdir (Faz 2b.2-R; canlı editörün "yaz → Load → LogAdd"
akışının komut karşılığı). Bu turda hata tespiti `m_count == 0` sezgisinden
`m_LoadResult` alanına çevrildi: dosya okunamazsa/parse edilemezse eski veri geri
yüklenir ve `Reload failed - old data restored` yazılır (E-05 deseni).

---

## 4. E2E doğrulama (test sunucusu)

Geçici test sürücüsü (tur sonunda **kaldırıldı**, arşiv `BuildLog\2c1\`):

- `SPK_MonsterSkill.cpp`: `CustomMonsterSkillSelfTest()`, `CustomMonsterSkillAttackBranchTest()`,
  `CustomMonsterSkillTestTick()` (1 sn throttle + `SPK\MonsterSkill_selftest.flag` tetiği).
- `SPK\EventMainManager.cpp` `MainProc` 1 sn bloğu: `{ extern bool CustomMonsterSkillTestTick(); CustomMonsterSkillTestTick(); }`
- `Monster.cpp`: `#include "Log.h"` + `extern bool CustomMonsterSkillAttackBranchTest(int);`,
  `gObjSetMonster` custom bloğunda 1 `LogAdd`, `gObjMonsterAttack` koşulu geçici olarak
  `CustomMonsterSkillAttackBranchTest(lpObj->Class)` (aynı boolean + dal logu).

Sonuçlar (`MuServer\4.GameServer\Sub 1\GameServer\LOG\2026-10-02.txt` 4523-4543, 09:48:51):

```
[MSTEST] Instance()=01EFD200 gCustomMonsterSkill=01EFD200 ayni=1 | kayit=16 (canli SPK\CustomMonsterSkill.txt = 16 satir)
[MSTEST] loader yolu: ..\Data\SPK\CustomMonsterSkill.txt
[MSTEST] GetSkillMonster(750) -> {class=750,skill1=41,skill2=232} beklenen {750,41,232} OK
[MSTEST] GetSkillMonster(704) -> {class=704,skill1=42,skill2=264} ... OK      (706/561/754/722 de OK)
[MSTEST] 719 mukerrer kayit ilk-eslesme kurali -> skill1=232 skill2=0 (beklenen ilk kayit 232/0)
[MSTEST] GetSkillMonster(99999)=00000000 (beklenen 0)
[MSTEST] gObjSetMonster custom dal: class=704 skill1=42 skill2=264
[MSTEST] spawn idx=1143 class=704 AttackType=150 Type=2 | GetSkill(42)=1 GetSkill(38)=0 GetSkill(264)=1
[MSTEST] spawn idx=1216 class=706 AttackType=150 Type=2 | GetSkill(42)=0 GetSkill(38)=1 GetSkill(264)=0
[MSTEST] spawn idx=1338 class=700 AttackType=150 Type=2 | GetSkill(42)=0 GetSkill(38)=0 GetSkill(264)=0   ; kontrol (canli listede yok)
[MSTEST] gObjMonsterAttack DURATION dal secildi: class=704 custom=1 (561 mi=0)
[MSTEST] gObjMonsterAttack cagrildi: attacker idx=1143 class=704 -> target idx=1216 class=706
[SPK] CustomMonsterSkill configuration saved and reloaded.
[MSTEST] Reload sonrasi kayit=16 (once=16)
[MSTEST] ===== SELFTEST BITTI (spawn edilen 3 obje silindi) =====
```

Temiz (sürücüsüz) derleme ile son açılış: `09:52:27` ve (sonuç derlemesiyle)
`10:16:18 [ServerInfo] Custom loaded successfully` — modal hata yok, `MSTEST` satırı yok,
`.dmp` yok, süreç ayakta.

Sonuç artefaktı: `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe` =
**10.819.072 B**, md5 `ee0b85303236b9858acb0b09e5dfa7ee` (10:15 derlemesi; A2 öncesi
10.817.536 B).

Kanıtlananlar: **16 satır canlı veri** ✅ · `SPK\` yolundan yükleme ✅ · `Instance()`
tekilliği ✅ · ilk-eşleşme kuralı (719 mükerrer kaydı) ✅ · negatif yol (`0`) ✅ ·
`gObjSetMonster` → `AddSkill` skill dağıtımı (704→{42,264}, 706→{38}, 700→yok) ✅ ·
`gObjMonsterAttack` DURATION dalı custom koşulla seçiliyor ✅ · `Reload` canlı log
stringini yazıyor ✅.

---

## 5. Bu turda öğrenilen tuzaklar

1. **`AddSkill` inline'ı yanıltıyor:** Canlı `gObjSetMonster` disasm'ında görünen
   `CheckSkillRequire*` zinciri ek kontrol değil, `CSkillManager::AddSkill`'in LTCG
   inline'ı. Bir tüketici noktasını canlıyla karşılaştırırken önce çağrılan fonksiyonun
   gövdesini (bizim kaynakta) doğrulamak gerekir; yoksa "canlıda fazladan kontrol var"
   yanılgısı doğar (bu turda tam olarak bu yanılgı düzeltildi).
2. **`if(lpObj->AttackType != 0)` kapısı:** custom skill dağıtımı bu kapının **içinde**;
   `AttackType == 0` olan bir sınıfa canlı da skill vermez (canlı 0051EC7D → 0051FE59).
3. **`#if(GAMESERVER_UPDATE>=701)`:** bizim `Release_EX603` derlemesinde `>=701` dalı
   **derlenmiyor**. Geçici enstrümantasyon o dala konursa hiç çalışmaz (bu turda bir kez
   yaşandı, enstrümantasyon koşul ifadesine taşınarak çözüldü).
4. **Yol taşıma:** `Data\SPK\` klasörü bizde zaten vardı (AddBuff/ChangeClass/CustomBuyVip/
   CustomHarmony). A2 dosyası da oraya taşındı; eski `Data\Custom\CustomMonsterSkill.txt`
   canlıda olmadığı için silindi.
5. **Boşluk/tab ayracı:** `CMemScript` tokenizer'ı için ayraç önemsiz; ama canlı dosya
   birebir hizalandı (§6), böylece ileride byte-karşılaştırması yapılabilir.
6. **Kodlama notu (zararsız ama şeffaf):** `Monster.cpp` düzenlenirken dosyada önceden var
   olan geçersiz UTF-8 baytları (donor'ın `//<- aqu\xEE custommonsterskill` ve
   `//==Get Top v\xE0 Send` yorumlarındaki tek baytlar) editör tarafından **U+FFFD** ile
   normalize edildi. Yorum metni dışında etkisi yok; derleme ve davranış etkilenmedi
   (`git diff` yalnız 3 yorum satırında bu farkı gösterir — biri bu turda zaten
   düzenlenen satırlar).

---

## 6. Veri dosyası parite kanıtı

```
$ cmp "MuServer/4.GameServer/Sub 1/Data/SPK/CustomMonsterSkill.txt" \
      "/c/Axion Mu Mobile/4.MuServer/Sub-1/Data/SPK/CustomMonsterSkill.txt"
BYTE-IDENTICAL to live          # 206 B, 16 satır, CRLF, tab ayraçlı
```

İçerik sırası: 750/41/232 · 751/39/0 · 752/235/0 · 753/56/0 · 754/78/0 · 755/214/0 ·
756/232/0 · 719/232/0 · 704/42/264 · 561/38/0 · 706/38/0 · 717/262/0 · 712/78/0 ·
723/42/0 · 722/13/0 · 719/4/0 (719 iki kez — canlıda da böyle; `GetSkillMonster` ilk
kaydı döner).

---

## 7. Doğrulama araçları / komutlar

```
# derleme (0 hata)
export MSYS2_ARG_CONV_EXCL='*'
cd "/c/Axion Mu Source/Source/4.GameServer/GameServer"
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  GameServer.vcxproj -p:Configuration=Release_EX603 -p:Platform=Win32 \
  -p:PlatformToolset=v143 -m -v:q -nologo

# bizim temiz derleme disasm (yeni, A2 sonrası)
cd "/c/Axion Mu Source/MuServer/4.GameServer/Sub 1/GameServer"
".../Hostx64/x86/dumpbin.exe" /disasm GameServer.exe > ../../../BuildLog/envanter/bizim_disasm3.txt
```

Doğrulanacak bizim adresler (`BuildLog\envanter\bizim_disasm3.txt`):
`?GetSkillMonster@CCustomMonsterSkill@@` @0x00465B30 (vector taraması, `2AAAAAABh` = /12,
`add eax,0Ch`), `gObjSetMonster` custom bloğu @0x004DCE2D+ (begin/end + `add ebx,0Ch`),
`gObjMonsterAttack` @0x004DEC24 (`call ?GetSkillMonster`). Exe stringleri:
`SPK\CustomMonsterSkill.txt` @1498696, canlı log stringi @1477040;
`Custom\CustomMonsterSkill.txt` **yok**.

---

## 8. Kalan işler / notlar

- **SPK GUI editörü** (`SPK_MonsterSkillProc`, satır ekle/sil/kaydet + `MessageBoxW`)
  portlanmadı — A1'deki editör kararıyla aynı (kapsam dışı). Dosya formatı §2.3'te kayıtlı.
- **Diğer eski kopyalar dokunulmadı:** `MuServer\4.GameServer\Data\Custom\CustomMonsterSkill.txt`,
  `MuServer\4.GameServer_real\Sub 1\Data\Custom\CustomMonsterSkill.txt`,
  `ServerTools\MuServer_S6_2020\Data\Custom\CustomMonsterSkill.txt` — bunlar çalışan
  (Sub 1) ağaç değil; ileride temizlik kalemi.
- **Sınıf adı korundu:** kaynak dosya adı canlı obje adı (`SPK_MonsterSkill.cpp`),
  sınıf adı `CCustomMonsterSkill` (canlı sembolle aynı).
- **`Instance()` bizde inline oluyor** (ayrı sembol yok; `gCustomMonsterSkill` adresini
  döner). Aynı durum A1 `CustomHarmony` için de geçerli (canlıda out-of-line Meyers
  singleton, bizde global + `Instance()`); davranış farkı yok.
- Sıradaki kalemler: docs/14'ün 7 kesin SPK paket başlık düzeltmesi, B4 SPK_CastleEvent,
  B5 BEventThanMa, `CUSTOM_JEWEL_INFO` tam parite backlog'u, `BossGuild` NPC spam'i
  (`[BossGuild] Ko tao duoc NPC` her saniye).
