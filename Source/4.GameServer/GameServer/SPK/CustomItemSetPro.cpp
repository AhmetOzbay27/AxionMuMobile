// SPK_CustomItemSetPro.cpp: implementation of the CustomSetDameItem class (2c.1-A3).
//
// Canli kanittan sifirdan yazim — docs/13 A3 / docs/05 #37.
// Ayrintili kanit bloklari: SPK_CustomItemSetPro.h. Ozet:
//   Load  @00476640: '<ItemData>' altinda 'Item' (Section dahil 22 alan) ve 'ItemSet'
//     (Section'siz 21 alan) dugumleri; iki vector once temizlenir; parse hatasinda
//     ErrorMessageBox("File %s load fail. Error: %s", ...) + int 3 (canli 0x637F34).
//   Save  @00477360: ItemData kok; her Item satiri 22, her ItemSet satiri 21 attribute;
//     pugixml varsayilan bicim (canli: fopen "wb" + doc.save(writer)).
//   CalcCharacter @00478BF0: flag != 0 -> erken donus (canli cmp [ebp+0Ch],0 / jne epilog);
//     once Item[] satirlari kendi Section'iyla, sonra ItemSet[] satirlari Section=7..11
//     varyantlariyla (canli: kopyayi alip ilk alani 7..11 yapar, satir basina 5 cagri).
//   CalcSlot (lambda @00478C90): wear slot taramasi 2..11; eslesen her item icin EKLEME.

#include "stdafx.h"
#include "SPK/CustomItemSetPro.h"
#include "Util.h"

CustomSetDameItem gCustomSetDameItem;

CustomSetDameItem* CustomSetDameItem::Instance()	// canli ?Instance@CustomSetDameItem@@SAPAV1@XZ @004765D0
{
	return &gCustomSetDameItem;
}

void CustomSetDameItem::Load(char* path)	// canli ?Load@CustomSetDameItem@@QAEXPAD@Z @00476640
{
	this->m_vItemData.clear();		// canli: [edi] -> [edi+4] (end=begin)
	this->m_vItemSetData.clear();	// canli: [edi+0Ch] -> [edi+10h]

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);

	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node root = file.child("ItemData");

	// canli 1. dongu: child("Item") + next_sibling("Item") — Section dahil tum alanlar okunur
	for (pugi::xml_node node = root.child("Item"); node; node = node.next_sibling("Item"))
	{
		ConfigSetDataDamage info;
		memset(&info, 0, sizeof(info));

		info.Section = node.attribute("Section").as_int(0);
		info.Index = node.attribute("Index").as_int(0);
		info.Effect = node.attribute("Effect").as_int(0);
		info.Lv = node.attribute("Lv").as_int(0);
		info.Opt = node.attribute("Opt").as_int(0);
		info.Dmg = node.attribute("Dmg").as_int(0);
		info.AddSD = node.attribute("AddSD").as_int(0);
		info.AddHP = node.attribute("AddHP").as_int(0);
		info.AddMP = node.attribute("AddMP").as_int(0);
		info.Def = node.attribute("Def").as_int(0);
		info.ExlDmgRate = node.attribute("ExlDmgRate").as_int(0);
		info.CriDmgRate = node.attribute("CriDmgRate").as_int(0);
		info.DoubleDmgRate = node.attribute("DoubleDmgRate").as_int(0);
		info.ResistDouble = node.attribute("ResistDouble").as_int(0);
		info.ResistIgnoreDef = node.attribute("ResistIgnoreDef").as_int(0);
		info.ResistIgnoreSD = node.attribute("ResistIgnoreSD").as_int(0);
		info.ResistCrit = node.attribute("ResistCrit").as_int(0);
		info.ResistExl = node.attribute("ResistExl").as_int(0);
		info.ResistStun = node.attribute("ResistStun").as_int(0);
		info.Reflect = node.attribute("Reflect").as_int(0);
		info.Time = node.attribute("Time").as_int(0);
		info.SetOption = node.attribute("SetOption").as_int(0);

		this->m_vItemData.push_back(info);
	}

	// canli 2. dongu: child("ItemSet") + next_sibling("ItemSet") — Section dugumde YOK
	// (canli 0x476D8B: [ebp-70h] acikca 0'lanir; CalcCharacter Section'i 7..11 ile ezer)
	for (pugi::xml_node node = root.child("ItemSet"); node; node = node.next_sibling("ItemSet"))
	{
		ConfigSetDataDamage info;
		memset(&info, 0, sizeof(info));

		info.Index = node.attribute("Index").as_int(0);
		info.Effect = node.attribute("Effect").as_int(0);
		info.Lv = node.attribute("Lv").as_int(0);
		info.Opt = node.attribute("Opt").as_int(0);
		info.Dmg = node.attribute("Dmg").as_int(0);
		info.AddSD = node.attribute("AddSD").as_int(0);
		info.AddHP = node.attribute("AddHP").as_int(0);
		info.AddMP = node.attribute("AddMP").as_int(0);
		info.Def = node.attribute("Def").as_int(0);
		info.ExlDmgRate = node.attribute("ExlDmgRate").as_int(0);
		info.CriDmgRate = node.attribute("CriDmgRate").as_int(0);
		info.DoubleDmgRate = node.attribute("DoubleDmgRate").as_int(0);
		info.ResistDouble = node.attribute("ResistDouble").as_int(0);
		info.ResistIgnoreDef = node.attribute("ResistIgnoreDef").as_int(0);
		info.ResistIgnoreSD = node.attribute("ResistIgnoreSD").as_int(0);
		info.ResistCrit = node.attribute("ResistCrit").as_int(0);
		info.ResistExl = node.attribute("ResistExl").as_int(0);
		info.ResistStun = node.attribute("ResistStun").as_int(0);
		info.Reflect = node.attribute("Reflect").as_int(0);
		info.Time = node.attribute("Time").as_int(0);
		info.SetOption = node.attribute("SetOption").as_int(0);

		this->m_vItemSetData.push_back(info);
	}
}

