// MoveSummon.cpp: implementation of the CMoveSummon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MoveSummon.h"
#include "Log.h"	// Faz 2b.2-N: Reload logları
#include "CastleSiege.h"
#include "DefaultClassInfo.h"
#include "Gate.h"
#include "ItemManager.h"
#include "Map.h"
#include "MemScript.h"
#include "Util.h"

CMoveSummon gMoveSummon;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveSummon::CMoveSummon() // OK
{
	this->m_MoveSummonInfo.clear();
}

CMoveSummon::~CMoveSummon() // OK
{

}

void CMoveSummon::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_MoveSummonInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			MOVE_SUMMON_INFO info;

			info.Map = lpMemScript->GetNumber();

			info.X = lpMemScript->GetAsNumber();

			info.Y = lpMemScript->GetAsNumber();

			info.TX = lpMemScript->GetAsNumber();

			info.TY = lpMemScript->GetAsNumber();

			info.MinLevel = lpMemScript->GetAsNumber();

			info.MaxLevel = lpMemScript->GetAsNumber();

			info.MinReset = lpMemScript->GetAsNumber();

			info.MaxReset = lpMemScript->GetAsNumber();

			info.AccountLevel = lpMemScript->GetAsNumber();

			info.PkMove = lpMemScript->GetAsNumber();

			this->m_MoveSummonInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	// Faz 2b.2-N: yolu sakla (reload icin)
	memset(this->m_Path,0,sizeof(this->m_Path));
	strcpy_s(this->m_Path,path);
}

// Faz 2b.2-N: config'i yeniden yükler (canlı SPK log deseni: 'MoveSummon
// configuration reloaded'). /reload movesummon komutu çağırır; hata durumunda
// mevcut veriler korunur (E-05 deseni).
void CMoveSummon::Reload() // Faz 2b.2-N
{
	if(this->m_Path[0] == 0)
	{
		LogAdd(LOG_RED,"[MoveSummon] Reload skipped - config path not set yet");
		return;
	}

	std::vector<MOVE_SUMMON_INFO> oldInfo;
	oldInfo = this->m_MoveSummonInfo;

	this->Load(this->m_Path);

	if(this->m_MoveSummonInfo.empty())
	{
		this->m_MoveSummonInfo = oldInfo;
		LogAdd(LOG_RED,"[MoveSummon] Reload failed - old data restored (%s)",this->m_Path);
		return;
	}

	LogAdd(LOG_BLUE,"[CMoveSummon] MoveSummon configuration reloaded.");
}

bool CMoveSummon::CheckMoveSummon(LPOBJ lpObj,int map,int x,int y) // OK
{
	if(lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		return 0;
	}

	#if(GAMESERVER_TYPE==1)

	if(map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE && (x > 160 && x < 192 && y > 187 && y < 217))
	{
		return 0;
	}

	#endif

	if(map == MAP_ATLANS && (lpObj->Inventory[8].IsItem() != 0 && (lpObj->Inventory[8].m_Index == GET_ITEM(13,2) || lpObj->Inventory[8].m_Index == GET_ITEM(13,3)))) // Uniria,Dinorant
	{
		return 0;
	}

	if((map == MAP_ICARUS || map == MAP_KANTURU3) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13,3) && lpObj->Inventory[8].m_Index != GET_ITEM(13,37))) // Dinorant,Fenrir
	{
		return 0;
	}

	for(std::vector<MOVE_SUMMON_INFO>::iterator it=this->m_MoveSummonInfo.begin();it != this->m_MoveSummonInfo.end();it++)
	{
		if(it->Map != map)
		{
			continue;
		}

		if((it->X > x || it->TX < x) || (it->Y > y || it->TY < y))
		{
			continue;
		}

		if(it->MinLevel != -1 && lpObj->Level < gGate.GetMoveLevel(lpObj,it->Map,it->MinLevel))
		{
			return 0;
		}

		if(it->MaxLevel != -1 && lpObj->Level > it->MaxLevel)
		{
			return 0;
		}

		if(it->MinReset != -1 && lpObj->Reset < it->MinReset)
		{
			return 0;
		}

		if(it->MaxReset != -1 && lpObj->Reset > it->MaxReset)
		{
			return 0;
		}

		if(lpObj->AccountLevel < it->AccountLevel)
		{
			return 0;
		}		
		
		if(it->PkMove == 0 && lpObj->PKLevel >= 5)
		{
			return 0;
		}

		else
		{
			return 1;
		}
	}

	return 0;
}
