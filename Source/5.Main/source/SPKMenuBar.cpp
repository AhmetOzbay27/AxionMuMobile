// SPKMenuBar.cpp — Faz 3C.0 (docs/31): canli SPK 20 slotlu ozellik menusu cubugu.
//
// Canli sozlesme zinciri:
//   GetMain\GetEngine.ini [SuperKhung] MENU_BUTTON_01..20
//     -> GetMainInfo.exe (bizim Source/6.GetMainInfo/SPK/ServerDataWriter.cpp)
//     -> ClientFile\Data\SPK\ServerData.bmd 0x4F9..0x50C (20 bayt)
//     -> canli Engine.exe menusu cizer
// Bizde ayni zincirin istemci ucu: CSPKData (SPKData.cpp) 0x4F9 blogunu okur,
// bu dosya 20 slotu cizer ve tiklamayi ozellige baglar.
//
// Neden yeni dosya: MenuCustom.cpp'teki 15 slotlu menu CBGetMain.bin
// (MAIN_FILE_INFO.Menu[15]) ile besleniyor; SPK paketinde bu dosya YOK
// (docs/24 2e.2). Bu yuzden SPK paketinde ozellik menusu tamamen bos kaliyordu.
// MenuCustom korunur (CBGetMain.bin olan MU paketi icin); SPK paketinde bu cubuk
// devreye girer.
#include "stdafx.h"
#include "SPKMenuBar.h"
#include "SPKData.h"
#include "CBInterface.h"
#include "NewUIBCustomMenu.h"
#include "NewUISystem.h"
#include "Other.h"
#include "Util.h"
#include "CustomEventTime.h"
#include "CustomRanking.h"
#include "WindowClass.h"
#include "CB_DanhHieu.h"
#include "CB_JewelBank.h"
#include "CBChoTroi.h"
#include "CB_AutoResetInfo.h"
#include "CBInterfaceVIPChar.h"
#include "MocDonate.h"
#include "VongQuay.h"

CSPKMenuBar gSPKMenuBar;

// Canli GetEngine.ini yorum sutunundaki etiketler (ASCII karsiligi).
// Kod dosyalari bu projede ASCII tutuluyor; Turkce karakterler WebFont ile cizilir.
static const char* g_SPKMenuLabel[SPK_SLOT_COUNT] =
{
	"Siralama",			// 01
	"Etkinlik Saati",	// 02
	"Relife",			// 03
	"Reset Degisimi",	// 04
	"Danh Hieu",		// 05
	"Cark",				// 06
	"VIP",				// 07
	"Moc Nap",			// 08
	"Sinif Degistirme",	// 09
	"Sifre Degistirme",	// 10
	"Hon Hoan",			// 11
	"X-Shop",			// 12
	"Tu Luyen",			// 13
	"Quan Ham",			// 14
	"Yildiz Secimi",	// 15
	"Mucevher Magazasi",// 16
	"",					// 17 canlida kullanilmiyor
	"",					// 18
	"",					// 19
	"",					// 20
};

// Slotun istemci tarafi karsiligi yazildi mi (kalanlar 3C.3/3C.4 is emri).
static const bool g_SPKMenuHasFeature[SPK_SLOT_COUNT] =
{
	true,	// 01 Ranking      -> gCustomRanking
	true,	// 02 EventTime    -> gCustomEventTime
	true,	// 03 Relife       -> gCBAutoResetInfo
	true,	// 04 ResetChange  -> gCBAutoResetInfo
	true,	// 05 DanhHieu     -> gCBDanhHieu
	true,	// 06 Spin         -> gVongQuay
	true,	// 07 VIP          -> eVip_MAIN + gBInterfaceVIPChar
	true,	// 08 MocNap       -> gMocDonate
	true,	// 09 ChangeClass  -> WindowClass
	true,	// 10 ChangePass   -> gCBChotroi (NhapPass alt penceresi)
	false,	// 11 HonHoan      -> 3C.3
	false,	// 12 X-Shop       -> 3C.3
	false,	// 13 TuLuyen      -> 3C.4
	false,	// 14 QuanHam      -> 3C.4
	false,	// 15 Star         -> 3C.3
	false,	// 16 JewelShop    -> 3C.3
	false,	// 17
	false,	// 18
	false,	// 19
	false,	// 20
};

// Cubuk geometrisi — canli MenuBarLeft/Center/Right tekligi yerine mevcut
// NewUIBCustomMenu buton katmani kullanilir (docs/31 §3: widget katmani hazir).
#define SPK_MENU_BTN_H		16.0f
#define SPK_MENU_BTN_W		92.0f
#define SPK_MENU_GAP		2.0f
#define SPK_MENU_TOP		90.0f	// HUD alti

CSPKMenuBar::CSPKMenuBar()
{
	this->m_Clicked = false;
	this->m_LastSlot = -1;
}

bool CSPKMenuBar::IsReady() const
{
	return gSPKData.m_MenuBlockLoaded;
}

bool CSPKMenuBar::IsEnabled(int slot) const
{
	if (slot < 0 || slot >= SPK_SLOT_COUNT) return false;
	if (gSPKData.m_MenuBlockLoaded == false) return false;

	return (gSPKData.m_MenuButton[slot] != 0);
}