void CustomSetDameItem::Save(char* path)	// canli ?Save@CustomSetDameItem@@QAEXPAD@Z @00477360
{
	pugi::xml_document file;

	pugi::xml_node root = file.append_child("ItemData");

	// canli 1. dongu: Item[] — 22 attribute (Section ilk sirada)
	for (size_t i = 0; i < this->m_vItemData.size(); i++)
	{
		ConfigSetDataDamage* lpInfo = &this->m_vItemData[i];

		pugi::xml_node node = root.append_child("Item");

		node.append_attribute("Section").set_value(lpInfo->Section);
		node.append_attribute("Index").set_value(lpInfo->Index);
		node.append_attribute("Effect").set_value(lpInfo->Effect);
		node.append_attribute("Lv").set_value(lpInfo->Lv);
		node.append_attribute("Opt").set_value(lpInfo->Opt);
		node.append_attribute("Dmg").set_value(lpInfo->Dmg);
		node.append_attribute("AddSD").set_value(lpInfo->AddSD);
		node.append_attribute("AddHP").set_value(lpInfo->AddHP);
		node.append_attribute("AddMP").set_value(lpInfo->AddMP);
		node.append_attribute("Def").set_value(lpInfo->Def);
		node.append_attribute("ExlDmgRate").set_value(lpInfo->ExlDmgRate);
		node.append_attribute("CriDmgRate").set_value(lpInfo->CriDmgRate);
		node.append_attribute("DoubleDmgRate").set_value(lpInfo->DoubleDmgRate);
		node.append_attribute("ResistDouble").set_value(lpInfo->ResistDouble);
		node.append_attribute("ResistIgnoreDef").set_value(lpInfo->ResistIgnoreDef);
		node.append_attribute("ResistIgnoreSD").set_value(lpInfo->ResistIgnoreSD);
		node.append_attribute("ResistCrit").set_value(lpInfo->ResistCrit);
		node.append_attribute("ResistExl").set_value(lpInfo->ResistExl);
		node.append_attribute("ResistStun").set_value(lpInfo->ResistStun);
		node.append_attribute("Reflect").set_value(lpInfo->Reflect);
		node.append_attribute("Time").set_value(lpInfo->Time);
		node.append_attribute("SetOption").set_value(lpInfo->SetOption);
	}

	// canli 2. dongu: ItemSet[] — Section YAZILMAZ (21 attribute)
	for (size_t i = 0; i < this->m_vItemSetData.size(); i++)
	{
		ConfigSetDataDamage* lpInfo = &this->m_vItemSetData[i];

		pugi::xml_node node = root.append_child("ItemSet");

		node.append_attribute("Index").set_value(lpInfo->Index);
		node.append_attribute("Effect").set_value(lpInfo->Effect);
		node.append_attribute("Lv").set_value(lpInfo->Lv);
		node.append_attribute("Opt").set_value(lpInfo->Opt);
		node.append_attribute("Dmg").set_value(lpInfo->Dmg);
		node.append_attribute("AddSD").set_value(lpInfo->AddSD);
		node.append_attribute("AddHP").set_value(lpInfo->AddHP);
		node.append_attribute("AddMP").set_value(lpInfo->AddMP);
		node.append_attribute("Def").set_value(lpInfo->Def);
		node.append_attribute("ExlDmgRate").set_value(lpInfo->ExlDmgRate);
		node.append_attribute("CriDmgRate").set_value(lpInfo->CriDmgRate);
		node.append_attribute("DoubleDmgRate").set_value(lpInfo->DoubleDmgRate);
		node.append_attribute("ResistDouble").set_value(lpInfo->ResistDouble);
		node.append_attribute("ResistIgnoreDef").set_value(lpInfo->ResistIgnoreDef);
		node.append_attribute("ResistIgnoreSD").set_value(lpInfo->ResistIgnoreSD);
		node.append_attribute("ResistCrit").set_value(lpInfo->ResistCrit);
		node.append_attribute("ResistExl").set_value(lpInfo->ResistExl);
		node.append_attribute("ResistStun").set_value(lpInfo->ResistStun);
		node.append_attribute("Reflect").set_value(lpInfo->Reflect);
		node.append_attribute("Time").set_value(lpInfo->Time);
		node.append_attribute("SetOption").set_value(lpInfo->SetOption);
	}

	file.save_file(path);	// canli: fopen(path,"wb") + doc.save(xml_writer_file) — varsayilan bicim (\t girinti)
}

