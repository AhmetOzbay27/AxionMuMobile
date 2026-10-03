// SPK_CustomItemSetPro.h: interface for the CustomSetDameItem class (2c.1-A3, canli kanittan).
//
// Canli kanit (CustomItemSetPro.obj; GameServer_canli.map 1542-1553 + live_disasm 149094-153060):
//   ?Instance@CustomSetDameItem@@SAPAV1@XZ                       @004765D0
//   ?Load@CustomSetDameItem@@QAEXPAD@Z                           @00476640  pugixml, yol 'SPK\CustomItemSetPro.xml'
//   ?Save@CustomSetDameItem@@QAEXPAD@Z                           @00477360  ItemData kok: Item[] + ItemSet[]
//   ?CalcCharacter@CustomSetDameItem@@QAEXPAUOBJECTSTRUCT@@_N@Z  @00478BF0  flag != 0 -> erken donus
//   Yukleyici zinciri (0x569B78 Instance / 0x569BAE Load): canli okuma sirasi
//     CustomHarmony -> SystemItemChanger -> MoveOptionNew -> [CustomSetDameItem] -> CustomBuyVip -> CustomDameItem.
//   Karakter zinciri (tek cagri 0x5424D5, push 0): SocketItemOption -> MasterSkillTree ->
//     [CustomSetDameItem] -> CustomPet -> CustomStartItemDame -> CustomStartSetItemDame -> DanhHieu ...
//   Veri modeli: std::vector<ConfigSetDataDamage> x2 (canli this+0 'Item', this+0Ch 'ItemSet');
//     eleman 0x58 bayt = 22 int; alan sirasi XML/canli listeyle birebir.
//   Lambda @00478C90 (CalcSlot): wear taramasi slot 2..11 (canli 1F8h..0AD4h, adim 0FCh = CItem boyu;
//     C380ItemOption @00404A21 ayni taban [obj+3A8h] + 0FCh adim dogrulamasi). Kosullar:
//     m_Index == Section*512+Index (canli shl 9), m_Level >= Lv, m_NewOption >= Opt (canli byte +9F; 63 = 6/6 exc),
//     m_SetOption == SetOption (canli byte +0B6). Eslesen her yuva icin alan EKLEMELERI (Dmg alti hasar alanina).
//   ItemSet dugumleri Section'siz yuklenir (canli: [ebp-70h] acikca 0); CalcCharacter her ItemSet satirini
//     Section=7..11 (helm..boots) varyantlariyla bes kez uygular (canli birebir).
//   SPK GUI editoru (SPK_CustomItemSetProProc @004796A0) kapsam disi (A1/A2 ile ayni karar).
//
// Canli config deploy: MuServer\4.GameServer\Sub 1\Data\SPK\CustomItemSetPro.xml
//   (92 Item + 84 ItemSet satiri, bayt-birebir kopya).

#pragma once

#include "stdafx.h"
#include "User.h"
#include "Item.h"
#include <vector>

struct ConfigSetDataDamage	// canli UConfigSetDataDamage (0x58 bayt = 22 int)
{
	int Section;			// +00 canli Item satirlarinda; ItemSet satirlarinda 0 (dugumde yok)
	int Index;				// +04
	int Effect;				// +08 (CalcCharacter kullanmaz; canli 205 degerleri Time/SetOption ile birlikte)
	int Lv;					// +0C item level alt siniri (m_Level >= Lv)
	int Opt;				// +10 m_NewOption alt siniri (canli tum satirlar 63 = 6/6 excellent)
	int Dmg;				// +14 alti hasar alanina eklenir (Physi/Magic/Curse Min+Max)
	int AddSD;				// +18 -> AddShield
	int AddHP;				// +1C -> AddLife
	int AddMP;				// +20 -> AddMana
	int Def;				// +24 -> Defense
	int ExlDmgRate;			// +28 -> ExcellentDamageRate
	int CriDmgRate;			// +2C -> CriticalDamageRate
	int DoubleDmgRate;		// +30 -> DoubleDamageRate
	int ResistDouble;		// +34 -> ResistDoubleDamageRate
	int ResistIgnoreDef;	// +38 -> ResistIgnoreDefenseRate
	int ResistIgnoreSD;		// +3C -> ResistIgnoreShieldGaugeRate
	int ResistCrit;			// +40 -> ResistCriticalDamageRate
	int ResistExl;			// +44 -> ResistExcellentDamageRate
	int ResistStun;			// +48 -> ResistStunRate
	int Reflect;			// +4C -> DamageReflect
	int Time;				// +50 (CalcCharacter kullanmaz; canli editor alani)
	int SetOption;			// +54 m_SetOption esitlik kosulu (canli 0 ve 5)
};

class CustomSetDameItem
{
public:
	static CustomSetDameItem* Instance();	// canli ?Instance@CustomSetDameItem@@SAPAV1@XZ @004765D0

	void Load(char* path);					// canli ?Load (pugixml; Item[] + ItemSet[])
	void Save(char* path);					// canli ?Save (ItemData; Item[] Section dahil, ItemSet[] Section'siz)
	void CalcCharacter(LPOBJ lpObj, bool flag);	// canli ?CalcCharacter @00478BF0 (flag != 0 -> no-op)

	std::vector<ConfigSetDataDamage> m_vItemData;		// canli this+0
	std::vector<ConfigSetDataDamage> m_vItemSetData;	// canli this+0Ch

private:
	void CalcSlot(LPOBJ lpObj, ConfigSetDataDamage* lpInfo);	// canli lambda @00478C90
};

extern CustomSetDameItem gCustomSetDameItem;
