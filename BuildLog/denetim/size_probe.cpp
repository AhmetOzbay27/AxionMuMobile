#include "stdafx.h"
#include "WSclient.h"
template <int N> struct ShowSize;
ShowSize<sizeof(PCREATE_CHARACTER)> probe_character;
ShowSize<sizeof(PCREATE_TRANSFORM)> probe_transform;
ShowSize<sizeof(PCREATE_SUMMON)>    probe_summon;
ShowSize<sizeof(PCREATE_MONSTER)>   probe_monster;
ShowSize<MAX_BUFF_SLOT_INDEX>       probe_buffidx;
ShowSize<MAX_ID_SIZE>               probe_idsize;
ShowSize<EQUIPMENT_LENGTH>          probe_equiplen;
