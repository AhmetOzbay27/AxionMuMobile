#pragma once

#include "User.h"
#include "Protocol.h"

struct PMSG_CHANGECLASS_DATA
{
	PSBMSG_HEAD	Head;
	int m_WCoinC;
};

struct CG_CHANGECLASS_RECV
{
	PSBMSG_HEAD Head;
	int Type;
};

class cChangeClass
{
public:
	cChangeClass();
	virtual ~cChangeClass();
	void Init();
	void Load(char* path);
	void LoadXML(char* path);	// E-04 (2b.2-O): canli Data\SPK\ChangeClass.xml okuyucu
	void ClearMasterChangeClass(LPOBJ lpObj);	// E-04 (2b.2-O): canli ClearMasterChangeClass adimi
	void ChangeClass(LPOBJ lpObj, int Class);
	void SendData(int aIndex);
	void RecvChangeClass(CG_CHANGECLASS_RECV* Data, int aIndex);
	static void ChangeClassCallback(LPOBJ lpObj, int Class, DWORD null, DWORD WCoinC, DWORD WCoinP, DWORD GoblinPoint);
	// ----
	int		m_Price[MAX_ACCOUNT_LEVEL];
	int		m_PriceType[MAX_ACCOUNT_LEVEL];
	int m_WCoinC;
	int Enable;
	int LevelStart;
	char m_MsgDisabled[128];	// E-04: XML Msg Index=0 (canli sema)
	char m_MsgNoCoin[128];		// E-04: XML Msg Index=1
	char m_MsgInvalidClass[128];	// E-04: XML Msg Index=2
}; extern cChangeClass gChangeClass;