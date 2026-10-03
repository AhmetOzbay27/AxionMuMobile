// SPK_MonsterSkill.h: interface for the CCustomMonsterSkill class (2c.1-A2, canli kanittan).
//
// Canli kanit (SPK_MonsterSkill.obj; GameServer_canli.map 1717-1719 + live_disasm 208982-209250):
//   ?Instance@CCustomMonsterSkill@@SAPAV1@XZ          @004A4D00  Meyers singleton
//     (guard 0213C8D4h, nesne 0213C8D8h, atexit yikici 0062C390h).
//   ?Load@CCustomMonsterSkill@@QAEXPAD@Z              @004A4D70  CMemScript new (0x318 B) +
//     SetBuffer(char*) + vector clear + 4x GetToken; '[%s] Could not open file' yolu
//     (MEM_SCRIPT_ERROR_CODE0) + try/catch -> ErrorMessageBox(GetLastError());
//     catch yolu 1 sn token zaman asiminda MEM_SCRIPT_ERROR_CODE4
//     ('[%s] The file were not configured correctly').
//   ?GetSkillMonster@CCustomMonsterSkill@@...H@Z      @004A4FC0  vector taramasi
//     (Myfirst @[ecx], Mylast @[ecx+4], eleman 0Ch = 12 B = 3 int, ilk eslesme kazanir,
//     bulunamazsa 0).
//   Depolama canlida std::vector<CUSTOM_MONSTER_SKILL> (3-pointer deseni 0213C8D8/DC/E0);
//     ust sinir YOK. Bizim eski sabit m_Monster_Skill[1000] + m_count yapisi kaldirildi
//     (tasma korumasi yoktu; canli kayitta da sinir yok).
//   Config: Data\SPK\CustomMonsterSkill.txt - canli yukleyici 0x569791 (?Instance) /
//     0x5697C7 (?Load), yol literali 'SPK\CustomMonsterSkill.txt' @0x63DCB4
//     (strcpy_s(gPath)+strcat_s deseni); canli yukleme sirasi:
//     Message.xml -> AddBuff -> CustomMonsterSkill -> CustomShop.
//   Dosya formati (canli editor kaydi 0x4A55B9-0x4A5655, birebir fprintf bicimi):
//     "0\n" + "//MonsterClass\tSkill1\tSkill2\n" + ("%d\t%d\t%d\n" x N) + "end\n".
//   Canli log stringi (exe 2344740): '[SPK] CustomMonsterSkill configuration saved and reloaded.'
//     (nokta dahil) - kaydet akisinda: dosyayi yaz -> Instance()->Load(yol) -> LogAdd ->
//     MessageBoxW(L"Info", L"Saved and reloaded successfully!").
//   Tuketiciler (canli, birebir hizali):
//     gObjSetMonster  @0x51FD62  GetSkillMonster(MonsterClass) -> AddSkill(m_Skill1,0) +
//       AddSkill(m_Skill2,0). Canlida gorunen CheckSkillRequireLevel/Energy/Leadership/Class
//       cagrilari AddSkill govdesinin LTCG inline'idir (SkillManager.cpp:798 - yalniz
//       OBJECT_USER icin; monsterda dal olu). Bizim disasm'da da ayni inline deseni var
//       (bizim_disasm2 004DCF12-004DD010).
//     gObjMonsterAttack @0x521571 Class zincirinin sonunda 561 (0x231) ile ayni hedefe dallanir
//       (005215DB): PMSG_DURATION_SKILL_ATTACK_RECV, skillL = (GetLargeRand()%100 >= 25).
//   SPK GUI editoru (SPK_MonsterSkillProc/MonSkillEditSubclass @0x4A5070+) kapsam disi
//     (A1 SPK_HarmonyProc ile ayni karar) - dosya format kaniti yukarida kayitli.

#pragma once

#include "stdafx.h"
#include <vector>

struct CUSTOM_MONSTER_SKILL
{
	int m_MonsterClass;
	int m_Skill1;
	int m_Skill2;
};

class CCustomMonsterSkill
{
public:
	static CCustomMonsterSkill* Instance();	// canli ?Instance@CCustomMonsterSkill@@SAPAV1@XZ (bizde global uzerinden - A1 CustomHarmony deseni)

	void Load(char* path);				// canli ?Load@CCustomMonsterSkill@@QAEXPAD@Z
	void Reload();					// Faz 2b.2-R (bizim ek): /reload custommonsterskill - canli editorun kaydet+yeniden yukle akisinin karsiligi
	CUSTOM_MONSTER_SKILL* GetSkillMonster(int MonsterClass);	// canli ?GetSkillMonster@CCustomMonsterSkill@@QAEPAUCUSTOM_MONSTER_SKILL@@H@Z

	std::vector<CUSTOM_MONSTER_SKILL> m_Monster_Skill;	// canli std::vector (0213C8D8h Myfirst/MyLast/MyEnd)
private:
	char m_Path[256];	// Faz 2b.2-R: Load'ta saklanan config yolu (reload icin)
	int m_LoadResult;	// Faz 2b.2-R: son Load sonucu (1=basarili) - Reload eski veriyi buna gore geri alir (E-05)
};

extern CCustomMonsterSkill gCustomMonsterSkill;
