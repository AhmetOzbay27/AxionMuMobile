// SPK_Harmony.cpp: implementation of the CustomHarmony class (2c.1-A1).
//
// Canli kanittan sifirdan yazim — docs/13 A1 / docs/05 #4. Config:
// Data\SPK\CustomHarmony.xml (canli sema birebir; Enable=0 canli degeri).
// Paket akisi canli dispatcher ile eslesti (bizim 0xD3 switch ailesi):
//   0x6F -> SetStateInterface(a,0), 0x71 -> ProcMix(a),
//   ProcItemSend(a,Source) / BackItem(a,ThaoTac; -1 korumasi canli birebir).
// Canli XML OptIndex degerleri bizim engine harmony-byte kodlamasinin
// birebir kendisi: (option<<4)|level — 86=0x56 (opt5,lvl6), 173=0xAD
// (opt10,lvl13) dogrulandi; JewelOfHarmonyOption.cpp:433 kodlamasi.
// Basarisizlik stringi canli Msg3, basari 'Harmony enhancement',
// tasma korumasi canli '[DaTaoHoa] Data qua dai !!' — hepsi exe string.

#include "stdafx.h"
#include "SPK_Harmony.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "Log.h"
#include "Path.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "CashShop.h"
#include "JewelOfHarmonyOption.h"
#include "JewelOfHarmonyType.h"
#include "Protocol.h"
#include "DSProtocol.h"

CustomHarmony gCustomHarmony;

CustomHarmony* CustomHarmony::Instance()	// canli singleton (map: ?Instance@CustomHarmony@@SAPAV1@XZ)
{
	return &gCustomHarmony;
}

void CustomHarmony::Load(char* path) // canli ?Load@CustomHarmony@@QAEXPAD@Z semantigi
{
	this->m_Enable = 0;
	this->m_MessageMap.clear();
	this->m_HarmonyMap.clear();
	this->m_OptWeaponCount = 0;
	this->m_OptStaffCount = 0;
	this->m_OptItemCount = 0;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);
	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node root = file.child("Harmony");

	this->m_Enable = root.attribute("Enable").as_int(0);
	this->m_PriceType = root.attribute("PriceType").as_int(0);
	this->m_Price = root.attribute("Price").as_int(0);
	this->m_Rate = root.attribute("Rate").as_int(0);

	for (pugi::xml_node msg = root.child("Message").child("Msg"); msg; msg = msg.next_sibling("Msg"))
	{
		int index = msg.attribute("Index").as_int(-1);
		if (index < 0)
		{
			continue;
		}

		HARMONY_MESSAGE info;
		memset(&info, 0, sizeof(info));
		info.Index = index;
		strcpy_s(info.Text, msg.attribute("Text").as_string(""));

		this->m_MessageMap[index] = info;
	}

	pugi::xml_node npc = root.child("NPC");
	if (npc != 0)
	{
		this->m_NPCClass = npc.attribute("NPCClass").as_int(0);
		this->m_NPCMap = npc.attribute("NPCMap").as_int(0);
		this->m_NPCX = npc.attribute("NPCX").as_int(0);
		this->m_NPCY = npc.attribute("NPCY").as_int(0);
		this->m_NPCDir = npc.attribute("NPCDir").as_int(0);
		strcpy_s(this->m_NPCName, npc.attribute("Name").as_string(""));
	}

	for (pugi::xml_node opt = root.child("OptWeapon").child("Option"); opt; opt = opt.next_sibling("Option"))
	{
		if (this->m_OptWeaponCount >= 16) break;
		HARMONY_OPTION_INFO* info = &this->m_OptWeapon[this->m_OptWeaponCount++];
		info->OptIndex = opt.attribute("OptIndex").as_int(0);
		strcpy_s(info->Name, opt.attribute("Name").as_string(""));
		info->Level = opt.attribute("Level").as_int(0);
		info->Rate = opt.attribute("Rate").as_int(0);
	}

	for (pugi::xml_node opt = root.child("OptStaff").child("Option"); opt; opt = opt.next_sibling("Option"))
	{
		if (this->m_OptStaffCount >= 16) break;
		HARMONY_OPTION_INFO* info = &this->m_OptStaff[this->m_OptStaffCount++];
		info->OptIndex = opt.attribute("OptIndex").as_int(0);
		strcpy_s(info->Name, opt.attribute("Name").as_string(""));
		info->Level = opt.attribute("Level").as_int(0);
		info->Rate = opt.attribute("Rate").as_int(0);
	}

	for (pugi::xml_node opt = root.child("OptItem").child("Option"); opt; opt = opt.next_sibling("Option"))
	{
		if (this->m_OptItemCount >= 16) break;
		HARMONY_OPTION_INFO* info = &this->m_OptItem[this->m_OptItemCount++];
		info->OptIndex = opt.attribute("OptIndex").as_int(0);
		strcpy_s(info->Name, opt.attribute("Name").as_string(""));
		info->Level = opt.attribute("Level").as_int(0);
		info->Rate = opt.attribute("Rate").as_int(0);
	}
}

