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

#include "stdafx.h"
#include "SPK_MonsterSkill.h"
#include "MemScript.h"
#include "Util.h"
#include "Log.h"
#include "Path.h"
#include "User.h"		// 2c.1-A2 TEST (GECICI)
#include "ObjectManager.h"	// 2c.1-A2 TEST (GECICI)
#include "Monster.h"		// 2c.1-A2 TEST (GECICI)
#include "SkillManager.h"	// 2c.1-A2 TEST (GECICI)

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

// ========================= 2c.1-A2 TEST (GECICI) =========================
// Tetik: ..\Data\SPK\MonsterSkill_selftest.flag (EventMainManager 1 sn tick gorup siler).
// Tur sonunda kaldirilir; snapshot + patch BuildLog/2c1 altinda arsivlenir.

void CustomMonsterSkillSelfTest()
{
	CCustomMonsterSkill* lpSkill = CCustomMonsterSkill::Instance();

	LogAdd(LOG_RED,"[MSTEST] ===== SELFTEST BASLADI =====");
	LogAdd(LOG_RED,"[MSTEST] Instance()=%p gCustomMonsterSkill=%p ayni=%d | kayit=%d (canli SPK\\CustomMonsterSkill.txt = 16 satir)",
		(void*)lpSkill,(void*)&gCustomMonsterSkill,(lpSkill == &gCustomMonsterSkill),(int)lpSkill->m_Monster_Skill.size());
	LogAdd(LOG_RED,"[MSTEST] loader yolu: %s",gPath.GetFullPath("SPK\\CustomMonsterSkill.txt"));

	// 1) veri dogrulugu (canli dosya satirlari birebir)
	int testRows[6][3] = {{750,41,232},{704,42,264},{706,38,0},{561,38,0},{754,78,0},{722,13,0}};

	for(int n=0;n < 6;n++)
	{
		CUSTOM_MONSTER_SKILL* lpInfo = lpSkill->GetSkillMonster(testRows[n][0]);

		if(lpInfo != 0)
		{
			LogAdd(LOG_RED,"[MSTEST] GetSkillMonster(%d) -> {class=%d,skill1=%d,skill2=%d} beklenen {%d,%d,%d} %s",
				testRows[n][0],lpInfo->m_MonsterClass,lpInfo->m_Skill1,lpInfo->m_Skill2,
				testRows[n][0],testRows[n][1],testRows[n][2],
				((lpInfo->m_MonsterClass==testRows[n][0] && lpInfo->m_Skill1==testRows[n][1] && lpInfo->m_Skill2==testRows[n][2])?"OK":"FARKLI"));
		}
		else
		{
			LogAdd(LOG_RED,"[MSTEST] GetSkillMonster(%d) -> NULL (BEKLENMEYEN)",testRows[n][0]);
		}
	}

	// canli dosyada 719 iki kez: satir 8 (232) / satir 16 (4) -> canli GetSkillMonster ilk bulani doner
	CUSTOM_MONSTER_SKILL* lpDup = lpSkill->GetSkillMonster(719);
	LogAdd(LOG_RED,"[MSTEST] 719 mukerrer kayit ilk-eslesme kurali -> skill1=%d skill2=%d (beklenen ilk kayit 232/0)",
		(lpDup!=0)?lpDup->m_Skill1:-1,(lpDup!=0)?lpDup->m_Skill2:-1);
	LogAdd(LOG_RED,"[MSTEST] GetSkillMonster(99999)=%p (beklenen 0)",(void*)lpSkill->GetSkillMonster(99999));

	// 2) TUKETICI 1: gObjSetMonster (canli 0x51FD62) -> gercek spawn + AddSkill zinciri
	int idxA = gObjAddMonster(0);	// class 704 {704,42,264}
	int idxB = gObjAddMonster(0);	// class 706 {706,38,0}
	int idxC = gObjAddMonster(0);	// class 700 - kontrol (canli listede yok)

	if(OBJECT_RANGE(idxA) == 0 || OBJECT_RANGE(idxB) == 0 || OBJECT_RANGE(idxC) == 0)
	{
		LogAdd(LOG_RED,"[MSTEST] spawn slotu alinamadi idxA=%d idxB=%d idxC=%d",idxA,idxB,idxC);
		return;
	}

	int testIndex[3] = {idxA,idxB,idxC};
	int testClass[3] = {704,706,700};

	for(int n=0;n < 3;n++)
	{
		LPOBJ lpObj = &gObj[testIndex[n]];

		lpObj->PosNum = -1;
		lpObj->X = 130; lpObj->Y = 133; lpObj->TX = 130; lpObj->TY = 133;
		lpObj->OldX = 130; lpObj->OldY = 133; lpObj->StartX = 130; lpObj->StartY = 133;
		lpObj->Dir = 0; lpObj->Map = 0;

		if(gObjSetMonster(testIndex[n],testClass[n]) == 0)
		{
			LogAdd(LOG_RED,"[MSTEST] gObjSetMonster(%d,%d)=0 (Monster.txt kaydi yok?)",testIndex[n],testClass[n]);
			continue;
		}

		LogAdd(LOG_RED,"[MSTEST] spawn idx=%d class=%d AttackType=%d Type=%d | GetSkill(42)=%d GetSkill(38)=%d GetSkill(264)=%d",
			testIndex[n],lpObj->Class,lpObj->AttackType,lpObj->Type,
			(gSkillManager.GetSkill(lpObj,42)!=0),(gSkillManager.GetSkill(lpObj,38)!=0),(gSkillManager.GetSkill(lpObj,264)!=0));
	}

	// 3) TUKETICI 2: gObjMonsterAttack (canli 0x521571) - custom dal secimi
	gObj[idxA].TargetNumber = idxB;
	gObjMonsterAttack(&gObj[idxA],&gObj[idxB]);
	LogAdd(LOG_RED,"[MSTEST] gObjMonsterAttack cagrildi: attacker idx=%d class=%d -> target idx=%d class=%d",
		idxA,gObj[idxA].Class,idxB,gObj[idxB].Class);

	// 4) Reload yolu + canli log stringi
	int before = (int)lpSkill->m_Monster_Skill.size();
	lpSkill->Reload();
	LogAdd(LOG_RED,"[MSTEST] Reload sonrasi kayit=%d (once=%d)",(int)lpSkill->m_Monster_Skill.size(),before);

	// 5) temizlik
	gObjDel(idxA);
	gObjDel(idxB);
	gObjDel(idxC);
	LogAdd(LOG_RED,"[MSTEST] ===== SELFTEST BITTI (spawn edilen 3 obje silindi) =====");
}

