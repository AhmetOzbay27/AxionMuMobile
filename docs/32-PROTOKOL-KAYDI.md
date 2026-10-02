# 32 — PROTOKOL KAYDI (manifest 4. eksen)

> **Tarih:** 03.10.2026 · **Kapsam:** istemci ↔ sunucu paket sözleşmesi
> **Bağlı:** docs/30 (manifest) · docs/31 (istemci iş emri)
> **Kural:** bir opcode iki tarafta da kayıtlı değilse özellik yarım sayılır. Bu belge tek doğruluk kaynağıdır.

---

## 1. KONVANSİYON

**Sunucu tarafı** — PBMSG_HEAD3 (Source/4.GameServer/GameServer/Protocol.h:106)

    lpBuf[0] = 0xC1     paket tipi
    lpBuf[1] = size     boyut
    lpBuf[2] = headcode opcode
    lpBuf[3] = subcode  alt komut

Dağıtım: Protocol.cpp içinde switch(headcode) → gXxx.CGXxxRecv((PMSG_xxx_RECV*)lpMsg, aIndex)

**İstemci tarafı** — Source/5.Main/source/ProtocolSend.h

DataSend(uint8_t*, uint16_t) · SendPacket(ProtocolHead, ...) · alıcı: Protocol.h

**SPK önerisi:** özellik başına 2 opcode (C→S istek, S→C yanıt); komut ayrımı subcode ile yapılır. 16 özellik için 32 opcode yeter.

---

## 2. MEVCUT OPKOD ENVANTERİ (GS Protocol.cpp)

Kullanılan: **166** · toplam boş: **90** · 0xD4 üstü boş: **22**

    0x28 0x29 0x2A 0x2B 0x2C 0x2D 0x2F 0x38 0x3B 0x3E 0x44 0x46 0x47 0x48 0x49 0x4F 0x56 0x58 0x59 0x60 0x62 0x63 0x64 0x65 0x6D 0x6E 0x73 0x74 0x76 0x77 0x7A 0x7D 0x7E 0x7F 0x80 0x84 0x85 0x89 0x8D 0x8F 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9D 0x9E 0x9F 0xA1 0xA3 0xA4 0xA5 0xA6 0xA8 0xAD 0xB6 0xB8 0xBA 0xBB 0xBE 0xC6 0xCC 0xCD 0xCE 0xCF 0xD4 0xD5 0xD6 0xD7 0xD8 0xD9 0xDA 0xDB 0xDC 0xDD 0xDE 0xDF 0xE0 0xE3 0xE4 0xEA 0xF2 0xF4 0xF9 0xFB 0xFD 0xFE

H-007 notu: 0x35 iki kez tanımlıydı; AUTOHP'ye verildi, ekipman tamiri #if(0) ile kapatıldı. Tahsis tablosu tek sahiplik tanımlar, ikili tanım yapılmaz.

---

## 3. SPK ÖZELLİK TAHSİSİ (0xD4+ boş alan)

| C→S | S→C | Özellik | MENU_BUTTON | Durum |
|---|---|---|---|---|
| 0xD4 | 0xD5 | RankingServer (sıralama listesi) | 01 | ⬜ |
| 0xD6 | 0xD7 | SPK_EventMainManager (etkinlik saati) | 02 | ⬜ |
| 0xD8 | 0xD9 | SPK_Relife (yeniden canlanma) | 03 | ⬜ |
| 0xDA | 0xDB | ResetChange (reset değişimi) | 04 | ⬜ |
| 0xDC | 0xDD | SPK_DanhHieu (ünvan / nişan) | 05 | ⬜ |
| 0xDE | 0xDF | CustomLuckySpin (çark) | 06 | ⬜ |
| 0xE0 | 0xE3 | ChecklevelVip (VIP kontrol) | 07 | ⬜ |
| 0xE4 | 0xEA | B_MocNap (bağış / bakiye) | 08 | ⬜ |
| 0xF2 | 0xF4 | ChangeClass (sınıf değiştirme) | 09 | ⬜ |
| 0xF9 | 0xFB | ChangePass (şifre değiştirme) | 10 | ⬜ |
| 0xFD | 0xFE | SPK_HonHoan (ruh döngüsü) | 11 | ⬜ |
| — | — | SPK_NewXShop | 12 | ⚠ alan yok |
| — | — | SPK_TuLuyen | 13 | ⚠ alan yok |
| — | — | SPK_QuanHam | 14 | ⚠ alan yok |
| — | — | CustomItemPro | 15 | ⚠ alan yok |
| — | — | SPK_ExtendShop | 16 | ⚠ alan yok |

Kalan boş (yedek): yok

> ⚠ Alan yetmedi: SPK_NewXShop, SPK_TuLuyen, SPK_QuanHam, CustomItemPro, SPK_ExtendShop → opcode bölgesi genişletilmeli (şu an tek bayt).

---

## 4. İKİ TARAFLI KAYIT KURALI

Her özellik için dört yer güncellenmeden PROTO yeşile dönmez:

| # | Yer | Dosya |
|---|---|---|
| 1 | Sunucu karşılayıcı | Source/4.GameServer/GameServer/Protocol.cpp — case bloğu |
| 2 | Sunucu veri yapısı | Protocol.h — PMSG_SPK_xxx_RECV / _SEND (PBMSG_HEAD3 türevli) |
| 3 | İstemci karşılayıcı | Source/5.Main/source/Protocol.h — opcode eşlemesi |
| 4 | İstemci gönderim | Source/5.Main/source/ProtocolSend.h — SendPacket çağrısı |

Doğrulama: opcode'un iki exe'de de bulunması (derleme sonrası bayt taraması) + paketin 0xC1,size başlığıyla eşleşmesi (paket yakalama).

---

## 5. UYGULAMA KONTROL LİSTESİ (özellik başına)

- [ ] 1. Opcode tahsis edildi, çakışma yok (grep ile doğrulandı)
- [ ] 2. GS: veri yapısı + Protocol.cpp case + modül kancası
- [ ] 3. İstemci: alıcı + gönderici + pencere tetikleyicisi
- [ ] 4. Derleme temiz (GS + Main)
- [ ] 5. E2E: buton → paket → GS log satırı → ekran güncellemesi
- [ ] 6. Manifest 4. eksen yeşile döndü + CHANGELOG + commit
