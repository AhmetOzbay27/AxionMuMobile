// EventMainManager.h: interface for the EventMainManager class (2c.1-B1).
//
// CANLI KANIT (SPK_EventMainManager.obj — live map 3207-3293 + exe stringler):
//  - Editor yuzeyi: DialogProc/LoadDetails/Save ailesi (CC/DS/Invasion/
//    CTCMini/SkyEvent/Item380/ItemDrop/ItemMove/ItemOption/ItemStack/MocNap)
//    — SPK GUI editor, GameServer runtime kapsami disi.
//  - Config yukleyici izleri: canli .ini semalari bizim MEM-script .dat
//    semalariyla birebir (BloodCastle/ChaosCastle/DevilSquare/
//    IllusionTemple: "0 WarningTime/NotifyTime/EventTime/CloseTime",
//    "1 Year/Month/Day/DoW/Hour/Minute/Second", "2 Level/ExperienceTable1/2",
//    "3 Level/MoneyTable1/2", GateNpcLife/StatueNpcLife, Enable/PKCanJoin/
//    EnableQuest/KillCount, BlowUserRate/ExpRank%d/MoneyRank%d — exe string
//    anahtarlari SPK_EventMainManager.obj'te); canli yollar .ini
//    (Event\BloodCastle.ini x1 ... bizim .dat — fark kapatildi).
//  - Canli stringler: "Create New", "//WarningTime", "NotifyTime", "//Year",
//    "DoW", "2", "//Level", "ExperienceTable1/2", "3", "MoneyTable1/2",
//    "4", "GateNpcLife", "StatueNpcLife", "5", "6", "//Enable", "PKCanJoin",
//    "EnableQuest", "KillCount", "//Reward", "3", "BlowUserRate",
//    "ExpRank%d", "Mone" (MoneyRank%d) — hepsi SPK_EventMainManager.obj.
//  - SkyEvent (EventMainManager'in tasidigi runtime event): canli
//    Event\SkyEvent\Config.xml (EventStage/EventTime/EventWin semalari okundu)
//    + Monster.ini (bolumlu monster yerlesim listesi); yollar canli exe'de x1.
//    Canli map: map<int,vector<SKYEVENT_MONSTER_DATA>> + SKYEVENT_REWARD_DATA.
//
// Bu iskelet: SkyEvent runtime event'i + config yolu paritesi (.dat -> .ini)
// + diger event'lerin tasiyicisi olacak merkezi yapi. Editor (DialogProc
// ailesi) kapsam disi — SPK GUI araci.

#pragma once

#include "User.h"

#define MAX_SKYEVENT_STAGE			3
#define MAX_SKYEVENT_WIN			5
#define MAX_SKYEVENT_MONSTER		200

enum eSkyEventState
{
	SKYEVENT_STATE_BLANK = 0,
	SKYEVENT_STATE_EMPTY = 1,
	SKYEVENT_STATE_STAND = 2,
	SKYEVENT_STATE_START = 3,
	SKYEVENT_STATE_CLEAN = 4,
};

// canli exe string kaniti: "2" bolumu "//Level ExperienceTable1 ExperienceTable2"
// (BC/CC/DS paylasimi) — editor enum etiketleri
enum eEventMainSection
{
	EVENT_MAIN_SECTION_TIME = 0,		// WarningTime/NotifyTime/EventTime/CloseTime
	EVENT_MAIN_SECTION_SCHEDULE = 1,	// Year/Month/Day/DoW/Hour/Minute/Second
	EVENT_MAIN_SECTION_EXP = 2,			// Level/ExperienceTable1/ExperienceTable2
	EVENT_MAIN_SECTION_MONEY = 3,		// Level/MoneyTable1/MoneyTable2
	EVENT_MAIN_SECTION_NPC = 4,			// GateNpcLife/StatueNpcLife
	EVENT_MAIN_SECTION_RULE = 5,		// Enable/PKCanJoin/EnableQuest/KillCount
	EVENT_MAIN_SECTION_REWARD = 6,		// BlowUserRate/ExpRank%d/MoneyRank%d
};

struct SKYEVENT_MONSTER_DATA	// canli struct adi (map<int,vector<SKYEVENT_MONSTER_DATA>>)
{
	int Stage;
	int MonsterClass;
	int Map;
	int X;
	int Y;
	int Dir;
	int RespawnTime;
};

struct SKYEVENT_REWARD_DATA	// canli struct adi
{
	int WinLevel;		// iLevel
	int LevelMin;
	int LevelMax;
	int ExtraExpStage[MAX_SKYEVENT_STAGE];
	int ItemType;
	int ItemIndex;
	int ItemLevel;
	int ItemDur;
	int ItemLuck;
	int ItemSkill;
	int ItemOpt;
	int ItemExc;
	int WcoinC;
	int WcoinP;
	int GPoint;
};

struct SKYEVENT_STAGE_DATA	// canli XML: EventStage/Stage
{
	int Enabled;
	int StageMin[MAX_SKYEVENT_STAGE];
};

struct SKYEVENT_START_TIME
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
};

class EventMainManager
{
public:
	EventMainManager();
	virtual ~EventMainManager();
	void Init();
	void Clear();
	void Load();				// merkez yukleyici: SkyEvent + yol paritesi (canli .ini)
	void LoadSkyEvent();		// canli Event\SkyEvent\Config.xml + Monster.ini
	void MainProc();			// gObjEventRunProc kancasi (EventGvG deseni)
	void ProcState_BLANK();
	void ProcState_EMPTY();
	void ProcState_STAND();
	void ProcState_START();
	void ProcState_CLEAN();
	void SetState(int state);
	int GetState();
	void CheckSync();
private:
	int m_State;
	int m_RemainTime;
	int m_TickCount;
	int m_WarningTime;
	int m_NotifyTime;
	int m_EventTime;
	int m_CloseTime;
	SKYEVENT_STAGE_DATA m_Stage;
	std::vector<SKYEVENT_START_TIME> m_SkyEventStartTime;
	std::vector<SKYEVENT_REWARD_DATA> m_SkyEventReward;
	std::map<int, std::vector<SKYEVENT_MONSTER_DATA>> m_SkyEventMonster;	// canli: map<int,vector<SKYEVENT_MONSTER_DATA>>
	DWORD m_TickCountLast;
};

extern EventMainManager gEventMainManager;