void CustomHarmony::Save(char* path) // canli yuzey (SPK editor kaydi); calisma aninda Load yeterli
{
	this->Load(path);
}

char* CustomHarmony::GetMessage(int index) // canli GetMessageA
{
	std::map<int, HARMONY_MESSAGE>::iterator it = this->m_MessageMap.find(index);
	if (it == this->m_MessageMap.end())
	{
		return "[Harmony] Unknown message";
	}

	return it->second.Text;
}

void CustomHarmony::SetStateInterface(int aIndex, int State) // canli 0x6F dali
{
	if (this->m_Enable == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return;
	}

	// oturumu ac / yenile
	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	if (lpSession == 0)
	{
		if (this->m_HarmonyMap.size() >= HARMONY_MAX_SESSION)	// canli: '[DaTaoHoa] Data qua dai !!'
		{
			LogAdd(LOG_RED, "[DaTaoHoa] Data qua dai !!");
			return;
		}

		HM_HAMORNY info;
		info.Init();
		this->m_HarmonyMap.insert(std::pair<int, HM_HAMORNY>(aIndex, info));
		lpSession = this->GetSession(aIndex);

		if (lpSession == 0)
		{
			return;
		}
	}

	lpSession->Init();

	this->SendListItemPoint(aIndex, State);
}

void CustomHarmony::ProcItemSend(int aIndex, int Source) // canli: item oturuma kilitleniyor
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->m_Enable == 0)
	{
		return;
	}

	if (Source < HARMONY_INVENTORY_START || Source >= (INVENTORY_WEAR_SIZE + INVENTORY_MAIN_SIZE))
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[Source];

	if (lpItem->IsItem() == 0)
	{
		return;
	}

	// canli Msg 1: item harmoni tasiyamaz (canli motor ile ayni kisit seti)
	if (gJewelOfHarmonyType.CheckJewelOfHarmonyItemType(lpItem) == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	if (lpItem->IsSetItem() != 0 && gServerInfo.m_SetItemAcceptHarmonySwitch == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	if (lpItem->IsJewelOfHarmonyItem() != 0)	// canli Msg 2: harmony op'u zaten dolu
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	if (lpItem->IsSocketItem() != 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	if (lpSession == 0)
	{
		return;
	}

	lpSession->Slot = Source;	// item oturuma kilitlendi (yerinde; mix sonunda yenilenir)

	this->SendListItemPoint(aIndex, 0);
}

void CustomHarmony::BackItem(int aIndex, int Slot) // canli: ThaoTac==-1 korumasi birebir
{
	if (Slot == -1)	// canli: cmp 0FFFFFFFFh / je return
	{
		return;
	}

	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	if (lpSession == 0 || lpSession->Slot == -1)
	{
		return;
	}

	lpSession->Init();	// oturum kilidi kalkar (item yerinde kaldigi icin tasima gerekmez)

	this->SendListItemPoint(aIndex, 0);
}

int CustomHarmony::PayPrice(LPOBJ lpObj) // canli: PriceType 1=WcoinC, 2=WcoinP, 3=GP, diger/Zen
{
	switch (this->m_PriceType)
	{
		case 1:
		{
			if (lpObj->Coin1 < (DWORD)this->m_Price) { return 2; }
			GDSetCoinSend(lpObj->Index, -(this->m_Price), 0, 0, "Harmony");
			gCashShop.CGCashShopPointRecv(lpObj->Index);
			return 1;
		}
		case 2:
		{
			if (lpObj->Coin2 < (DWORD)this->m_Price) { return 3; }
			GDSetCoinSend(lpObj->Index, 0, -(this->m_Price), 0, "Harmony");
			gCashShop.CGCashShopPointRecv(lpObj->Index);
			return 1;
		}
		case 3:
		{
			if (lpObj->Coin3 < (DWORD)this->m_Price) { return 4; }
			GDSetCoinSend(lpObj->Index, 0, 0, -(this->m_Price), "Harmony");
			gCashShop.CGCashShopPointRecv(lpObj->Index);
			return 1;
		}
		default:
		{
			if (lpObj->Money < (DWORD)this->m_Price) { return 5; }
			lpObj->Money -= this->m_Price;
			GCMoneySend(lpObj->Index, lpObj->Money);
			return 1;
		}
	}
}

void CustomHarmony::ProcMix(int aIndex) // canli 0x71 dali: Jewel of Harmony mix
{
	LPOBJ lpObj = &gObj[aIndex];

	if (this->m_Enable == 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(0));
		return;
	}

	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	if (lpSession == 0 || lpSession->Slot == -1)
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[lpSession->Slot];

	if (lpItem->IsItem() == 0 || lpItem->m_JewelOfHarmonyOption != 0)
	{
		lpSession->Init();
		return;
	}

	int type = gJewelOfHarmonyOption.GetJewelOfHarmonyItemOptionType(lpItem);

	if (type == JEWEL_OF_HARMONY_ITEM_OPTION_TYPE_NONE)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	if (gItemManager.GetInventoryItemCount(lpObj, HARMONY_JEWEL_INDEX, 0) < 1)	// canli Msg 5
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(5));
		return;
	}

	int PayResult = this->PayPrice(lpObj);	// canli Msg 4: eksik para

	if (PayResult != 1)
	{
		char* szCoin = ((this->m_PriceType == 1) ? "WcoinC" : (this->m_PriceType == 2) ? "WcoinP" : (this->m_PriceType == 3) ? "GoblinPoint" : "Zen");
		char szTemp[160] = { 0 };
		wsprintf(szTemp, this->GetMessage(4), szCoin, "");
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, szTemp);
		return;
	}

	// canli tablo secimi: item turune gore XML tablosundan op secilir;
	// OptIndex zaten (option<<4)|level kodlu — dogrudan byte'a yazilir
	HARMONY_OPTION_INFO* lpTable = 0;
	int lpTableCount = 0;

	switch (type)
	{
		case HARMONY_OPTTYPE_WEAPON:
			lpTable = this->m_OptWeapon;
			lpTableCount = this->m_OptWeaponCount;
			break;
		case HARMONY_OPTTYPE_STAFF:
			lpTable = this->m_OptStaff;
			lpTableCount = this->m_OptStaffCount;
			break;
		default:
			lpTable = this->m_OptItem;
			lpTableCount = this->m_OptItemCount;
			break;
	}

	if (lpTableCount <= 0)
	{
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(1));
		return;
	}

	HARMONY_OPTION_INFO* lpOpt = &lpTable[(GetLargeRand() % lpTableCount)];

	int Rate = ((lpOpt->Rate > 0) ? lpOpt->Rate : this->m_Rate);	// canli: kademeli tablo, global Rate yedegi

	if ((GetLargeRand() % 100) >= Rate)
	{
		gItemManager.DeleteInventoryItemCount(lpObj, HARMONY_JEWEL_INDEX, 0, 1);
		gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, this->GetMessage(3));	// canli Msg 3: basarisiz
		return;
	}	// engine kodlamasi: byte = XML OptIndex; Convert + preview + envanter yenile
	lpItem->m_JewelOfHarmonyOption = (lpOpt->OptIndex & 0xFF);
	lpItem->Convert(lpItem->m_Index, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, lpItem->m_NewOption, lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_SocketOptionBonus);
	gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);	gItemManager.DeleteInventoryItemCount(lpObj, HARMONY_JEWEL_INDEX, 0, 1);

	gItemManager.GCItemModifySend(aIndex, (BYTE)lpSession->Slot);	// item durumunu istemciye yenile

	lpSession->Init();

	gNotice.GCNoticeSend(aIndex, 1, 0, 0, 0, 0, 0, "Harmony enhancement");	// canli string
}

