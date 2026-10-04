// 04.10.2026 (docs/35): Main "Global Release" projesinin GERCEK derleme
// kosullarinda (ek /D override yok — proje tlog'u kanit) istemci viewport tel
// duzeninin canli SPK 5.2 (GAMESERVER_UPDATE=603 + GAMESERVER_HAISLOTRING=0)
// oldugunu derleme zamaninda kanitlar.
//
// Istemci sozlesmesi (WSclient.cpp): sabit govde + s_BuffCount kadar buff
// kuyrugu; kayit ilerletmesi sizeof(struct)-(MAX_BUFF_SLOT_INDEX-count).
// Bu yuzden sifir-buff tel boyu = sizeof(struct) - MAX_BUFF_SLOT_INDEX'tir.
// Referans: canli GameServer.pdb -> PLAYER 36 / CHANGE 38 / MONSTER 20 /
// SUMMON 20; count@+35/+37/+16/+19; CurHp@+9, Level@+10, Life@+12
// (BuildLog/2e7/live_pdb_viewport_layout.txt).
#include "stdafx.h"
#include "WSclient.h"
#include <cstddef>

#if GAMESERVER_UPDATE != 603
#error GAMESERVER_UPDATE_is_not_603
#endif
#if GAMESERVER_HAISLOTRING != 0
#error GAMESERVER_HAISLOTRING_is_not_0
#endif

// Tel govde boylari (sifir buff) = canli sunucu kayit boylari
static_assert(sizeof(PCREATE_CHARACTER) - MAX_BUFF_SLOT_INDEX == 36, "PLAYER tel boyu 36 olmali");
static_assert(sizeof(PCREATE_TRANSFORM) - MAX_BUFF_SLOT_INDEX == 38, "CHANGE tel boyu 38 olmali");
static_assert(sizeof(PCREATE_SUMMON)  - MAX_BUFF_SLOT_INDEX == 20, "SUMMON tel boyu 20 olmali");
static_assert(sizeof(PCREATE_MONSTER) - MAX_BUFF_SLOT_INDEX == 20, "MONSTER tel boyu 20 olmali");

// Canli PDB count ofsetleri
static_assert(offsetof(PCREATE_CHARACTER, s_BuffCount) == 35, "PLAYER count@+35");
static_assert(offsetof(PCREATE_TRANSFORM, s_BuffCount) == 37, "CHANGE count@+37");
static_assert(offsetof(PCREATE_SUMMON, s_BuffCount) == 19, "SUMMON count@+19");
static_assert(offsetof(PCREATE_MONSTER, s_BuffCount) == 16, "MONSTER count@+16");
// Canli PDB MONSTER alan ofsetleri (603 dali)
static_assert(offsetof(PCREATE_MONSTER, CurHp) == 9, "MONSTER CurHp@+9");
static_assert(offsetof(PCREATE_MONSTER, Level) == 10, "MONSTER Level@+10");
static_assert(offsetof(PCREATE_MONSTER, Life) == 12, "MONSTER Life@+12");

int verify_603_layout_ok = GAMESERVER_UPDATE + GAMESERVER_HAISLOTRING;
