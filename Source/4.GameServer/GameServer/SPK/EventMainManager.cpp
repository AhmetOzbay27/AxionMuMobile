// EventMainManager.cpp: implementation of the EventMainManager class (2c.1-B1).
//
// Canli SPK_EventMainManager.obj kanitlarina gore iskelet (header'a bak:
// kanit listesi). Bu turda: SkyEvent runtime event (Config.xml + Monster.ini)
// + merkez config yolu paritesi (.dat -> .ini, canli exe stringleriyle).
// Editor (DialogProc/LoadDetails/Save ailesi) SPK GUI araci — kapsam disi.
// Diger event'lerin tasiyicisi: sonraki 2c.1-B adimlarinda tekil event
// yukleyicileri bu manager uzerinden merkezlesir.

#include "stdafx.h"
#include "EventMainManager.h"
#include "Log.h"
#include "Util.h"
#include "MemScript.h"
#include "Path.h"
#include "GameMain.h"

EventMainManager gEventMainManager;

EventMainManager::EventMainManager()
{
	this->Init();
}

EventMainManager::~EventMainManager()
{
}

void EventMainManager::Init()
{
	this->SetState(SKYEVENT_STATE_BLANK);

	this->m_RemainTime = 0;
	this->m_TickCount = 0;
	this->m_WarningTime = 0;
	this->m_NotifyTime = 0;
	this->m_EventTime = 0;
	this->m_CloseTime = 0;
	this->m_TickCountLast = GetTickCount();

	this->m_Stage.Enabled = 0;
	memset(this->m_Stage.StageMin, 0, sizeof(this->m_Stage.StageMin));

	this->Clear();
}

void EventMainManager::Clear()
{
	this->m_SkyEventStartTime.clear();
	this->m_SkyEventReward.clear();
	this->m_SkyEventMonster.clear();
}

// Merkez yukleyici: SkyEvent (EventMainManager'in tasidigi runtime event) +
// canli yol paritesi. Canli exe string kaniti: Event\BloodCastle.ini /
// ChaosCastle.ini / DevilSquare.ini / IllusionTemple.ini / InvasionManager.ini
// (x1) — bizim .dat yollari canlida YOK; bu turda canli Data\Event\*.ini
// deploy agacina kopyalanir (icerik semalari zaten birebir — MemScript
// ayristiricilari ayni kalir, dosya degisir).
// Canli static Load sembolleri (CBloodCastle::Load x7 vs.) canli tabanda
// bu isleri SPK_EventMainManager arkasinda yuruttugunu gosteriyor.
void EventMainManager::Load()
{
	this->LoadSkyEvent();
}