void CustomHarmony::SendListItemPoint(int aIndex, int State) // canli C2 0xD3:0C00C2; bizim liste deseni 0xD3:0x24 (BCustomVIPChar)
{
	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	PMSG_HARMONY_LIST_SEND pMsg;
	pMsg.header.set(0xD3, 0x24, sizeof(pMsg));
	pMsg.Result = 0;

	if (lpSession != 0)
	{
		pMsg.Result = lpSession->Slot + 1;	// canli: [esi+14h] (Slot+1; 0 = oturum yok)
	}

	DataSend(aIndex, (BYTE*)&pMsg, sizeof(pMsg));
}

void CustomHarmony::SendInfoItemCache(int aIndex) // item durumu yenileme (bizim GCItemModifySend deseni)
{
	HM_HAMORNY* lpSession = this->GetSession(aIndex);

	if (lpSession != 0 && lpSession->Slot != -1)
	{
		gItemManager.GCItemModifySend(aIndex, (BYTE)lpSession->Slot);
	}
}

void CustomHarmony::ClearSession(int aIndex)
{
	std::map<int, HM_HAMORNY>::iterator it = this->m_HarmonyMap.find(aIndex);

	if (it != this->m_HarmonyMap.end())
	{
		this->m_HarmonyMap.erase(it);
	}
}

HM_HAMORNY* CustomHarmony::GetSession(int aIndex)
{
	std::map<int, HM_HAMORNY>::iterator it = this->m_HarmonyMap.find(aIndex);

	if (it == this->m_HarmonyMap.end())
	{
		return 0;
	}

	return &it->second;
}
