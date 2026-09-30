
#pragma once

#include "User.h"

class CAUTOHP
{
public:
	CAUTOHP();
	virtual ~CAUTOHP();
	void Load(char* path);
	void AutoHp(LPOBJ lpObj);
	bool CommandHp(int aIndex);
	void MainProc();
private:

};

extern CAUTOHP gAUTOHP;
