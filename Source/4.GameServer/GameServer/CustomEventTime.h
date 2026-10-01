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
#define MAX_EVENTTIME  42

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
	void GCReqEventTime(int Index, PMSG_CUSTOM_EVENTTIME_RECV* pMsg);
private:
	CUSTOM_EVENTTIME_DATA r_Data[MAX_EVENTTIME];
};
extern CCustomEventTime gCustomEventTime;
// ---
