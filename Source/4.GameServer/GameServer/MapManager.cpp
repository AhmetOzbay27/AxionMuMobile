// MapManager.cpp: implementation of the CMapManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapManager.h"
#include "CustomPKFree.h"	// SPK (Faz 2b.2-M): gPKFree PK-zona zinciri (bizim metodun bagimliligi)
#include "MemScript.h"
#include "ServerInfo.h"
#include "Util.h"

CMapManager gMapManager;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapManager::CMapManager() // OK
{
	this->m_MapManagerInfo.clear();
}

CMapManager::~CMapManager() // OK
{

}

void CMapManager::Load(char* path) // OK
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

	this->m_MapManagerInfo.clear();

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

			MAP_MANAGER_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpMemScript->GetNumber();

			info.NonPK = lpMemScript->GetAsNumber();

			info.ViewRange = lpMemScript->GetAsNumber();

			info.ExperienceRate = lpMemScript->GetAsNumber();

			info.ItemDropRate = lpMemScript->GetAsNumber();

			info.ExcItemDropRate = lpMemScript->GetAsNumber();

			info.SetItemDropRate = lpMemScript->GetAsNumber();

			info.SocketItemDrop = lpMemScript->GetAsNumber();

			#if(GAMESERVER_UPDATE>=603)

			info.HelperEnable = lpMemScript->GetAsNumber();

			#else

			info.HelperEnable = 0;

			#endif

			#if(GAMESERVER_UPDATE>=501)

			info.GensBattle = lpMemScript->GetAsNumber();

			#else

			info.GensBattle = 0;

			#endif

			info.CustomAttack = lpMemScript->GetAsNumber();

			info.CustomStore = lpMemScript->GetAsNumber();

			info.CustomPick = lpMemScript->GetAsNumber();

			info.PkDropItem = lpMemScript->GetAsNumber();

			// SPK (Faz 2b.2-M) canli MapManager.txt kolon sirasi: ... CustAtt CustStore CustPick PkDrop Trade(AllowTrade) DeathGate "Name"
			info.AllowTrade = lpMemScript->GetAsNumber();

			info.DeathGate = lpMemScript->GetAsNumber();

			info.AllowTradeSafe = 0;	// SPK: canli dosyada kolon yok (donor sirasindaki fazlalik); getter 0 dondurur

			info.PartyEnable = 1;	// SPK: canli dosyada kolon yok; parti her haritada acik (bizim onceki 12-kolon yuklemiyle ayni etki)

			strcpy_s(info.Name,lpMemScript->GetAsString());

			this->m_MapManagerInfo.insert(std::pair<int,MAP_MANAGER_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

int CMapManager::GetMapNonPK(int index,LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		//pkfree
		if(!gPKFree.CheckPKZona(index,lpObj->X,lpObj->Y) && !gPKFree.CheckPKZona(index,lpTarget->X,lpTarget->Y)){
			return gServerInfo.m_NonPK;
		}else {
			return 0;
		}
	}
	else
	{
		//pkfree
		if(!gPKFree.CheckPKZona(index,lpObj->X,lpObj->Y) && !gPKFree.CheckPKZona(index,lpTarget->X,lpTarget->Y)){
			return ((it->second.NonPK==-1)?gServerInfo.m_NonPK:((it->second.NonPK==1)?1:0));
		}else {
			return 0;
		}
		
	}
}

// SPK (Faz 2b.2-C korunudu; 2b.2-M'de geri eklendi): canli GetEngine/Axion'da gPKFree PK-zona kontrolu Attack zincirinde aktif
int CMapManager::GetMapNonPK(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return gServerInfo.m_NonPK;
	}
	else
	{
		return ((it->second.NonPK==-1)?gServerInfo.m_NonPK:((it->second.NonPK==1)?1:0));
	}
}

int CMapManager::GetMapNonOutlaw(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.NonPK==-1)?0:((it->second.NonPK==2)?1:0));
	}
}

int CMapManager::GetMapViewRange(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 15;
	}
	else
	{
		return ((it->second.ViewRange==-1)?15:it->second.ViewRange);
	}
}

int CMapManager::GetMapExperienceRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ExperienceRate==-1)?100:it->second.ExperienceRate);
	}
}

int CMapManager::GetMapMasterExperienceRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ExperienceRate==-1)?100:it->second.ExperienceRate);
	}
}

int CMapManager::GetMapItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 100;
	}
	else
	{
		return ((it->second.ItemDropRate==-1)?100:it->second.ItemDropRate);
	}
}

int CMapManager::GetMapExcItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.ExcItemDropRate==-1)?0:it->second.ExcItemDropRate);
	}
}

int CMapManager::GetMapSetItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.SetItemDropRate==-1)?0:it->second.SetItemDropRate);
	}
}

int CMapManager::GetMapSocketItemDrop(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.SocketItemDrop==-1)?0:it->second.SocketItemDrop);
	}
}

int CMapManager::GetMapHelperEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.HelperEnable==-1)?0:it->second.HelperEnable);
	}
}

int CMapManager::GetMapGensBattle(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.GensBattle==-1)?0:it->second.GensBattle);
	}
}

int CMapManager::GetMapCustomAttack(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.CustomAttack==-1)?0:it->second.CustomAttack);
	}
}

int CMapManager::GetMapCustomStore(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.CustomStore==-1)?0:it->second.CustomStore);
	}
}

int CMapManager::GetMapCustomPick(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.CustomPick==-1)?0:it->second.CustomPick);
	}
}

int CMapManager::GetMapPkDropItem(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.PkDropItem==-1)?0:it->second.PkDropItem);
	}
}

int CMapManager::GetMapDeathGate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return 17;
	}
	else
	{
		return ((it->second.DeathGate==-1)?17:it->second.DeathGate);
	}
}


int CMapManager::GetMapAllowTrade(int index) // OK
{
	std::map<int, MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if (it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.AllowTrade == -1) ? 0 : it->second.AllowTrade);
	}
}


int CMapManager::GetMapAllowTradeSafe(int index) // OK
{
	std::map<int, MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if (it == this->m_MapManagerInfo.end())
	{
		return 0;
	}
	else
	{
		return ((it->second.AllowTradeSafe == -1) ? 0 : it->second.AllowTradeSafe);
	}
}



int CMapManager::GetMapPartyEnable(int index) // OK
{
	std::map<int, MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if (it != this->m_MapManagerInfo.end())
	{
		return it->second.PartyEnable;
	}

	return 0;
}


char* CMapManager::GetMapName(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it == this->m_MapManagerInfo.end())
	{
		return "NoName";
	}
	else
	{
		return it->second.Name;
	}
}

bool CMapManager::CheckMap(int index)
{
	std::map<int, MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	return (it != this->m_MapManagerInfo.end());
}
