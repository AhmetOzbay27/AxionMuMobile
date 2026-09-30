// MonsterSetBase.cpp: implementation of the CMonsterSetBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterSetBase.h"
#include "MapServerManager.h"
#include "MemScript.h"
#include "Util.h"

CMonsterSetBase gMonsterSetBase;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterSetBase::CMonsterSetBase() // OK
{
	this->m_count = 0;
}

CMonsterSetBase::~CMonsterSetBase() // OK
{

}

void CMonsterSetBase::Load(char* path) // OK
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

	this->m_count = 0;

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while(true)
			{
				if(strcmp("end",lpMemScript->GetAsString()) == 0)
				{
					break;
				}

				MONSTER_SET_BASE_INFO info;

				memset(&info,0,sizeof(info));

				info.Type = section;

				info.MonsterClass = lpMemScript->GetNumber();

				info.Map = lpMemScript->GetAsNumber();

				info.Dis = lpMemScript->GetAsNumber();

				info.X = lpMemScript->GetAsNumber();

				info.Y = lpMemScript->GetAsNumber();

				if(section == 1 || section == 3)
				{
					info.TX = lpMemScript->GetAsNumber();
					info.TY = lpMemScript->GetAsNumber();
				}
				else if(section == 2)
				{
					info.X = (info.X-3)+GetLargeRand()%7;
					info.Y = (info.Y-3)+GetLargeRand()%7;
				}

				info.Dir = lpMemScript->GetAsNumber();

				if(section == 1 || section == 3)
				{
					int count = lpMemScript->GetAsNumber();

					if(section == 3)
					{
						info.Value = lpMemScript->GetAsNumber();
					}

					for(int n=0;n < count;n++)
					{
						this->SetInfo(info);
					}
				}
				else
				{
					this->SetInfo(info);
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CMonsterSetBase::SetInfo(MONSTER_SET_BASE_INFO info) // OK
{
	if(this->m_count < 0 || this->m_count >= MAX_MSB_MONSTER)
	{
		return;
	}
	
	if(gMapServerManager.CheckMapServer(info.Map) == 0)
	{
		return;
	}

	info.Dir = ((info.Dir==-1)?(GetLargeRand()%8):info.Dir);

	info.index = this->m_count;	// 2b.2-B: donor .index alani (gObjSetPosMonster(map, info.index) icin)
	this->m_MonsterSetBaseInfo[this->m_count++] = info;
}

bool CMonsterSetBase::GetPosition(int index,short map,short* ox,short* oy) // OK
{
	if(index < 0 || index >= MAX_MSB_MONSTER)
	{
		return 0;
	}

	MONSTER_SET_BASE_INFO* lpInfo = &this->m_MonsterSetBaseInfo[index];
	
	if(lpInfo->Type == 0 || lpInfo->Type == 4)
	{
		(*ox) = lpInfo->X;
		(*oy) = lpInfo->Y;
		return 1;
	}
	else if(lpInfo->Type == 1 || lpInfo->Type == 3)
	{
		return this->GetBoxPosition(map,lpInfo->X,lpInfo->Y,lpInfo->TX,lpInfo->TY,ox,oy);
	}
	else if(lpInfo->Type == 2)
	{
		return this->GetBoxPosition(map,(lpInfo->X-3),(lpInfo->Y-3),(lpInfo->X+3),(lpInfo->Y+3),ox,oy);
	}

	return 0;
}

bool CMonsterSetBase::GetBoxPosition(int map,int x,int y,int tx,int ty,short* ox,short* oy) // OK
{
	for(int n=0;n < 100;n++)
	{
		int subx = tx-x;
		int suby = ty-y;

		subx = ((subx<1)?1:subx);
		suby = ((suby<1)?1:suby);

		subx = x+(GetLargeRand()%subx);
		suby = y+(GetLargeRand()%suby);

		if(gMap[map].CheckAttr(subx,suby,1) == 0 && gMap[map].CheckAttr(subx,suby,4) == 0 && gMap[map].CheckAttr(subx,suby,8) == 0)
		{
			(*ox) = subx;
			(*oy) = suby;
			return 1;
		}
	}

	return 0;
}

void CMonsterSetBase::SetBoxPosition(int index,int map,int x,int y,int tx,int ty) // OK
{
	if(index < 0 || index >= MAX_MSB_MONSTER)
	{
		return;
	}

	MONSTER_SET_BASE_INFO* lpInfo = &this->m_MonsterSetBaseInfo[index];

	lpInfo->Map = map;
	lpInfo->X = x;
	lpInfo->Y = y;
	lpInfo->TX = tx;
	lpInfo->TY = ty;
}

// ===== 2b.2-B: array-tabanli GetMonsterMap uyarlamasi (donor std::map modelinin islev esdegeri) =====
std::vector<MONSTER_SET_BASE_INFO> CMonsterSetBase::GetMonsterMap(int _map)
{
	std::vector<MONSTER_SET_BASE_INFO> list;
	for (int n = 0; n < this->m_count; n++)
	{
		if (this->m_MonsterSetBaseInfo[n].Map == _map)
		{
			list.push_back(this->m_MonsterSetBaseInfo[n]);
		}
	}
	return list;
}

MONSTER_SET_BASE_INFO* CMonsterSetBase::GetMonsterMap(int _map, int _index)
{
	for (int n = 0; n < this->m_count; n++)
	{
		if (this->m_MonsterSetBaseInfo[n].Map == _map && this->m_MonsterSetBaseInfo[n].index == _index)
		{
			return &this->m_MonsterSetBaseInfo[n];
		}
	}
	return NULL;
}

int CMonsterSetBase::GetMonsterMapCount(int _map)
{
	int count = 0;
	for (int n = 0; n < this->m_count; n++)
	{
		if (this->m_MonsterSetBaseInfo[n].Map == _map)
		{
			count++;
		}
	}
	return count;
}

MONSTER_SET_BASE_INFO* CMonsterSetBase::GetMonsterMapAt(int _map, int n)
{
	int found = 0;
	for (int i = 0; i < this->m_count; i++)
	{
		if (this->m_MonsterSetBaseInfo[i].Map == _map)
		{
			if (found == n)
			{
				return &this->m_MonsterSetBaseInfo[i];
			}
			found++;
		}
	}
	return NULL;
}
