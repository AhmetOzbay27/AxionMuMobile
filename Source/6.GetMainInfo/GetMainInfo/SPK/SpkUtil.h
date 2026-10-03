// SpkUtil.h — Faz 2d.1 ortak yardımcıları: CRC32 (SPK_CRCFILE standardı) + tam dosya IO.
//
// CRC standardı kanıtı: canlı SPK_CRCFILE.ini değerleri standart CRC32'dir
// (poly 0xEDB88320, init/final 0xFFFFFFFF) — ConnectIP.bmd ve ServerData.bmd için
// zlib/CCRC32 ile birebir doğrulandı (BuildLog/2d1).

#pragma once

bool SPK_ReadAll(const char* path, unsigned char* buffer, int size);						// tam boyut şart
bool SPK_WriteAll(const char* path, const unsigned char* buffer, int size);
bool SPK_FileCrc32(const char* path, unsigned long* outCrc);								// dosya yoksa false
void SPK_BufferCrc32(const unsigned char* data, int size, unsigned long* outCrc);
