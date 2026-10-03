// SPK_MonsterSkill.cpp: implementation of the CCustomMonsterSkill class (2c.1-A2, canli kanittan).
//
// Canli semantik birebir (ayrintili kanit listesi SPK_MonsterSkill.h basliginda):
//   Load   -> vector clear (004A4E90-004A4E9A) + section 0 disi satirlari atla (004A4EDC)
//             + ic dongu 'end' (004A4EE8) + 3 sayi (GetNumber/GetAsNumber/GetAsNumber,
//             004A4F2D/3F/51) + push_back (004A4F5F-004A4F85).
//   GetSkillMonster -> ilk eslesme (004A4FC0-004A4FDE).
//   Instance -> canli Meyers singleton; bizde A1 CustomHarmony deseniyle global + Instance().
//   Reload -> Faz 2b.2-R bizim ekimiz; canli editorun 'yaz + yukle + LogAdd' akisinin
//             komut karsiligi (/reload custommonsterskill) ve canli log stringi birebir:
//             '[SPK] CustomMonsterSkill configuration saved and reloaded.'
//             (exe 2344740, nokta dahil).
//
// 2c.1-A2 E2E (2026-10-02 09:48, LOG/2026-10-02.txt 4523-4543): kayit=16, loader yolu
// ..\Data\SPK\CustomMonsterSkill.txt, GetSkillMonster satirlari OK, mukerrer 719 ilk-eslesme
// = 232, gObjSetMonster(704) -> GetSkill(42)/(264) mevcut, gObjMonsterAttack DURATION dali
// class=704 icin secildi, Reload canli log stringini yazdi. Detay: docs/16.

#include "stdafx.h"
#include "SPK_MonsterSkill.h"
#include "MemScript.h"
#include "Util.h"
#include "Log.h"
#include "Path.h"

CCustomMonsterSkill gCustomMonsterSkill;

CCustomMonsterSkill* CCustomMonsterSkill::Instance()	// canli ?Instance@CCustomMonsterSkill@@SAPAV1@XZ @004A4D00
{
	return &gCustomMonsterSkill;
}

void CCustomMonsterSkill::Load(char* path)		// canli ?Load@CCustomMonsterSkill@@QAEXPAD@Z @004A4D70
{
	this->m_LoadResult = 0;

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

	this->m_Monster_Skill.clear();				// canli 004A4E90-004A4E9A: vector::clear

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)	// canli 004A4EA4-004A4EAE
			{
				break;
			}

			if(lpMemScript->GetNumber() != 0)		// canli 004A4EDC-004A4EE6: section != 0 -> satiri atla
			{
				continue;
			}

			while(true)					// canli 004A4EE8: ic dongu, 'end' gelene kadar
			{
				if(strcmp("end",lpMemScript->GetAsString()) == 0)
				{
					break;
				}

				CUSTOM_MONSTER_SKILL info;

				info.m_MonsterClass = lpMemScript->GetNumber();		// canli 004A4F2D
				info.m_Skill1 = lpMemScript->GetAsNumber();		// canli 004A4F3F
				info.m_Skill2 = lpMemScript->GetAsNumber();		// canli 004A4F51

				this->m_Monster_Skill.push_back(info);			// canli 004A4F5F-004A4F85 (0Ch adim)
			}
		}

		this->m_LoadResult = 1;
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());	// canli __catch$ yolu 004A4F8A-004A4FB2
	}

	delete lpMemScript;

	memset(this->m_Path,0,sizeof(this->m_Path));		// Faz 2b.2-R: yolu sakla (reload icin)
	strcpy_s(this->m_Path,path);
}

// Faz 2b.2-R: config'i yeniden yukler; canli editor akisinin (dosya yaz -> Load -> LogAdd)
// komut karsiligi. Hata durumunda mevcut veriler korunur (E-05 deseni).
void CCustomMonsterSkill::Reload()
{
	if(this->m_Path[0] == 0)
	{
		LogAdd(LOG_RED,"[SPK] CustomMonsterSkill Reload skipped - config path not set yet");
		return;
	}

	std::vector<CUSTOM_MONSTER_SKILL> oldInfo = this->m_Monster_Skill;
	int oldResult = this->m_LoadResult;

	this->Load(this->m_Path);

	if(this->m_LoadResult == 0)
	{
		this->m_Monster_Skill = oldInfo;
		this->m_LoadResult = oldResult;
		LogAdd(LOG_RED,"[SPK] CustomMonsterSkill Reload failed - old data restored (%s)",this->m_Path);
		return;
	}

	LogAdd(LOG_BLUE,"[SPK] CustomMonsterSkill configuration saved and reloaded.");	// canli string (nokta dahil)
}

CUSTOM_MONSTER_SKILL* CCustomMonsterSkill::GetSkillMonster(int MonsterClass)	// canli @004A4FC0
{
	for(int n=0;n < (int)this->m_Monster_Skill.size();n++)	// canli Myfirst..Mylast taramasi, ilk eslesme
	{
		if(this->m_Monster_Skill[n].m_MonsterClass == MonsterClass)
		{
			return &this->m_Monster_Skill[n];
		}
	}

	return 0;
}
