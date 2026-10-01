// GameMaster.h: interface for the CGameMaster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

#define MAX_GAME_MASTER 100

struct GAME_MASTER_INFO
{
	char Account[11];
	char Name[11];
	BYTE Level;
};

class CGameMaster
{
public:
	CGameMaster();
	virtual ~CGameMaster();
	void Load(char* path);
	void Reload();	// Faz 2b.2-R: canlı log deseni '[CGameMaster] GameMaster configuration reloaded' — /reload gamemaster
	void SetInfo(GAME_MASTER_INFO info);
	void SetGameMasterLevel(LPOBJ lpObj,int level);
	bool CheckGameMasterLevel(LPOBJ lpObj,int level);
private:
	GAME_MASTER_INFO m_GameMasterInfo[MAX_GAME_MASTER];
	int m_count;
	char m_Path[256];	// Faz 2b.2-R: Load'ta saklanan config yolu (reload için)
};

extern CGameMaster gGameMaster;
