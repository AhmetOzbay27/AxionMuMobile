// CrcFileReport.cpp — Faz 2d.1 (D6): SPK_CRCFILE.ini raporu (uygulama).
#include "stdafx.h"
#include "CrcFileReport.h"
#include <time.h>

// girdi dosyası kontrolleri (canlı rapor sırası; yollar dataRoot'a göreli)
static const char* SPK_INPUT_CHECKS[][2] = {
	{ "CustomPetEffect", "RenderEffect\\CustomPetEffect.txt" },
	{ "CustomPetGlow",   "RenderEffect\\CustomPetGlow.txt" },
	{ "JCItemToolTip",   "ItemToolTips\\JCItemToolTip.xml" },
	{ "JCTextTooltip",   "ItemToolTips\\JCTextTooltip.xml" },
	{ "CustomClaws",     "CustomClaws.txt" },
	{ "CustomPet",       "CustomPet.txt" },
	{ "CustomBowCross",  "CustomBowCross.txt" },
	{ "CustomItem",      "CustomItem.txt" },
	{ "CustomWing",      "CustomWing.txt" },
};

// Config\Info bmd kontrolleri (canlı rapor sırası; CustomMonster iki kez — parite)
static const char* SPK_CONFIG_INFO_CHECKS[] = {
	"CustomMonster.bmd", "RenderEffect.bmd", "CustomIconBuff.bmd", "CustomItemColorName.bmd",
	"CustomItemPosition.bmd", "CustomJewel.bmd", "CustomModelNpc.bmd", "CustomMonster.bmd",
	"CustomMonsterGold.bmd", "CustomNpcName.bmd", "CustomRingPen.bmd", "CustomSetEffect.bmd",
};

static const char* SPK_WEEKDAYS_EN[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
};

static bool SPK_FileExists(const char* path)
{
	return (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
}

// canlı araç biçimi: 0 -> "0x0"; aksi halde 8 haneli büyük harf hex
static void SPK_FormatCrc(char* out, int outSize, unsigned long crc)
{
	if (crc == 0)
	{
		strcpy_s(out, outSize, "0x0");
	}
	else
	{
		sprintf_s(out, outSize, "0x%08X", crc);
	}
}

bool SPK_WriteCrcReport(const char* clientRoot, const char* dataRoot, const char* outPath,
	unsigned long crcMEXE, unsigned long crcPBMD, unsigned long crcSDBMD, unsigned long crcCIBMD)
{
	FILE* fp = 0;

	if (fopen_s(&fp, outPath, "wb") != 0 || fp == 0)
	{
		return false;
	}

	char line[512];
	char path[MAX_PATH];

	fputs(";==============================================================\r\n", fp);
	fputs(";========================= SUPERKHUNG =========================\r\n", fp);
	fputs(";==============================================================\r\n", fp);
	fputs(";Crc File Checked Main 5.2! Code by SuperKhung [0775.838.858]\r\n", fp);
	fputs(";==============================================================\r\n", fp);

char crc[16];

	SPK_FormatCrc(crc, sizeof(crc), crcMEXE);
	sprintf_s(line, "%-36s= %s\r\n", "SPK_MEXE", crc); fputs(line, fp);

	SPK_FormatCrc(crc, sizeof(crc), crcPBMD);
	sprintf_s(line, "%-36s= %s\r\n", "SPK_PBMD", crc); fputs(line, fp);

	SPK_FormatCrc(crc, sizeof(crc), crcSDBMD);
	sprintf_s(line, "%-36s= %s\r\n", "SPK_SDBMD", crc); fputs(line, fp);

	SPK_FormatCrc(crc, sizeof(crc), crcCIBMD);
	sprintf_s(line, "%-36s= %s\r\n", "SPK_CIBMD", crc); fputs(line, fp);

	fputs(";==============================================================\r\n", fp);

	for (int i = 0; i < (int)(sizeof(SPK_INPUT_CHECKS) / sizeof(SPK_INPUT_CHECKS[0])); i++)
	{
		sprintf_s(path, "%s\\%s", dataRoot, SPK_INPUT_CHECKS[i][1]);
		sprintf_s(line, "%-36s= %s!\r\n", SPK_INPUT_CHECKS[i][0],
			SPK_FileExists(path) ? "FOUND OK" : "NOT FOUND");
		fputs(line, fp);
	}

	fputs(";==============================================================\r\n", fp);

	for (int i = 0; i < (int)(sizeof(SPK_CONFIG_INFO_CHECKS) / sizeof(SPK_CONFIG_INFO_CHECKS[0])); i++)
	{
		sprintf_s(path, "%s\\Data\\SPK\\Config\\Info\\%s", clientRoot, SPK_CONFIG_INFO_CHECKS[i]);
		sprintf_s(line, "%-36s= %s!\r\n", SPK_CONFIG_INFO_CHECKS[i],
			SPK_FileExists(path) ? "FOUND OK" : "NOT FOUND");
		fputs(line, fp);
	}

	fputs(";==============================================================\r\n", fp);

	{
		time_t now = time(0);
		struct tm local;
		localtime_s(&local, &now);

		int wday = local.tm_wday;

		if (wday < 0 || wday > 6)
		{
			wday = 0;
		}

		sprintf_s(line, "; %s, %02d:%02d:%02d %02d/%02d/%04d\r\n",
			SPK_WEEKDAYS_EN[wday], local.tm_hour, local.tm_min, local.tm_sec,
			local.tm_mday, local.tm_mon + 1, local.tm_year + 1900);
		fputs(line, fp);
	}

	fputs(";==============================================================\r\n", fp);

	fclose(fp);

	return true;
}
