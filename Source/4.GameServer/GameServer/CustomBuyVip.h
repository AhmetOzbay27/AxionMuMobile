// CustomItem.h: interface for the CCustomItem class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_CUSTOM_BUYVIP 3
#include "Protocol.h"

struct BUYPREMIUM_REQ
{
	PSBMSG_HEAD h;
	int	VipType;
};

struct CUSTOM_BUYVIP_INFO
{
	int Index;
	int Exp;
	int Drop;
	int Days;
	int Coin1;
	int Coin2;
	int Coin3;
	char VipName[32];
};

class CCustomBuyVip
{
public:
	CCustomBuyVip();
	virtual ~CCustomBuyVip();
	void Init();
	void Load(char* path);
	void Reload();	// E-05 (Faz 2b.2): canlı SPK 'CustomBuyVip configuration reloaded' log deseni — /reload buyvip
	void SetInfo(CUSTOM_BUYVIP_INFO info);
	CUSTOM_BUYVIP_INFO* GetInfo(int index);
	void BuyVip(int aIndex, BUYPREMIUM_REQ* lpMsg);
	void BuyVipDone(LPOBJ lpObj);
public:
	CUSTOM_BUYVIP_INFO m_CustomBuyVipInfo[MAX_CUSTOM_BUYVIP];
	char m_Path[256];	// E-05: Load'ta saklanan config yolu (reload için)
};

extern CCustomBuyVip gCustomBuyVip;