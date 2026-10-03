// CrcFileReport.h — Faz 2d.1 (D6): SPK_CRCFILE.ini raporu.
//
// Biçim canlı örnekten birebir alındı (BuildLog/2d1: canlı rapor + sandbox koşuları):
//   ;==============================================================
//   ;========================= SUPERKHUNG =========================
//   ;==============================================================
//   ;Crc File Checked Main 5.2! Code by SuperKhung [0775.838.858]
//   ;==============================================================
//   SPK_MEXE                            = 0x%X
//   SPK_PBMD                            = 0x%X
//   SPK_SDBMD                           = 0x%X
//   SPK_CIBMD                           = 0x%X
//   ;==============================================================
//   <9 girdi dosyası>                   = FOUND OK! / NOT FOUND!
//   ;==============================================================
//   <12 Config\Info bmd kontrolü>       = FOUND OK! / NOT FOUND!
//   ;==============================================================
//   ; <Weekday>, HH:MM:SS dd/mm/yyyy      (İngilizce gün adı)
//   ;==============================================================
//
// Kontrol listeleri canlı aracın string tablosu ve rapor çıktısıyla birebir
// (CustomMonster.bmd listede İKİ KEZ geçer — canlı davranışı korunur).

#pragma once

// clientRoot : "..\\Client"      (Config\Info kontrolleri + rapor CRC'leri bu kökten)
// dataRoot   : ".\\Data"         (girdi txt/xml kontrolleri)
// outPath    : ".\\SPK_CRCFILE.ini"
bool SPK_WriteCrcReport(const char* clientRoot, const char* dataRoot, const char* outPath,
	unsigned long crcMEXE, unsigned long crcPBMD, unsigned long crcSDBMD, unsigned long crcCIBMD);
