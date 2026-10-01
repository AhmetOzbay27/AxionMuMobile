// CustomEventTime.h: interface for the CCustomEventTime class (Faz 2b.2-O).
//
// Canlı SPK hattı: event-listesi CEventName modülünden (Event/EventName.xml →
// CEventName::OpenFile, ServerInfo) beslenir; bu sınıf yalnızca istemciye
// sayfalı event-saat listesi gönderir (donör 99-satırlık GCReqEventTime).
// Eski bizim InvasionManager/Arena kaynaklı LoadData/AddDataEventsTime ve
// CustomEventTime.xml LoadFileXML hattı kaldırıldı (donör hattıyla çakışıyordu;
// canlıda CustomEventTime.xml yok — Data/Event/EventTime.xml canlı
// EventMain/isteme formatı, docs/09 E-06 notu).

#include "Protocol.h"
#include <map>
#define MAX_EVENTTIME  42
#define MAX_EVENTTIME_TABLE 30	// E-06: canlı GetEventTime tablo sınırı (0x1E)

// E-06 (kalan parça): canlı MESSAGE_INFO_EVENTIME — map<int,struct> değeri (260B: int + Text[0x100])
struct MESSAGE_INFO_EVENTTIME
{
	int Index;
	char Text[256];
};

// E-06 (kalan parça): canlı Event elementi (46B: index+name[30]+map+gate+status — disasm imul 2Eh)
struct EVENT_INFO_EVENTTIME
{
	int Slot;
	char Name[30];
	int Map;
	int Gate;
	int Status;
};

struct CUSTOM_EVENTTIME_DATA
{
	int index;
	char NameEvent[30];
	char DesString[90];
	int NumberGate;
	int time;
};

//**********************************************//
//********** GameServer -> Cliente    **********//
//**********************************************//

struct PMSG_CUSTOM_EVENTTIME_SEND
{
	PSWMSG_HEAD header;
	int MaxList;
	int count;
};

//**********************************************//
//********** Cliente -> GameServer    **********//
//**********************************************//

struct PMSG_CUSTOM_EVENTTIME_RECV
{
	PSBMSG_HEAD header; // C1:BF:51
	BYTE Page;
};
// ---
class CCustomEventTime
{
public:
	CCustomEventTime();
	virtual ~CCustomEventTime();
	void Load(char* path);	// E-06 (kalan parça): canlı Event\EventTime.xml (SPK/Enable/Message/EventTime) — live 0x473C80
	int GetEventTime(BYTE slot);	// E-06 (kalan parça): canlı 0x4742A0 — switch 0..7 → 8 global, >7 → tablo
	void GCReqEventTime(int Index, PMSG_CUSTOM_EVENTTIME_RECV* pMsg);
private:
	CUSTOM_EVENTTIME_DATA r_Data[MAX_EVENTTIME];
	// E-06: canlı store (obj +0x185C slot-bayt dizisi, +0x1886 42×46B event dizisi)
	int m_Enable;
	std::map<int,MESSAGE_INFO_EVENTTIME> m_MessageMap;
	EVENT_INFO_EVENTTIME m_EventInfo[MAX_EVENTTIME];
	BYTE m_SlotUsed[MAX_EVENTTIME];
	int m_RemainTime[MAX_EVENTTIME_TABLE];	// invasion tablosu (slot-8; live 0x9AE5F8 karşılığı)
};
extern CCustomEventTime gCustomEventTime;
// ---
