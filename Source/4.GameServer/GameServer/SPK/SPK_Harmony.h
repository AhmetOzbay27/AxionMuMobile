// SPK_Harmony.h: interface for the CustomHarmony class (2c.1-A1, canli kanittan).
//
// Canli kanit (SPK_Harmony.obj / live map 1553-1563 + live_disasm 186732-189123):
//   CustomHarmony::Instance/Load/GetMessage/SetStateInterface/ProcMix/
//   SendListItemPoint/SendInfoItemCache/ProcItemSend/BackItem/Save +
//   SPK_HarmonyProc (SPK GUI editor WndProc — kapsam disi).
//   Config: Data\SPK\CustomHarmony.xml — root Harmony{Enable,PriceType,Price,Rate},
//   Message/Msg(Index,Text), NPC{NPCClass,NPCMap,NPCX,NPCY,NPCDir,Name},
//   OptWeapon/OptStaff/OptItem -> Option{OptIndex,Name,Level,Rate}.
//   Oturum: std::map<int,HM_HAMORNY> (canli map sembolleri); tasma korumasi
//   canli stringi '[DaTaoHoa] Data qua dai !!'.
//   Dispatcher (live ProtocolCore 0x551984 tablo): 0x6F -> SetStateInterface(a,0),
//   0x71 -> ProcMix(a), baska deger -> ProcItemSend(a,Source), ThaoTac==-1 korumasi
//   -> BackItem(a,ThaoTac) — bizim 0xD3 switch konvansiyonuyla ayni aile
//   (SauChangeItem 0x6A/0x6B/0x6C deseni).

#pragma once
#include "StdAfx.h"
#include "User.h"
#include "Item.h"
#include "Protocol.h"

#define HARMONY_MAX_SESSION		50	// oturum siniri (canli '[DaTaoHoa] Data qua dai !!' korumasi)
#define HARMONY_INVENTORY_START	12	// INVENTORY_WEAR_SIZE — envanter taramasi wear bolgesini atlar
#define HARMONY_JEWEL_INDEX		GET_ITEM(14, 41)	// Jewel of Harmony (JewelMix.cpp:44 ayni)

// canli OptIndex baytlari sema dogrulamasi icin (canli XML birebir degerler)
#define HARMONY_OPTTYPE_WEAPON	1
#define HARMONY_OPTTYPE_STAFF	2
#define HARMONY_OPTTYPE_ITEM	3

struct HARMONY_MESSAGE
{
	int Index;
	char Text[128];
};

struct HARMONY_OPTION_INFO
{
	int OptIndex;
	char Name[64];
	int Level;
	int Rate;
};

// canli liste paketi (C2 0xD3:0C00C2 + [esi+14h] sonuc alani); bizim aile
// konvansiyonuyla 0xD3:0x24 (BCustomVIPChar SendListItemPoint ayni desen)
struct PMSG_HARMONY_LIST_SEND
{
	PSWMSG_HEAD header;
	int Result;	// kilitli Slot+1 (0 = oturum yok)
};

struct HM_HAMORNY	// canli struct adi (map<int,HM_HAMORNY>)
{
	int Slot;		// kullanici envanterindeki yuva (oturumda kilitli item)
	int Value;		// secilen opt degeri (mix sonucu icin ayirt edici)
	HM_HAMORNY() { this->Init(); }
	void Init() { this->Slot = -1; this->Value = 0; }
};

class CustomHarmony
{
public:
	static CustomHarmony* Instance();	// canli singleton (cpp'de tanimli)

	void Load(char* path);
	void Save(char* path);	// canli yuzey birebir (SPK editor kaydi; calisma aninda sadece Load kullaniyor)
	char* GetMessage(int index);	// canli GetMessageA
	void SetStateInterface(int aIndex, int State);
	void ProcItemSend(int aIndex, int Source);
	void BackItem(int aIndex, int Slot);
	void ProcMix(int aIndex);
	void SendListItemPoint(int aIndex, int State);
	void SendInfoItemCache(int aIndex);
	void ClearSession(int aIndex);
	HM_HAMORNY* GetSession(int aIndex);

	std::map<int, HM_HAMORNY> m_HarmonyMap;	// canli: map<int,HM_HAMORNY>
	HARMONY_OPTION_INFO m_OptWeapon[16];
	HARMONY_OPTION_INFO m_OptStaff[16];
	HARMONY_OPTION_INFO m_OptItem[16];
	int m_OptWeaponCount;
	int m_OptStaffCount;
	int m_OptItemCount;
private:
	int m_Enable;
	int m_PriceType;	// 1=WcoinC, 2=WcoinP, 3=GoblinPoint, 0=Zen
	int m_Price;
	int m_Rate;
	int m_NPCClass;
	int m_NPCMap;
	int m_NPCX;
	int m_NPCY;
	int m_NPCDir;
	char m_NPCName[64];
	std::map<int, HARMONY_MESSAGE> m_MessageMap;

	int PayPrice(LPOBJ lpObj);
};

extern CustomHarmony gCustomHarmony;
