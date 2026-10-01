// MoveSummon.h: interface for the CMoveSummon class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct MOVE_SUMMON_INFO
{
	int Map;
	int X;
	int Y;
	int TX;
	int TY;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int AccountLevel;
	int PkMove;
};

class CMoveSummon
{
public:
	CMoveSummon();
	virtual ~CMoveSummon();
	void Load(char* path);
	void Reload();	// Faz 2b.2-N: canlı SPK log deseni 'MoveSummon configuration reloaded' — /reload movesummon
	bool CheckMoveSummon(LPOBJ lpObj,int map,int x,int y);
private:
	std::vector<MOVE_SUMMON_INFO> m_MoveSummonInfo;
	char m_Path[256];	// Faz 2b.2-N: Load'ta saklanan config yolu (reload için)
};

extern CMoveSummon gMoveSummon;
