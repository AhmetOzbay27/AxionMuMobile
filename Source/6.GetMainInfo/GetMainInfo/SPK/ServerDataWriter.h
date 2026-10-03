// ServerDataWriter.h — Faz 2d.1 (D3): ServerData.bmd üreticisi (şablon tabanlı).
//
// Format: 1.089.576 B; tüm baytlar diske yazılırken XOR 0x20 (decode = okunur hal).
//
// Bu sürüm "şablon kopya + ini yaması" stratejisini uygular (docs/08 §3.4):
//   - ini'den türetilen TÜM alanlar bayt-birebir yazılır (aşağıdaki tablo);
//   - kanat/item/LEVEL katalogları ve bilinmeyen bloklar şablondan aynen korunur
//     (D4/D5 tam jeneratör işi — sıradaki dalga).
//
// Kanıtlı ini→offset haritası (canlı araç sentinel deneyleri, BuildLog/2d1):
//   0x200/0x220/0x240/0x260  ServerName_1..4  (32 B slot, 0 doldurmalı)
//   0x2A0 ClientName[32]; 0x2C0 CustomerName[32]; 0x2E0 WindowName[32]
//   0x3E0 ScreenShotPath[64]
//   0x4E0 ClientVersion[8]; 0x4E8 ClientSerial[16]
//   0x4F9..0x50C MENU_BUTTON_01..20 (byte)
//   0x50D ButtonCharracter; 0x50E JwBless; 0x50F JwSoul; 0x510 JwChaos;
//   0x511 WcoinC; 0x512 WcoinP; 0x513 WcoinG; 0x515 Zens
//   0x516..0x51C Ranking1..7; 0x51D SkillManaPet; 0x51E EnableCoinTitle; 0x51F TextTips3Line
//   0x521 RF_GLOVE; 0x522 MG_HELM; 0x524 CreateCharSeason; 0x525 ButtonClassUP; 0x526 JewelBankTab
//   0x52B MaxLevelDanhHieu; 0x52C MaxLevelQuanHam; 0x52D MaxLevelTuChan;
//   0x52E MaxLevelHonHoan; 0x52F MaxGameInstances
//   0x530..0x54B 7×int32 MaxAttackSpeed (DW,DK,FE,MG,DL,SU,RF)
//   0x54C ReconnectTime (byte)
//   0x554 u32 = CRC32(<clientRoot>\<ClientName>) — dosya yoksa 0 (canlı araç davranışı)
//   0x558 float CameraDefault; 0x55C float DefaultFPS (ham değer, ×10 DEĞİL)
//
// Açık kalemler (doküman: docs/20): 0x514, 0x520, 0x523, 0x527..0x52A vb. rezerve
// baytlar şablondan gelir; D4/D5 sonrası tam jeneratör hedefi.

#pragma once
#include "GetEngineConfig.h"

// templatePath : tam boyutlu kaynak (D4/D5'e kadar zorunlu)
// clientRoot   : "..\\Client" (0x554 CRC'si ve rapor yolları bu kökten)
// outPath      : "..\\Client\\Data\\SPK\\ServerData.bmd"
// outFileCrc   : yazılan (encode edilmiş) dosyanın CRC32'si — rapor için
bool SPK_WriteServerData(const SPK_ENGINE_CONFIG* cfg, const char* templatePath,
	const char* clientRoot, const char* outPath, unsigned long* outFileCrc);
