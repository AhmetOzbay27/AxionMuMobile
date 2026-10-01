#include "StdAfx.h"
#include "CB_ActiveInvasions.h"
#include "Util.h"
#if(CB_ActiveInvasionsD)
// =============================================================================
// 2c.1-B3 (01.10.2026) — canli ActiveInvasions.obj disasm paritesi
// Canli gvdeler (live_disasm.txt):
//   monster_add          0x41E950 : map<int,UInvasionsData> bul/ekle;
//                          varsa count++ & max_count++ (iki sayac birlikte);
//                          bool arg 1 ise update_by_monster_id (broadcast).
//                          (CInvasionManager::SetMonster + CObjectManager::
//                          ObjectSetStateProc boss-regen yolundan boyle
//                          cagriliyor — 0x4F1EBF/0x4F1EFC, 0x538687)
//   update_by_monster_id 0x41EA80 : paket C1 10 F3 99 {id, count, 0} (14 B)
//                          -> tum bagli istemciler (canli filtre [obj+4]==3
//                          = Connected/playing — bizim DataSendAll esdegeri).
//   send_list_to_client  0x41EB20 : paket C2 [size] F3 98 {count, N x 12 B
//                          {id, count, max}} -> tek istemci (aIndex) veya
//                          canli F3 40 sub2 cekme zinciriyle tüm istemciler.
// Donor kaynakta modul YOK (MUIG ozel).
// =============================================================================

CB_ActiveInvasions gCB_ActiveInvasions;

CB_ActiveInvasions::CB_ActiveInvasions()
{
	this->m_data.clear();
}

void CB_ActiveInvasions::monster_add(int monster_id,bool send_update) // 2c.1-B3: canli imza (0x41E950)
{
	std::map<int,UInvasionsData>::iterator it = this->m_data.find(monster_id);

	if (it == this->m_data.end())
	{
		this->m_data.insert(std::pair<int,UInvasionsData>(monster_id,UInvasionsData(1,1)));
	}
	else
	{
		it->second.count += 1;
		it->second.max_count += 1;	// 2c.1-B3: canli iki sayaci birlikte artirir (0x41E99F: inc [+14], inc [+18])
	}

	if (send_update != false)		// 2c.1-B3: canli bool arg = broadcast (0x41E9A5)
	{
		this->update_by_monster_id(monster_id);
	}
}

void CB_ActiveInvasions::monster_del(int monster_id,bool send_update)
{
	std::map<int,UInvasionsData>::iterator it = this->m_data.find(monster_id);

	if (it == this->m_data.end())
	{
		//LogAdd(LOG_RED, "[CB_ActiveInvasions] Error! Monster not found, id : %d", monster_id);
	}
	else
	{
		if (it->second.count > 0)
		{
			it->second.count -= 1;
			it->second.max_count -= 1;	// 2c.1-B3: canli iki sayaci birlikte azaltir (0x4F1C0C blok: dec [+14], dec [+18])

			if (send_update != false)
			{
				this->update_by_monster_id(monster_id);
			}
		}

		if (it->second.count == 0)		// 2c.1-B3: canli erase kriteri sadece count==0 (max korunmaz — CInvasionManager::MonsterDieProc/SetState_EMPTY)
		{
			this->m_data.erase(monster_id);
		}
	}
}

void CB_ActiveInvasions::update_by_monster_id(int monster_id) // canli 0x41EA80 birebir karsilik
{
	std::map<int,UInvasionsData>::iterator it = this->m_data.find(monster_id);

	if (it == this->m_data.end())
	{
		return;
	}

	PMSG_ACTIVE_INVASIONS_UPDATE_SEND pMsg;

	pMsg.header.set(0xF3,0x99,sizeof(pMsg));	// 2c.1-B3: canli C1 10 F3 99 (bizim eski D3 99 idi)

	pMsg.monster_id = monster_id;

	pMsg.count = it->second.count;

	pMsg.spare = 0;									// canli paketin son 4 Bayti 0 (0x41EABE)

	DataSendAll(reinterpret_cast<BYTE*>(&pMsg),pMsg.header.size);
}

void CB_ActiveInvasions::send_list_to_client() // 2c.1-B3: eski toplu cagri noktalari icin herkese gonderim
{
	BYTE send[8192];

	PMSG_ACTIVE_INVASIONS_SEND pMsg;

	pMsg.header.set(0xF3,0x98,0);	// 2c.1-B3: canli C2 F3 98 (bizim eski D3 98 idi)

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_ACTIVE_INVASIONS info;

	for (std::map<int,UInvasionsData>::iterator it = this->m_data.begin(); it != this->m_data.end(); it++)
	{
		info.monster_id = it->first;
		info.count = it->second;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSendAll(send,size);
}

void CB_ActiveInvasions::send_list_to_client(int aIndex) // 2c.1-B3: canli tekil imza (0x41EB20 — F7 sub 0x02 istegi + ProtocolCore)
{
	BYTE send[8192];

	PMSG_ACTIVE_INVASIONS_SEND pMsg;

	pMsg.header.set(0xF3,0x98,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_ACTIVE_INVASIONS info;

	for (std::map<int,UInvasionsData>::iterator it = this->m_data.begin(); it != this->m_data.end(); it++)
	{
		info.monster_id = it->first;
		info.count = it->second;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}
#endif
