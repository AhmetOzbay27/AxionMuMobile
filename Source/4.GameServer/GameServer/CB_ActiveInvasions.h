#pragma once
#include "Protocol.h"
#if(CB_ActiveInvasionsD)
// =============================================================================
// 2c.1-B3 (01.10.2026) — CB_ActiveInvasions canlı parite denetimi
// Canli kanit: GameServer_canli.map -> ActiveInvasions.obj
//   ?monster_add@CActiveInvasions@@QAEXH_N@Z        (0x41E950)
//   ?update_by_monster_id@CActiveInvasions@@QAEXH@Z (0x41EA80)
//   ?send_list_to_client@CActiveInvasions@@QBEXH@Z  (0x41EB20)
//   ?gActiveInvasions@@3VCActiveInvasions@@A        (0xAA31CC)
//   std::map<int,UInvasionsData> (agac kodu tek TU'da)
// Donor kaynakta bu modul YOK (MUIG ozel; docs/13 "sifirdan" etiketi).
// Bu tur: bizim ilk-commit'ten beri duran CB_ActiveInvasions taslagi canli
// disasm'a karsi denetlendi; farklar (paket basliklari D3->F3, monster_del
// max_count azaltma, monster_add broadcast imzasi, tekil liste istegi
// F7 sub2) canliya cekildi.
// =============================================================================

// Canli update_by_monster_id (0x41EA80): C1 10 F3 99 {int monster_id, int count, int 0}
struct PMSG_ACTIVE_INVASIONS_UPDATE_SEND
{
	PSBMSG_HEAD header; // C1 10 F3 99
	int monster_id;
	int count;
	int spare;
};

// Canli send_list_to_client (0x41EB20): C2 [size] F3 98 {BYTE count, N x 12 B}
struct PMSG_ACTIVE_INVASIONS_SEND
{
	PSWMSG_HEAD header; // C2 F3 98
	BYTE count;
};

// Canli map degeri (UInvasionsData, _Tree kodundan): iki int sayaç
// {canli_sayaç, maksimum/kayit_sayaci} — 12 B'lik liste elemaninin ikinci
// 8 Bayti (disasm 0x41EB70-0x41EB85: [+10]=id, [+14], [+18]).
struct UInvasionsData
{
	UInvasionsData(int count,int max_count) : count(count), max_count(max_count) {}
	UInvasionsData() : count(0), max_count(0) {}
	int count;
	int max_count;
};

struct PMSG_ACTIVE_INVASIONS
{
	int monster_id;
	UInvasionsData count;
};

class CB_ActiveInvasions
{
public:
	CB_ActiveInvasions();
	void monster_add(int monster_id,bool send_update);			// 2c.1-B3: canli imza (bool broadcast)
	void monster_del(int monster_id,bool send_update = false);
	void send_list_to_client();									// herkese (giris push + eski toplu cagrilari)
	void send_list_to_client(int aIndex);						// 2c.1-B3: canli tekil imza (F7 sub 0x02 istegi)
	void update_by_monster_id(int monster_id);
private:
	std::map<int,UInvasionsData> m_data;
}; extern CB_ActiveInvasions gCB_ActiveInvasions;

#endif
