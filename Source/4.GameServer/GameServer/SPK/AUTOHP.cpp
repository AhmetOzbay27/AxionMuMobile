#include "stdafx.h"
#include "CommandManager.h"
#include "AUTOHP.h"
#include "GensSystem.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Viewport.h"
#include "ObjectManager.h"

CAUTOHP gAUTOHP;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAUTOHP::CAUTOHP() // OK
{
}

CAUTOHP::~CAUTOHP() // OK
{

}

void CAUTOHP::AutoHp(LPOBJ lpObj) // OK
{

	if (lpObj->AUTOHP == 0)
	{
		return;
	}

	//if ((GetTickCount() - lpObj->PotionTime > (DWORD)gServerInfo.m_CheckAutoPotionHackTolerance))
	{
		//	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "12");
			//lpObj->PotionTime = GetTickCount();
		PMSG_ITEM_USE_RECV pMsg;

		pMsg.header.set(0x26, sizeof(pMsg));

		pMsg.SourceSlot = 0xFF;

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 3), -1) : pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 2), -1) : pMsg.SourceSlot);

		pMsg.SourceSlot = ((pMsg.SourceSlot == 0xFF) ? gItemManager.GetInventoryItemSlot(lpObj, GET_ITEM(14, 1), -1) : pMsg.SourceSlot);

		pMsg.TargetSlot = 0xFF;

		pMsg.type = 0;

		if (INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
		{
			gItemManager.CGItemUseRecv(&pMsg, lpObj->Index);
		}

		CItem* lpItem = &gObj[lpObj->Index].Inventory[pMsg.SourceSlot];
		//if ((lpItem->m_Index >= GET_ITEM(14, 0) && lpItem->m_Index <= GET_ITEM(14, 6)) || (lpItem->m_Index >= GET_ITEM(14, 35) && lpItem->m_Index <= GET_ITEM(14, 40)) || lpItem->m_Index == GET_ITEM(14, 70) || lpItem->m_Index == GET_ITEM(14, 71) || lpItem->m_Index == GET_ITEM(14, 133))
		//{
		//
		//	if (gObjectManager.CharacterUsePotion(lpObj, lpItem) != 0)
		//	{
		//		gItemManager.DecreaseItemDur(lpObj, pMsg.SourceSlot, 1);
		//	}
		//
		//}
	}




}

void CAUTOHP::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			gAUTOHP.AutoHp(&gObj[n]);
		}
	}
}

bool CAUTOHP::CommandHp(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GlobalText(659));
		return 0;
	}

	lpObj->AUTOHP ^= 1;

	XULY_CGPACKET cMsg;
	cMsg.header.set(0xD3, 0x10, sizeof(cMsg));
	cMsg.ThaoTac = lpObj->AUTOHP;	//Show Item Cache
	DataSend(lpObj->Index, (BYTE*)&cMsg, cMsg.header.size);

	//gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "%d ThaoTac", cMsg.ThaoTac);
	return 0;
}