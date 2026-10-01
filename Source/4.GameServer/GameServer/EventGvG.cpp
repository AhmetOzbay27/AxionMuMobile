#include "stdafx.h"
#include "EventGvG.h"
#include "ServerInfo.h"

// =============================================================================
// 2c.1-B2 (01.10.2026) — CGvGEvent ISKELETI (isinma kalemi)
// Canli kanit: GameServer.exe'de 7 config anahtari (EventGvGSwitch/Npc/NpcMap/
// NpcX/NpcY/MinUsers/MaxUsers — docs/12 §4). Header donor'dan birebir
// (EventGvG.h — struct + genis metot yuzeyi). Govede iskelet:
//   Init/Clear + state machine (BLANK/EMPTY/STAND/START/CLEAN) bos gecisler.
// Tam uygulama (NPC spawn, Dialog, katilim, rank, StartGvG) 2c.1-B2'de —
// donor EventGvG.cpp (1138 satir) o asamada canli config/kanitla denetlenerek
// alinacak (docs/12 §4: donor kodu tek basina "yanliş kaynak" uyarisinda).
// =============================================================================

CGvGEvent gGvGEvent;

CGvGEvent::CGvGEvent()
{
	this->Init();
}

CGvGEvent::~CGvGEvent()
{
}

void CGvGEvent::Init()
{
	this->SetState(GVG_EVENT_STATE_BLANK);

	this->m_RemainTime = 0;
	this->m_StandTime = 0;
	this->m_CloseTime = 0;
	this->m_TickCount = 0;
	this->m_WarningTime = 0;
	this->m_EventTime = 0;
	this->EnterEnabled = 0;
	this->AlarmMinSave = 0;
	this->AlarmMinLeft = 0;
	this->TargetTime = 0;
	this->ReqItemCount = 0;
	this->ReqItemIndex = 0;
	this->ReqItemLevel = 0;
	this->EventMap = -1;
	this->WaitingGate = -1;
	this->StartGate = -1;
	this->MinLevel = 0;
	this->MaxLevel = 0;
	this->MinReset = 0;
	this->MaxReset = 0;
	this->MinMasterReset = 0;
	this->MaxMasterReset = 0;
	this->Coin1 = 0;
	this->Coin2 = 0;
	this->Coin3 = 0;

	this->Clear();
}

void CGvGEvent::Clear()
{
	for (int n = 0; n < MAX_GVGEVENT_GUILD; n++)
	{
		this->Guild[n].Reset();
	}

	for (int n = 0; n < MAX_GVGEVENT_USER; n++)
	{
		this->User[n].Reset();
	}

	this->Winner = -1;
	this->TotalPlayer = 0;
	this->m_GVGStartTime.clear();
}

void CGvGEvent::Load(char* path)
{
	// 2c.1-B2: canli exe'de ayri GvG config dosyasi izi YOK (sadece ServerInfo
	// 7 anahtari kanitli) — dosya kanitlaninca buraya doldurulacak.
}

void CGvGEvent::MainProc()
{
	switch (this->GetState())
	{
	case GVG_EVENT_STATE_BLANK:
		this->ProcState_BLANK();
		break;
	case GVG_EVENT_STATE_EMPTY:
		this->ProcState_EMPTY();
		break;
	case GVG_EVENT_STATE_STAND:
		this->ProcState_STAND();
		break;
	case GVG_EVENT_STATE_START:
		this->ProcState_START();
		break;
	case GVG_EVENT_STATE_CLEAN:
		this->ProcState_CLEAN();
		break;
	default:
		this->SetState(GVG_EVENT_STATE_BLANK);
		break;
	}
}

// --- state gecis gvdeleri (iskelet — tam uygulama 2c.1-B2) ---
void CGvGEvent::ProcState_BLANK()
{
}

void CGvGEvent::ProcState_EMPTY()
{
}

void CGvGEvent::ProcState_STAND()
{
}

void CGvGEvent::ProcState_START()
{
}

void CGvGEvent::ProcState_CLEAN()
{
}

void CGvGEvent::SetState(int state)
{
	this->m_State = state;
}

int CGvGEvent::GetState()
{
	return this->m_State;
}