// 2c.1-A2 TEST (GECICI): gObjMonsterAttack 561/custom dal secimi — canli kosulun aynisi + log.
bool CustomMonsterSkillAttackBranchTest(int MonsterClass)
{
	bool l_Result = (MonsterClass == 561 || gCustomMonsterSkill.GetSkillMonster(MonsterClass) != 0);

	LogAdd(LOG_RED,"[MSTEST] gObjMonsterAttack DURATION dal secildi: class=%d custom=%d (561 mi=%d)",
		MonsterClass,(gCustomMonsterSkill.GetSkillMonster(MonsterClass)!=0),(MonsterClass==561));

	return l_Result;
}

bool CustomMonsterSkillTestTick()
{
	static DWORD l_TickCount = 0;

	if((GetTickCount()-l_TickCount) < 1000)
	{
		return 0;
	}

	l_TickCount = GetTickCount();

	char* l_TestFlag = gPath.GetFullPath("SPK\\MonsterSkill_selftest.flag");

	if(GetFileAttributes(l_TestFlag) == INVALID_FILE_ATTRIBUTES)
	{
		return 0;
	}

	DeleteFile(l_TestFlag);

	CustomMonsterSkillSelfTest();

	return 1;
}

// ======================= /2c.1-A2 TEST (GECICI) =======================