void CustomSetDameItem::CalcCharacter(LPOBJ lpObj, bool flag)	// canli ?CalcCharacter @00478BF0
{
	if (flag != 0)	// canli: cmp byte ptr [ebp+0Ch],0 / jne epilog — uncalc yolu no-op
	{
		return;
	}

	for (size_t i = 0; i < this->m_vItemData.size(); i++)	// canli 1. dongu: Item satirlari kendi Section'iyla
	{
		this->CalcSlot(lpObj, &this->m_vItemData[i]);
	}

	for (size_t i = 0; i < this->m_vItemSetData.size(); i++)	// canli 2. dongu: satir basina Section=7..11
	{
		for (int section = 7; section <= 11; section++)
		{
			ConfigSetDataDamage info = this->m_vItemSetData[i];	// canli: 0x58 bayt kopya + ilk alan = section
			info.Section = section;
			this->CalcSlot(lpObj, &info);
		}
	}
}

void CustomSetDameItem::CalcSlot(LPOBJ lpObj, ConfigSetDataDamage* lpInfo)	// canli lambda @00478C90
{
	int Type = (lpInfo->Section * 512) + lpInfo->Index;	// canli: shl 9 + Index (GET_ITEM kodlamasi)

	for (int slot = 2; slot <= 11; slot++)	// canli 1F8h..0AD4h adim 0FCh = wear slot 2..11 (helm..ring2)
	{
		CItem* lpItem = &lpObj->Inventory[slot];

		if (lpItem->m_Index != Type)	// canli: movsx [item+4] cmp (Section<<9)+Index
		{
			continue;
		}

		if (lpItem->m_Level < lpInfo->Lv)	// canli: [item+6] < Lv -> atla
		{
			continue;
		}

		if (lpItem->m_NewOption < lpInfo->Opt)	// canli byte +9F (excellent bitmask; 63 = 6/6)
		{
			continue;
		}

		if (lpItem->m_SetOption != lpInfo->SetOption)	// canli byte +0B6 (canli config 0 ve 5)
		{
			continue;
		}

		// canli ekleme blogu (Dmg alti hasar alanina; AddSD/AddHP/AddMP/Def + dokuz rate + Reflect)
		lpObj->PhysiDamageMin += lpInfo->Dmg;
		lpObj->PhysiDamageMax += lpInfo->Dmg;
		lpObj->MagicDamageMin += lpInfo->Dmg;
		lpObj->MagicDamageMax += lpInfo->Dmg;
		lpObj->CurseDamageMin += lpInfo->Dmg;
		lpObj->CurseDamageMax += lpInfo->Dmg;

		lpObj->AddShield += lpInfo->AddSD;
		lpObj->AddLife += lpInfo->AddHP;
		lpObj->AddMana += lpInfo->AddMP;

		lpObj->Defense += lpInfo->Def;

		lpObj->ExcellentDamageRate += lpInfo->ExlDmgRate;
		lpObj->CriticalDamageRate += lpInfo->CriDmgRate;
		lpObj->DoubleDamageRate += lpInfo->DoubleDmgRate;
		lpObj->ResistDoubleDamageRate += lpInfo->ResistDouble;
		lpObj->ResistIgnoreDefenseRate += lpInfo->ResistIgnoreDef;
		lpObj->ResistIgnoreShieldGaugeRate += lpInfo->ResistIgnoreSD;
		lpObj->ResistCriticalDamageRate += lpInfo->ResistCrit;
		lpObj->ResistExcellentDamageRate += lpInfo->ResistExl;
		lpObj->ResistStunRate += lpInfo->ResistStun;
		lpObj->DamageReflect += lpInfo->Reflect;
	}
}