// Canli kanit: Event\SkyEvent\Config.xml (x1) + Event\SkyEvent\Monster.ini
// (x1) — semalar canli dosyalardan okundu (header'a bak).
// Config.xml: SkyEvent/EventStage/Stage{Enabled,StageMin0..2} +
// EventTime/Time{Year,Month,Day,DayOfWeek,Hour,Minute,Second} +
// EventWin/Win{iLevel,LevelMin,LevelMax,ExtraExpStage0..2,ItemType,ItemIndex,
// ItemLevel,ItemDur,ItemLuck,ItemSkill,ItemOpt,ItemExc,WcoinC,WcoinP,GPoint}.
// Monster.ini: bolum numarasi (stage) + satirlar "Class X Y Dir".
void EventMainManager::LoadSkyEvent()
{
	char path[256] = { 0 };

	strcpy_s(path, gPath.GetFullPath("Event\\SkyEvent\\Config.xml"));

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node root = file.child("SkyEvent");

	pugi::xml_node stage = root.child("EventStage").child("Stage");
	if (stage != 0)
	{
		this->m_Stage.Enabled = stage.attribute("Enabled").as_int(0);
		this->m_Stage.StageMin[0] = stage.attribute("StageMin0").as_int(0);
		this->m_Stage.StageMin[1] = stage.attribute("StageMin1").as_int(0);
		this->m_Stage.StageMin[2] = stage.attribute("StageMin2").as_int(0);
	}

	this->m_SkyEventStartTime.clear();

	for (pugi::xml_node time = root.child("EventTime").child("Time"); time; time = time.next_sibling("Time"))
	{
		SKYEVENT_START_TIME info;
		info.Year = time.attribute("Year").as_int(-1);
		info.Month = time.attribute("Month").as_int(-1);
		info.Day = time.attribute("Day").as_int(-1);
		info.DayOfWeek = time.attribute("DayOfWeek").as_int(-1);
		info.Hour = time.attribute("Hour").as_int(-1);
		info.Minute = time.attribute("Minute").as_int(-1);
		info.Second = time.attribute("Second").as_int(-1);
		this->m_SkyEventStartTime.push_back(info);
	}

	this->m_SkyEventReward.clear();

	for (pugi::xml_node win = root.child("EventWin").child("Win"); win; win = win.next_sibling("Win"))
	{
		SKYEVENT_REWARD_DATA info;
		memset(&info, 0, sizeof(info));
		info.WinLevel = win.attribute("iLevel").as_int(0);
		info.LevelMin = win.attribute("LevelMin").as_int(0);
		info.LevelMax = win.attribute("LevelMax").as_int(0);
		info.ExtraExpStage[0] = win.attribute("ExtraExpStage0").as_int(0);
		info.ExtraExpStage[1] = win.attribute("ExtraExpStage1").as_int(0);
		info.ExtraExpStage[2] = win.attribute("ExtraExpStage2").as_int(0);
		info.ItemType = win.attribute("ItemType").as_int(0);
		info.ItemIndex = win.attribute("ItemIndex").as_int(0);
		info.ItemLevel = win.attribute("ItemLevel").as_int(0);
		info.ItemDur = win.attribute("ItemDur").as_int(0);
		info.ItemLuck = win.attribute("ItemLuck").as_int(0);
		info.ItemSkill = win.attribute("ItemSkill").as_int(0);
		info.ItemOpt = win.attribute("ItemOpt").as_int(0);
		info.ItemExc = win.attribute("ItemExc").as_int(0);
		info.WcoinC = win.attribute("WcoinC").as_int(0);
		info.WcoinP = win.attribute("WcoinP").as_int(0);
		info.GPoint = win.attribute("GPoint").as_int(0);
		this->m_SkyEventReward.push_back(info);
	}

	// Monster.ini: canli sema — bolum numarasi, satirlar "Class X Y Dir"
	this->m_SkyEventMonster.clear();

	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, "Event\\SkyEvent\\Monster.ini");
		return;
	}

	if (lpMemScript->SetBuffer(gPath.GetFullPath("Event\\SkyEvent\\Monster.ini")) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			// Bolum (grup) numarasi yalnizca yapisal ayiricidir; her satir kendi Stage kolonunu tasir.
			lpMemScript->GetNumber();

			while (true)
			{
				if (strcmp("end", lpMemScript->GetAsString()) == 0)
				{
					break;
				}

				SKYEVENT_MONSTER_DATA info;
				memset(&info, 0, sizeof(info));
				// Canli sema (Data\Event\SkyEvent\Monster.ini): satir = Stage Class X Y Dir (5 kolon).
				// Ilk kolon, end-kontrolunun okudugu token'dir; kalan 4 kolon GetAsNumber ile okunur.
				info.Stage = lpMemScript->GetNumber();
				info.MonsterClass = lpMemScript->GetAsNumber();
				info.Map = 0;
				info.X = lpMemScript->GetAsNumber();
				info.Y = lpMemScript->GetAsNumber();
				info.Dir = lpMemScript->GetAsNumber();
				info.RespawnTime = 0;

				this->m_SkyEventMonster[info.Stage].push_back(info);
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	LogAdd(LOG_BLUE, "[EventMainManager] SkyEvent loaded (Stage:%d, Win:%d, Monster-groups:%d)", this->m_Stage.Enabled, (int)this->m_SkyEventReward.size(), (int)this->m_SkyEventMonster.size());
}

// gObjEventRunProc kancasi (EventGvG deseni) — state machine iskeleti.
// SkyEvent spawn/zamanlama govdesi sonraki 2c.1-B adiminda (donor
// Sparkly/Sky mantigi canli Config.xml semasiyla denetlenerek).
void EventMainManager::MainProc()
{
	if ((GetTickCount() - this->m_TickCountLast) >= 1000)
	{
		this->m_TickCountLast = GetTickCount();

		switch (this->m_State)
		{
			case SKYEVENT_STATE_BLANK:
				this->ProcState_BLANK();
				break;
			case SKYEVENT_STATE_EMPTY:
				this->ProcState_EMPTY();
				break;
			case SKYEVENT_STATE_STAND:
				this->ProcState_STAND();
				break;
			case SKYEVENT_STATE_START:
				this->ProcState_START();
				break;
			case SKYEVENT_STATE_CLEAN:
				this->ProcState_CLEAN();
				break;
		}
	}
}

void EventMainManager::ProcState_BLANK()
{
	if (this->m_SkyEventStartTime.empty() != 0)
	{
		this->SetState(SKYEVENT_STATE_EMPTY);
	}
}

void EventMainManager::ProcState_EMPTY()
{
	this->CheckSync();
}

void EventMainManager::ProcState_STAND()
{
	this->CheckSync();
}

void EventMainManager::ProcState_START()
{
	this->CheckSync();
}

void EventMainManager::ProcState_CLEAN()
{
	this->CheckSync();
}

void EventMainManager::SetState(int state)
{
	this->m_State = state;
}

int EventMainManager::GetState()
{
	return this->m_State;
}

// canli desen: Schedule kontrolu (Year/Month/Day/DoW/Hour/Minute/Second —
// '*' = -1 joker). SkyEvent Config.xml EventTime tek girdi; kalan zaman
// hesaplanir (WarningTime/NotifyTime canli editor enum'larina uygun).
void EventMainManager::CheckSync()
{
	SYSTEMTIME now;
	GetLocalTime(&now);

	for (std::vector<SKYEVENT_START_TIME>::iterator it = this->m_SkyEventStartTime.begin(); it != this->m_SkyEventStartTime.end(); it++)
	{
		if (it->Year != -1 && it->Year != now.wYear) continue;
		if (it->Month != -1 && it->Month != now.wMonth) continue;
		if (it->Day != -1 && it->Day != now.wDay) continue;
		if (it->DayOfWeek != -1 && it->DayOfWeek != now.wDayOfWeek) continue;
		if (it->Hour == now.wHour && it->Minute == now.wMinute && now.wSecond < ((it->Second == -1) ? 0 : it->Second))
		{
			// eslesme: STAND'a gec (spawn govdesi sonraki adimda)
			if (this->m_State == SKYEVENT_STATE_EMPTY)
			{
				this->SetState(SKYEVENT_STATE_STAND);
			}
		}
	}
}