char* CSPKMenuBar::GetSlotLabel(int slot) const
{
	if (slot < 0 || slot >= SPK_SLOT_COUNT) return "";

	if (g_SPKMenuLabel[slot][0] != 0) return (char*)g_SPKMenuLabel[slot];

	// 17-20: canlida kullanilmiyor — bos etiket doner, cizim de yapilmaz.
	return "";
}

bool CSPKMenuBar::HasFeature(int slot) const
{
	if (slot < 0 || slot >= SPK_SLOT_COUNT) return false;

	return g_SPKMenuHasFeature[slot];
}

int CSPKMenuBar::GetEnabledCount() const
{
	int count = 0;

	for (int i = 0; i < SPK_SLOT_COUNT; i++)
	{
		if (this->IsEnabled(i) && g_SPKMenuLabel[i][0] != 0) count++;
	}

	return count;
}

void CSPKMenuBar::GetSlotRect(int slot, float& x, float& y, float& w, float& h) const
{
	int index = 0;

	for (int i = 0; i < slot; i++)
	{
		if (this->IsEnabled(i) && g_SPKMenuLabel[i][0] != 0) index++;
	}

	w = SPK_MENU_BTN_W;
	h = SPK_MENU_BTN_H;

	int total = this->GetEnabledCount();

	float barW = (total * SPK_MENU_BTN_W) + ((total > 0) ? ((total - 1) * SPK_MENU_GAP) : 0);

	x = (DisplayWin - barW) / 2 + (index * (SPK_MENU_BTN_W + SPK_MENU_GAP));
	y = SPK_MENU_TOP;
}

void CSPKMenuBar::Draw()
{
	if (this->IsReady() == false) return;
	if (this->GetEnabledCount() == 0) return;

	// Yalnizca karakter sahnedeyken cizilir (Interface::Work icinden gelir).
	if (CharacterAttribute == 0) return;

	this->m_Clicked = false;

	for (int i = 0; i < SPK_SLOT_COUNT; i++)
	{
		if (this->IsEnabled(i) == false) continue;
		if (g_SPKMenuLabel[i][0] == 0) continue;	// 17-20 cizilmez

		float x = 0, y = 0, w = 0, h = 0;

		this->GetSlotRect(i, x, y, w, h);

		if (g_pBCustomMenuInfo->DrawButton(x, y, SPK_MENU_BTN_H, 12, this->GetSlotLabel(i), w))
		{
			if (this->m_Clicked == false)
			{
				this->m_Clicked = true;
				this->m_LastSlot = i;
				this->ActionSlot(i);
			}
		}
	}
}

void CSPKMenuBar::ActionSlot(int slot)
{
	if (slot < 0 || slot >= SPK_SLOT_COUNT) return;

	// Ozelligi henuz yazilmamis slot: oyuncuyu yaniltmamak acik mesaj ver.
	if (g_SPKMenuHasFeature[slot] == false)
	{
		char caption[64] = { 0 };
		char message[256] = { 0 };

		strcpy_s(caption, "Axion Mu");
		sprintf_s(message, "%s ozelligi bu sunucu surumunde kapali.", this->GetSlotLabel(slot));

		gInterface.OpenMessageBox(caption, message);

		return;
	}

	switch (slot)
	{
	case SPK_SLOT_RANKING:
		if (gCustomRanking) gCustomRanking->OpenWindow();
		break;

	case SPK_SLOT_EVENTTIME:
		gInterface.Data[eWindowEventTime].OpenClose();
		if (gInterface.Data[eWindowEventTime].OnShow)
		{
			gCustomEventTime.ClearCustomEventTime();
			gCustomEventTime.OpenTestWindow();
		}
		break;

	case SPK_SLOT_RELIFE:
#if(CB_AUTORESETINFO)
		if (gCBAutoResetInfo) gCBAutoResetInfo->OpenWindow();
#endif
		break;

	case SPK_SLOT_RESETCHANGE:
#if(CB_AUTORESETINFO)
		if (gCBAutoResetInfo) gCBAutoResetInfo->OpenWindow();
#endif
		break;

	case SPK_SLOT_DANHHIEU:
#if(DANH_HIEU_NEW == 1)
		gCBDanhHieu.OpenWindow();
#endif
		break;

	case SPK_SLOT_SPIN:
		gVongQuay.OpenVongQuay();
		break;

	case SPK_SLOT_VIP:
		gInterface.Data[eVip_MAIN].OpenClose();
#if(CB_VIP_CHAR)
		if (gBInterfaceVIPChar && gInterface.Data[eVip_MAIN].OnShow)
		{
			gBInterfaceVIPChar->CGSendOpenWinwdowVIP();
		}
#endif
		break;

	case SPK_SLOT_MOCNAP:
		gMocDonate.OpenWindowMocNap();
		break;

	case SPK_SLOT_CHANGECLASS:
		WindowClass.SetVisible(true);
		break;

	case SPK_SLOT_CHANGEPASS:
		// Sifre degistirme ChoTroi penceresinin alt penceresidir (CBChoTroi.cpp:756).
		gCBChotroi.GetOpenChoTroiWinDow();
		gInterface.Data[eWindowNhapPass].OnShow = 1;
		break;

	default:
		break;
	}
}