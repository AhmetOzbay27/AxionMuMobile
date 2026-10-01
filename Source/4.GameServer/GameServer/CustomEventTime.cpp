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
