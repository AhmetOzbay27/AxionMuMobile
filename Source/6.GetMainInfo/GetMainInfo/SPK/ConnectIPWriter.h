// ConnectIPWriter.h — Faz 2d.1 (D2): ConnectIP.bmd üreticisi.
//
// Format (canlı araç deneyleriyle KESİNLEŞTİ; docs/07 §1.3/§6/1 yanlış varsayımı düzeltildi):
//   decode (XOR 0x20 sonrası) 36 bayt:
//     0x00..0x0B : IpAddress dizesi (en çok 12 bayt; kısa dize kalanı 0x00)
//     0x0C..0x1F : 20 bayt 0x00 dolgu
//     0x20..0x21 : IpAddressPort (u16 LE)   <-- canlı 0xAD75 = 44405
//     0x22..0x23 : AntiPort (u16 LE)        <-- canlı 0xDA32 = 55858
//   Diske yazımdan önce TÜM 36 bayt XOR 0x20.
//
// NOT: docs/07'de "CRC32 (algoritma açık)" sanılan son 4 bayt aslında port çiftidir;
// hiçbir CRC yoktur. SPK_CRCFILE.ini'deki SPK_CIBMD ayrıdır (dosyanın tamamının
// standart CRC32'si — CrcFileReport üretir).

#pragma once
#include "GetEngineConfig.h"

// outPath: ör. "..\\Client\\Data\\SPK\\ConnectIP.bmd"
bool SPK_WriteConnectIP(const SPK_ENGINE_CONFIG* cfg, const char* outPath);
