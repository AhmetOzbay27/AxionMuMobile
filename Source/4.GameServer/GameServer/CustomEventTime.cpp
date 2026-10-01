#include "stdafx.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ServerDisplayer.h"
#include "Util.h"
#include "Message.h"
#include "User.h"
#include "Path.h"
#include "ServerInfo.h"
#include "CustomEventTime.h"
#include "MemScript.h"
#include "CEventName.h"
#include "Notice.h"

CCustomEventTime gCustomEventTime;

CCustomEventTime::CCustomEventTime() // E-06 (Faz 2b.2-O): declare h'te duruyor (donor h ile uyum)
{
	// E-06 (kalan parça): canlı store init — Load çağrılana kadar kapalı
	this->m_Enable = 0;
	memset(this->m_EventInfo,0,sizeof(this->m_EventInfo));
	memset(this->m_SlotUsed,0,sizeof(this->m_SlotUsed));
	memset(this->m_RemainTime,0,sizeof(this->m_RemainTime));
}

// E-06 (kalan parça): canlı Load (0x473C80) birebir şema — root SPK → Enable,
// Message/Msg (Index+Text, map<int,MESSAGE_INFO_EVENTTIME>), EventTime/Event
// (Slot 0..41 sınırı, 46B: Name/Map/Gate/Status). /reload custommonsterskill
// desenine uygun şekilde ServerInfo ReadEventInfo'dan çağrılır.
void CCustomEventTime::Load(char* path) // E-06
{
	this->m_Enable = 0;
	this->m_MessageMap.clear();
	memset(this->m_EventInfo,0,sizeof(this->m_EventInfo));
	memset(this->m_SlotUsed,0,sizeof(this->m_SlotUsed));
	memset(this->m_RemainTime,0,sizeof(this->m_RemainTime));

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node root = file.child("SPK");
	this->m_Enable = root.attribute("Enable").as_int(0);

	for (pugi::xml_node msg = root.child("Message").child("Msg"); msg; msg = msg.next_sibling("Msg"))
	{
		int index = msg.attribute("Index").as_int(-1);
		if (index < 0) continue;
		MESSAGE_INFO_EVENTTIME info;
		memset(&info,0,sizeof(info));
		info.Index = index;
		strcpy_s(info.Text,msg.attribute("Text").as_string(""));
		this->m_MessageMap[index] = info;
	}

	for (pugi::xml_node ev = root.child("EventTime").child("Event"); ev; ev = ev.next_sibling("Event"))
	{
		int slot = ev.attribute("Slot").as_int(-1);
		if (slot < 0 || slot >= MAX_EVENTTIME) continue;	// canlı: 0..0x29 sınırı
		this->m_EventInfo[slot].Slot = slot;
		strcpy_s(this->m_EventInfo[slot].Name,ev.attribute("Name").as_string(""));
		this->m_EventInfo[slot].Map = ev.attribute("Map").as_int(0);
		this->m_EventInfo[slot].Gate = ev.attribute("Gate").as_int(0);
		this->m_EventInfo[slot].Status = ev.attribute("Status").as_int(0);
		this->m_SlotUsed[slot] = 1;	// canlı: this+0x185C+slot bayt
	}
}

// E-06 (kalan parça): canlı GetEventTime (0x4742A0) — BYTE arg switch 0..7 →
// 8 event-main global sayaç; >7 → (slot-8) < 0x1E tablo (canlı 0x9AE5F8,
// taban *(this+0x10)=8 → XML'de Invasion slot 8'den başlıyor).
// Bizim köprü: 0..7 → gEventName.GlobalRemainTime(slot) — 2b.2-O'da bağlanan
// 11 yazıcının beslediği global sayaç deposu (BC=0/DS=1/CC=2/IL=3/…
// canlı yazar adresleriyle aynı semantik: 9AE5DC←BC bölgesi, 9AE5E4←CC bölgesi).
// 8..21 invasion tablosu m_RemainTime — InvasionManager paritesi 2c'de doldurur.
// NOT: donor index seti (QUIZ/BONUS/TVT/…) ile canlı ana-event seti
// (CTCMini/FFA/KingMu/DivineWar/GuildBoss) birebir aynı değil; tam birleşim
// canlı GCReqEventTime paket çözümüyle (disasm 146450+) yapılacak.
int CCustomEventTime::GetEventTime(BYTE slot) // E-06
{
	if (slot < 8)
	{
		return gEventName.GlobalRemainTime(slot);
	}

	int index = ((int)slot) - 8;
	if (index >= 0 && index < MAX_EVENTTIME_TABLE)
	{
		return this->m_RemainTime[index];
	}

	return 0;
}

CCustomEventTime::~CCustomEventTime() // E-06
{
}

//ThangCuoi Fix Bảng H Sự Kiện
int MaxPerPage = 14;

void CCustomEventTime::GCReqEventTime(int Index, PMSG_CUSTOM_EVENTTIME_RECV* lpMsg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 2)

	if (gServerInfo.m_CustomEventTimeSwitch == 0) return;
	if (gObjIsConnected(Index) == false) return;

	int GetPage = lpMsg->Page;
	int TotalEvent = gEventName.m_SendClientDataEventTime.size();
	if (TotalEvent <= 0) return;

	int needSkip = GetPage * MaxPerPage;
	int skipped = 0;
	int startIndex = -1;

	for (int i = 0; i < TotalEvent; i++)
	{
		int CatE = gEventName.m_SendClientDataEventTime[i].switch_on;
		int IndexE = gEventName.m_SendClientDataEventTime[i].m_Key;
		int t = gEventName.GetTimeEventSwitch(CatE, IndexE);
		if (t == -1) continue;
		if (skipped >= needSkip)
		{
			startIndex = i;
			break;
		}
		skipped++;
	}

	if (startIndex == -1) return;

	BYTE send[2048];
	PMSG_CUSTOM_EVENTTIME_SEND pMsg;
	pMsg.header.set(0xF3, 0xE8, 0);

	int size = sizeof(pMsg);
	pMsg.count = 0;
	pMsg.MaxList = 0;

	int totalValid = 0;
	for (int i = 0; i < TotalEvent; i++)
	{
		int CatE = gEventName.m_SendClientDataEventTime[i].switch_on;
		int IndexE = gEventName.m_SendClientDataEventTime[i].m_Key;
		if (gEventName.GetTimeEventSwitch(CatE, IndexE) != -1)
			totalValid++;
	}
	pMsg.MaxList = totalValid;

	memcpy(send, &pMsg, sizeof(pMsg));

	CUSTOM_EVENTTIME_DATA info;
	int collected = 0;
	for (int i = startIndex; i < TotalEvent && collected < MaxPerPage; i++)
	{
		int CatE = gEventName.m_SendClientDataEventTime[i].switch_on;
		int IndexE = gEventName.m_SendClientDataEventTime[i].m_Key;
		int t = gEventName.GetTimeEventSwitch(CatE, IndexE);
		if (t == -1) continue;

		info.index = i;
		info.time = t;
		info.NumberGate = gEventName.m_SendClientDataEventTime[i].GetGate();
		memcpy(&info.NameEvent, gEventName.m_SendClientDataEventTime[i].GetName(), sizeof(info.NameEvent));
		memcpy(&info.DesString, gEventName.m_SendClientDataEventTime[i].GetDes(), sizeof(info.DesString));

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		collected++;
		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(Index, send, size);

#endif
}
