#include "stdafx.h"
#include "Util.h"
#include "Log.h"	// Faz 2b.2-R: Reload logları
#include "ItemManager.h"
#include "Message.h"
#include "User.h"
#include "Path.h"
#include "MemScript.h"
#include "CustomMonsterSkill.h"

CCustomMonsterSkill gCustomMonsterSkill;

void CCustomMonsterSkill::Load(char* path){

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

	for(int n=0;n < MAX_MONSTER_SKILL;n++)
	{
		this->m_Monster_Skill[n];
	}

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
				if(section == 0)
				{
					if(strcmp("end",lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					this->m_Monster_Skill[this->m_count].m_MonsterClass = lpMemScript->GetNumber();

					this->m_Monster_Skill[this->m_count].m_Skill1 = lpMemScript->GetAsNumber();

					this->m_Monster_Skill[this->m_count].m_Skill2 = lpMemScript->GetAsNumber();

					this->m_count++;
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	// Faz 2b.2-R: yolu sakla (reload icin)
	memset(this->m_Path,0,sizeof(this->m_Path));
	strcpy_s(this->m_Path,path);
}

// Faz 2b.2-R: config'i yeniden yükler (canlı SPK log deseni: '[SPK]
// CustomMonsterSkill configuration saved and reloaded'). /reload
// custommonsterskill komutu çağırır; hata durumunda mevcut veriler korunur
// (E-05 deseni).
void CCustomMonsterSkill::Reload() // Faz 2b.2-R
{
	if(this->m_Path[0] == 0)
	{
		LogAdd(LOG_RED,"[SPK] CustomMonsterSkill Reload skipped - config path not set yet");
		return;
	}

	CUSTOM_MONSTER_SKILL oldInfo[MAX_MONSTER_SKILL];
	int oldCount = this->m_count;
	memcpy(oldInfo,this->m_Monster_Skill,sizeof(oldInfo));

	this->Load(this->m_Path);

	if(this->m_count == 0)
	{
		memcpy(this->m_Monster_Skill,oldInfo,sizeof(oldInfo));
		this->m_count = oldCount;
		LogAdd(LOG_RED,"[SPK] CustomMonsterSkill Reload failed - old data restored (%s)",this->m_Path);
		return;
	}

	LogAdd(LOG_BLUE,"[SPK] CustomMonsterSkill configuration saved and reloaded");
}

CUSTOM_MONSTER_SKILL* CCustomMonsterSkill::GetSkillMonster(int MonsterClass){

	for(int n=0;n < MAX_MONSTER_SKILL;n++)
	{
		if(this->m_Monster_Skill[n].m_MonsterClass == MonsterClass)
		{
			return &this->m_Monster_Skill[n];
		}
	}
	return 0;
}