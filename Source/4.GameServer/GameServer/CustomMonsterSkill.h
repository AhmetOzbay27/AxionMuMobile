#include "stdafx.h"
#define MAX_MONSTER_SKILL 1000

struct CUSTOM_MONSTER_SKILL
{
	int  m_MonsterClass;
	int  m_Skill1;
	int  m_Skill2;
};

class CCustomMonsterSkill
{
public:
	int m_count;

	void Load(char* path);
	void Reload();	// Faz 2b.2-R: canlı SPK log deseni '[SPK] CustomMonsterSkill configuration saved and reloaded' — /reload custommonsterskill
	CUSTOM_MONSTER_SKILL* GetSkillMonster(int MonsterClass);
	CUSTOM_MONSTER_SKILL m_Monster_Skill[MAX_MONSTER_SKILL];
private:
	char m_Path[256];	// Faz 2b.2-R: Load'ta saklanan config yolu (reload için)

};
extern CCustomMonsterSkill gCustomMonsterSkill;