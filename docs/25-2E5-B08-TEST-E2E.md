# 25 — 2e.5: B-08 (SPK ToolTipText) + Faz 3 E2E Bağlantı Kanıtı

> Tarih: 03.10.2026 · Kapsam: B-08 içerik/şema eşlemesinin kapanışı ve Faz 3.1/3.2
> E2E bağlantı testi. Ham kanıtlar: `BuildLog\2e2\`, `BuildLog\2e3\`, `BuildLog\2e4\`.
> Test sunucusu bu turun sonunda **kapatıldı** (§5.1).

---

## 1. ÖZET

| İş | Sonuç |
|----|-------|
| B-08 (SPK içerik/şema) | ✅ **uygulandı:** `ToolTipText.txt` yükleyicisi (19 kayıt) — runtime kanıtı KEN.txt |
| Faz 3.1 test yığını | ✅ CS/DS/JS/GS (bizim derlemeler) + `MuOnlineS6` DB + `MuOnlineS6ODBC` DSN |
| Faz 3.2 E2E bağlantı | ✅ istemci ConnectServer'a **TCP ESTABLISHED** + sunucu seçim ekranı |
| Faz 3.2 UI ilerlemesi (GS/login) | ⛔ otomatikleştirilemedi — oturum **disconnected** (bkz. §4.3); manuel adım |
| Yeni bulgu | ⚠️ istemci `127.0.0.1` hedefini **kasten reddediyor** (WSctlc.cpp:230) — test düzeneği tuzağı |

---

## 2. B-08 — SPK paketindeki tooltip/metin şeması

### 2.1 Canlı kanıt (Live ölçümü)

`Client and Tools\Client\Data\SPK\Config\` içeriği (03.10.2026):

- Metin/tooltip ile ilgili dosyalar: `Text.bmd` (144.246 B), `Text.txt` (148.450 B),
  `TextHtml.txt` (20.064 B), `ToolTipText.txt` (865 B, 19 satır), `MasterSkillTooltip.bmd` (315.392 B).
- **`itemtooltip_*.bmd`, `itemleveltooltip_*.bmd`, `itemtooltiptext_*.bmd` YOKTUR**
  (canlı Engine.exe string taramasında da `itemtooltip*` yok; yalnız `ToolTipText`,
  `Text.txt`, `Text.bmd`, `MasterSkillTooltip` var).
- Bizim istemci (MUIG soyu) bu tabloları `Data\Local\<Lang>\itemtooltip_<Lang>.bmd`
  yollarıyla ister (`ZzzOpenData.cpp:5712/5726/5733`); 2e.4'te bu yollar SPK-first
  çözümleyiciye bağlandı ve dosya yokluğunda istemci artık kilitlenmiyor (H-011/H-012).

### 2.2 Uygulama (2e.5)

Canlı paket, item tooltip **metin** tablosunu düz metin olarak taşıyor:

```
1000 "Event Item"
1001 "Drop on the ground to receive rewards"
1003 "Upgrade to <f c='#FFFF00'>Wing Tier 2.5</f>"
...
1018 "Increase <f c='#00FF00'>Star</f> upgrade points"
end
```

Bu satırlar iskelet olarak `_ITEM_TOOLTIP_TEXT { WORD index; char text[256]; short type; }`
şemasının karşılığıdır (index + tek satır metin + tip).

- Yeni yükleyici: `Source\5.Main\source\ZzzInfomation.cpp` → **`load_item_tooltip_text_spk()`**
  - `tooltip_text_data[]` tablosunu doldurur (`type = -1` → düz metin),
  - `<f c='#RRGGBB'>…</f>` markup'ını temizler (istemci düz metin basar),
  - UTF-8 BOM, boş satır, yorum (`/`) ve `end` satırlarını atlar,
  - sonuç KEN.txt'ye yazılır (Release'te `g_ErrorReport` dosyaya yazmaz).
- Çağrı noktası: `ZzzOpenData.cpp` — `itemtooltiptext_<Lang>.bmd` çözümlemesi
  başarısız olduğunda `ToolTipText_<Lang>.txt` istenip **mevcut çözümleyiciyle**
  (`SPK_ResolveAssetPath` → `Data\SPK\Config\ToolTipText.txt`) yüklenir; hemen ardından
  gelen `set_item_text_tooltip()` tabloyu `m_ItemToolTipTextData` map'ine çevirir.
- Mevcut tüketici (`ZzzInventory.cpp` ~3020/3080) `m_ItemToolTipTextData` üzerinden
  `text_index` ile arar → SPK modunda da tooltip satırları çözülür.

### 2.3 Doğrulama

| Kanıt | Sonuç |
|-------|-------|
| Derleme (Global Release/Win32) | `BuildLog\2e4\build_2e5.log`, `build_2e5b.log` — hata yok (yalnız LNK4099) |
| exe md5 (2e.5) | `c49383bf62c412bf53adf1ec62c67f77` (12.031.488 B) |
| exe string kanıtı | `ToolTipText_%s.txt` + `[SPK] ToolTipText` gömülü |
| **Runtime** | run26 KEN.txt: **`[SPK] ToolTipText: 19 kayit`** (canlı dosyanın 19 satırı) |
| Regresyon | run26: diyalog yok, çökme yok, CS bağlantısı kuruldu (§3) |

`BuildLog\2e4\evidence\KEN_run26.txt` (135 satır) tam kanıt dosyasıdır.

> Kapsam notu: `itemtooltip_*.bmd` / `itemleveltooltip_*.bmd` (item başına isim/renk/
> tooltip bileşimi) ve `MasterSkillTooltip.bmd` **canlı pakette yok**; canlı istemci bu
> bilgiyi kendi `Item.bmd` + `ToolTipText.txt` + `Text.bmd` üçlüsüyle kurar. Item
> tooltip **bileşim** tablosunun SPK-mode eşdeğeri (hangi item hangi text_index'i
> kullanır) canlıda binary şema olarak bulunmadığından açık kalem olarak bırakılmıştır
> (§7 — B-08b).

---

## 3. FAZ 3.1 — TEST YIĞINI

Kaynak: docs/23 (2e.3) + bu turun yamaları. Kurulan yığın:

| Bileşen | Port | Kimlik |
|---------|------|--------|
| ConnectServer (bizim) | 63000 TCP / 63001 UDP | pid (tur içi) |
| DataServer (bizim) | 63002 | — |
| JoinServer (bizim) | 63003 | — |
| GameServer (bizim) | 55901 | — |

Bu turda eklenen yamalar:

1. `MuOnlineS6ODBC` DSN (User DSN, SQL Server sürücüsü, `WIN-4TMUUQ42DNH\SQLEXPRESS`,
   `MuOnlineS6`) → DS `DataServerODBC`, JS `JoinServerODBC`.
2. Test hesabı `e2etest` / `test123` (`MEMB_INFO`; `MD5Encryption=0` → düz parola;
   `bloc_code='0'`, `ctl1_code='0'`, `AccountLevel=0`, `AccountExpireDate=2076-06-06`, `Lock=0`).
3. GS: `ServerVersion=1.03.34`, `ServerSerial=!571Axion@Mobile` (canlı SPK değerleri).
4. CS `ServerList.ini`: `0 "Axion Mu" "45.87.120.29" 55901 "SHOW"`.
5. İstemci paketi: `Data\SPK\ConnectIP.bmd` **45.87.120.29:63000** (anti 55858) —
   üretici: `BuildLog\2e4\make_connectip.js` (md5 `afe04596509ec378d069598447f1f6aa`).

Bağlantı zinciri (yalnız log kanıtı): CS "ServerList loaded successfully" +
"JoinServer online" + "GameServer online (Axion Mu) (0)"; DS/JS "AddServer (127.0.0.1)"
+ "ServerInfo (KEN-1) (55901) (0)"; GS↔DS/JS **ESTABLISHED** (63002/63003).

---

## 4. FAZ 3.2 — E2E BAĞLANTI

### 4.1 Koşu tablosu

| Koşu | İstemci hedefi | Diyalog | TCP kanıtı | Ekran |
|------|----------------|---------|-----------|-------|
| run18 (eski exe, 127.0.0.1) | 127.0.0.1:63000 | yok | **yok** | "Lost connection to the server" |
| run19 (izleyici 100 ms) | 127.0.0.1:63000 | yok | **yok** (trap: hiç bağlantı) | aynı popup (t=20 s) |
| run20 (trap 63000, yanıtsız) | 127.0.0.1:63000 | yok | **trap sıfır bağlantı** | popup t≈6 s'de hazır |
| run21 | 45.87.120.29:63000 | yok | **ESTABLISHED t=4,6 s (koşu boyu)** | **sunucu seçim ekranı** |
| run22/23/24/25 (tıklama denemeleri) | 45.87.120.29:63000 | yok | ESTABLISHED | değişmedi (bkz. §4.3) |
| run26 (**2e.5 exe**) | 45.87.120.29:63000 | yok | **ESTABLISHED t=4,4 s** | **sunucu seçim ekranı** + `[SPK] ToolTipText: 19 kayit` |

### 4.2 Kök neden: istemci 127.0.0.1'i reddediyor

`Source\5.Main\source\WSctlc.cpp:230-234`:

```c
if (addr.sin_addr.S_un.S_un_b.s_b1 == 127 && addr.sin_addr.S_un.S_un_b.s_b2 == 0 &&
    addr.sin_addr.S_un.S_un_b.s_b3 == 0 && addr.sin_addr.S_un.S_un_b.s_b4 == 1)
{   // local host
    return (FALSE);
}
```

`CWsctlc::Connect` bu durumda **hiç SYN göndermeden** FALSE döner; `CreateSocket`
`MESSAGE_SERVER_LOST` popup'ını açar (`WSclient.cpp:268-273`). Bu, canlı istemcinin
kendi korumasıdır (parite gereği **korunmuştur**), ama test düzeneği için tuzaktır:

- Kanıt (negatif): `run19` (100 ms global TCP izleme) ve `run20` (63000'de trap
  dinleyici) → istemciden **tek paket bile** çıkmadı; ekranda popup.
- Kanıt (pozitif): hedef `45.87.120.29:63000` yapıldığında bağlantı **t=4,4-4,6 s**
  içinde kuruldu ve koşu boyunca ESTABLISHED kaldı (`run21/22/23/24/25/26` JSON'ları,
  PID bazlı `netstat` + 100 ms global örnekleme).
- Makinenin tek IPv4'ü `45.87.120.29` olduğundan kendi IP'sine bağlantı yerel yığına
  düşer (trap testiyle doğrulandı: `Test-NetConnection` → trap `CONNECT` kaydı).

**Ders:** E2E testinde `ConnectIP` hedefi **127.0.0.1 yapılamaz**; makinenin gerçek
IPv4'ü (veya LAN IP) kullanılmalıdır. Bu kural, Faz 3'ün kalan koşuları için de geçerlidir.

### 4.3 Sınır: oturum "disconnected" → UI tıklaması otomatikleştirilemiyor

Sunucu seçim ekranından sonraki adım (sunucu butonuna basma → GameServer 55901 →
login ekranı) otomatikleştirilemedi. Denenenler ve sonuçları:

| Deneme | Yöntem | Sonuç |
|--------|--------|-------|
| run22 | `PostMessage(WM_LBUTTONDOWN/UP)` istemci koordinatıyla | etkisiz |
| run23 | gerçek fare: `SetCursorPos` + `mouse_event` | etkisiz |
| run24 | + ALT ile `SetForegroundWindow`, `WM_ACTIVATE`/`WM_ACTIVATEAPP` post | `foreground=false`, etkisiz |
| run25 | + `SetWindowPos(TOPMOST)` + diyagnostik (`WindowFromPoint`) | imleç doğru pencerede, ama tıklama işlenmiyor |

Kök neden: ajan süreci **bağlantısı kesilmiş bir oturumda** çalışıyor
(`qwinsta`: session 2 `Disc`; `GetForegroundWindow() == 0`). İstemci girdisi
`g_bWndActive` (WM_ACTIVATE ile kurulur) koşuluna bağlı (`Input.cpp`), pencere
gerçekten aktif hale getirilemediği için fare olayları yok sayılıyor.

→ **Faz 3.2'nin kalan adımı (sunucu seç → login → karakter ekranı) etkileşimli bir
masaüstü oturumunda elle koşulmalıdır.** Düzeneği tek komutla yeniden kurmak için
docs/23 + `BuildLog\2e2\deploy_spk_package.sh --with-assets` kullanılabilir.
(Not: sunucu seçim ekranındaki buton metni istemci tarafından "Viet Nam" fallback'iyle
basılır; CS'in gönderdiği "Axion Mu" adı ekrana yansımadı — açık gözlem, §7.)

---

## 5. TEMİZLİK / KAPATMA (§5.1)

- Test yığını kapatıldı: CS/DS/JS/GS `taskkill` ile durduruldu; 63000-63003 ve 55901
  portları boşaltıldı. Yardımcı süreçler (trap 63000, statik sunucu 8087) kapatıldı.
- Geçici kopyalar silindi: `BuildLog\2e2\deploy` (328 MB), `BuildLog\2e3\deploy` (25 MB),
  `BuildLog\5Main` (160 MB) → **~513 MB** geri kazanıldı (disk: 3,7 GB → 4,2 GB).
  Yeniden kurulum: docs/23 + `BuildLog\2e2\deploy_spk_package.sh`.
- Kanıtlar korundu (BuildLog\2e4: koşu JSON'ları, ekran görüntüleri, KEN_run26.txt,
  build logları, araç betikleri).

---

## 6. KOŞU ARACI ENVANTERİ (bu turda üretilen)

| Dosya | İş |
|-------|-----|
| `BuildLog\2e4\run19_watch.ps1` | 100 ms global TCP izleyici + zamanlı PrintWindow ekran görüntüsü |
| `BuildLog\2e4\run22_e2e.ps1` | TCP izleme + zamanlı tıklama (post/real input) + diyagnostik |
| `BuildLog\2e4\make_connectip.js` | ConnectIP.bmd üretici/çözücü (XOR 0x20) |
| `BuildLog\2e4\trap.js` | 63000'de ham bayt yakalayıcı (kanıt: bağlantı yok) |
| `BuildLog\2e4\apply_open_data_patch.js` | EUC-KR baytlarını koruyan yama uygulayıcı (encoding regresyonu önlemi) |
| `BuildLog\2e4\shot_crop_ascii.ps1`, `downscale.ps1` | Ekran görüntüsünden kanıt okuma (ASCII / ölçekli PNG) |

---

## 7. AÇIK KALEMLER (2e.5 sonrası)

| # | Kalem | Durum |
|---|-------|-------|
| B-08b | Item başına tooltip **bileşim** tablosu (`itemtooltip_*.bmd` eşdeğeri) canlı pakette binary şema olarak yok; item↔text_index eşlemesi `Item.bmd`+`ToolTipText.txt` üzerinden çıkarılmalı | ⬜ araştırma |
| OBS-1 | Sunucu seçim ekranındaki buton metni istemcide "Viet Nam" (fallback); CS adı ekrana yansımıyor | ⬜ gözlem |
| 3.2b | Sunucu seç → GS/login → karakter akışı etkileşimli oturumda elle koşulacak (§4.3) | ⬜ manuel |
| 3.3/3.4 | Canlı ile davranış karşılaştırma listesi + hata günlüğü turu | ⬜ Faz 3 devamı |
