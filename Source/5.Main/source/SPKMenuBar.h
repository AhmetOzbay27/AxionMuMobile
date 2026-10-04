// SPKMenuBar.h — Faz 3C.0 (docs/31): canlı SPK 20 slotlu özellik menü çubuğu.
//
// Canlı sözleşme: GetEngine.ini [SuperKhung] MENU_BUTTON_01..20 → GetMainInfo.exe
// ServerData.bmd 0x4F9..0x50C'ye 20 bayt olarak yazar (Source/6.GetMainInfo/SPK/
// ServerDataWriter.cpp). Canlı Engine.exe bu baytları okuyup menü çubuğunu çizer.
// Bizim 5.Main istemcisi bu bloğu 3C.0 ile CSPKData üzerinden okur (SPKData.h,
// 0x4F9 ofsetleri) ve burada 20 slotlu çubuğu çizer.
//
// NOT (docs/31 düzeltmesi): MenuCustom.cpp'teki 15 slotlu menü CBGetMain.bin
// (MAIN_FILE_INFO.Menu[15]) ile beslenir; SPK paketinde bu dosya yok. Bu yüzden
// SPK paketinde menü tamamen boş kalıyordu. 3C.0 bu köprüyü kurar.
#pragma once

// Canlı slot sırası (GetEngine.ini [SuperKhung] yorum sütunu = canlı Türkçe etiket)
enum eSPKMenuSlot
{
	SPK_SLOT_RANKING = 0,		// 01 Sıralama
	SPK_SLOT_EVENTTIME,		// 02 Etkinlik Saati
	SPK_SLOT_RELIFE,			// 03 Yeniden Canlanma (Relife)
	SPK_SLOT_RESETCHANGE,		// 04 Reset Değişimi
	SPK_SLOT_DANHHIEU,		// 05 Ünvan / Nişan (Danh Hieu)
	SPK_SLOT_SPIN,				// 06 Çark (Spin)
	SPK_SLOT_VIP,				// 07 VIP / ID Seviyesi
	SPK_SLOT_MOCNAP,			// 08 Bağış / Bakiye Yükleme (Moc Nap)
	SPK_SLOT_CHANGECLASS,		// 09 Sınıf Değiştirme
	SPK_SLOT_CHANGEPASS,		// 10 Şifre Değiştirme
	SPK_SLOT_HONHOAN,			// 11 Ruh Döngüsü (Hon Hoan)
	SPK_SLOT_XSHOP,			// 12 X-Shop / Yeni Mağaza
	SPK_SLOT_TULUYEN,			// 13 Yetiştirme (Tu Luyen)
	SPK_SLOT_QUANHAM,			// 14 Askeri Rütbe (Quan Ham)
	SPK_SLOT_STAR,				// 15 Yıldız Seçeneği
	SPK_SLOT_JEWELSHOP,		// 16 Mücevher Mağazası
	SPK_SLOT_UNUSED17,			// 17 canlıda kullanılmıyor
	SPK_SLOT_UNUSED18,
	SPK_SLOT_UNUSED19,
	SPK_SLOT_UNUSED20,
	SPK_SLOT_COUNT
};

class CSPKMenuBar
{
public:
	CSPKMenuBar();

	// ServerData.bmd 0x4F9 bloğu okundu mu (CSPKData::m_MenuBlockLoaded)
	bool IsReady() const;

	// Slot canlı baytı: 0 = kapalı, 1 = açık
	bool IsEnabled(int slot) const;

	// Slotun canlı Türkçe etiketi (GetEngine.ini yorum sütunu)
	char* GetSlotLabel(int slot) const;

	// Slotun istemci tarafı karşılığı yazıldı mı (false ise "yakında" mesajı)
	bool HasFeature(int slot) const;

	// Etkin slot sayısı (0 ise çubuk çizilmez)
	int GetEnabledCount() const;

	// Çubuğu çizer ve tıklamayı işler; Interface::Work() içinden çağrılır.
	void Draw();

	// Slot → özellik penceresi (test/harici çağrı için)
	void ActionSlot(int slot);

private:
	// Slotun sol üst köşesinin ekran koordinatı
	void GetSlotRect(int slot, float& x, float& y, float& w, float& h) const;

	bool m_Clicked;			// bu karede tıklandi mi (yeniden cift tetik korumasi)
	int m_LastSlot;			// son acilan slot
};

extern CSPKMenuBar gSPKMenuBar;