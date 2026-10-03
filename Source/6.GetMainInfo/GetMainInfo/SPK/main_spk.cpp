// main_spk.cpp — Faz 2d.1 (D7): SPK modu giriş noktası (uygulama).
#include "stdafx.h"
#include "main_spk.h"
#include "GetEngineConfig.h"
#include "ConnectIPWriter.h"
#include "ServerDataWriter.h"
#include "CrcFileReport.h"
#include "SpkUtil.h"

#define SPK_CONNECTIP_SIZE	36
#define SPK_SERVERDATA_SIZE	1089576

static bool SPK_MakeDir(const char* path)
{
	char temp[MAX_PATH];
	strncpy_s(temp, path, _TRUNCATE);

	for (char* p = temp; *p != 0; p++)
	{
		if (*p == '\\' || *p == '/')
		{
			char save = *p;
			*p = 0;

			if (strlen(temp) > 0)
			{
				CreateDirectoryA(temp, 0);
			}

			*p = save;
		}
	}

	return (CreateDirectoryA(temp, 0) != false || GetLastError() == ERROR_ALREADY_EXISTS);
}

static bool SPK_CopyFile(const char* src, const char* dst)
{
	BYTE buffer[65536];
	FILE* in = 0;
	FILE* out = 0;

	if (fopen_s(&in, src, "rb") != 0 || in == 0)
	{
		return false;
	}

	if (fopen_s(&out, dst, "wb") != 0 || out == 0)
	{
		fclose(in);
		return false;
	}

	size_t read;

	while ((read = fread(buffer, 1, sizeof(buffer), in)) > 0)
	{
		if (fwrite(buffer, 1, read, out) != read)
		{
			fclose(in);
			fclose(out);
			return false;
		}
	}

	fclose(in);
	fclose(out);

	return true;
}

// beklenen slot icerigi ile karsilastirma (0 doldurmali)
static bool SPK_CheckSlot(BYTE* buffer, int offset, int slotSize, const char* text)
{
	BYTE expected[64];

	memset(expected, 0, sizeof(expected));

	int len = (int)strlen(text);

	if (len > slotSize)
	{
		len = slotSize;
	}

	memcpy(expected, text, len);

	return (memcmp(buffer + offset, expected, slotSize) == 0);
}

static void SPK_CheckLine(const char* name, bool ok, int* failCount)
{
	printf("[%s] %s\n", ok ? "OK  " : "FAIL", name);

	if (ok == false)
	{
		(*failCount)++;
	}
}

// --check: mevcut ciktilari ini ile dogrula
static int SPK_CheckOutputs(const SPK_ENGINE_CONFIG* cfg, const char* outPath)
{
	char path[MAX_PATH];
	int failCount = 0;

	BYTE connectIp[SPK_CONNECTIP_SIZE];

	sprintf_s(path, "%s\\ConnectIP.bmd", outPath);

	if (SPK_ReadAll(path, connectIp, sizeof(connectIp)) == false)
	{
		printf("[FAIL] ConnectIP.bmd okunamadi: %s\n", path);
		return 4;
	}

	for (int i = 0; i < SPK_CONNECTIP_SIZE; i++)
	{
		connectIp[i] ^= 0x20;
	}

	SPK_CheckLine("ConnectIP: IP", SPK_CheckSlot(connectIp, 0, 12, cfg->IpAddress), &failCount);

	WORD port = 0, anti = 0;
	memcpy(&port, connectIp + 0x20, 2);
	memcpy(&anti, connectIp + 0x22, 2);

	SPK_CheckLine("ConnectIP: IpAddressPort", port == cfg->IpAddressPort, &failCount);
	SPK_CheckLine("ConnectIP: AntiPort", anti == cfg->AntiPort, &failCount);

	BYTE* serverData = new BYTE[SPK_SERVERDATA_SIZE];

	sprintf_s(path, "%s\\ServerData.bmd", outPath);

	if (SPK_ReadAll(path, serverData, SPK_SERVERDATA_SIZE) == false)
	{
		delete[] serverData;
		printf("[FAIL] ServerData.bmd okunamadi: %s\n", path);
		return 4;
	}

	for (int i = 0; i < SPK_SERVERDATA_SIZE; i++)
	{
		serverData[i] ^= 0x20;
	}

	{
		bool ok = true;

		for (int i = 0; i < SPK_MAX_SERVER; i++)
		{
			ok = ok && SPK_CheckSlot(serverData, 0x200 + (i * 0x20), 32, cfg->ServerName[i]);
		}

		SPK_CheckLine("ServerData: ServerName_1..4", ok, &failCount);
	}

	SPK_CheckLine("ServerData: ClientName", SPK_CheckSlot(serverData, 0x2A0, 32, cfg->ClientName), &failCount);
	SPK_CheckLine("ServerData: CustomerName", SPK_CheckSlot(serverData, 0x2C0, 32, cfg->CustomerName), &failCount);
	SPK_CheckLine("ServerData: WindowName", SPK_CheckSlot(serverData, 0x2E0, 32, cfg->WindowName), &failCount);
	SPK_CheckLine("ServerData: ScreenShotPath", SPK_CheckSlot(serverData, 0x3E0, 64, cfg->ScreenShotPath), &failCount);
	SPK_CheckLine("ServerData: ClientVersion", SPK_CheckSlot(serverData, 0x4E0, 8, cfg->ClientVersion), &failCount);
	SPK_CheckLine("ServerData: ClientSerial", SPK_CheckSlot(serverData, 0x4E8, 16, cfg->ClientSerial), &failCount);

	{
		bool ok = true;

		for (int i = 0; i < SPK_MAX_MENU; i++)
		{
			ok = ok && (serverData[0x4F9 + i] == (BYTE)(cfg->Menu[i] & 0xFF));
		}

		SPK_CheckLine("ServerData: MENU_BUTTON_01..20", ok, &failCount);
	}

	{
		float camera = 0.0f, fps = 0.0f;
		memcpy(&camera, serverData + 0x558, 4);
		memcpy(&fps, serverData + 0x55C, 4);

		SPK_CheckLine("ServerData: CameraDefault", camera == cfg->CameraDefault, &failCount);
		SPK_CheckLine("ServerData: DefaultFPS", fps == cfg->DefaultFPS, &failCount);
	}

	{
		bool ok = true;

		for (int i = 0; i < SPK_MAX_SPEED; i++)
		{
			DWORD value = 0;
			memcpy(&value, serverData + 0x530 + (i * 4), 4);
			ok = ok && (value == cfg->MaxAttackSpeed[i]);
		}

		SPK_CheckLine("ServerData: MaxAttackSpeed x7", ok, &failCount);
	}

	delete[] serverData;

	printf("--check sonucu: %d hata\n", failCount);

	return (failCount == 0) ? 0 : 4;
}

int SPKMain(int argc, char** argv)
{
	char iniPath[MAX_PATH] = ".\\GetEngine.ini";
	char clientRoot[MAX_PATH] = "..\\Client";
	char dataRoot[MAX_PATH] = ".\\Data";
	char outPath[MAX_PATH] = "";
	char templatePath[MAX_PATH] = ".\\ServerData.template.bmd";
	char androidPath[MAX_PATH] = "..\\Source\\ExMain_SPK\\android\\app\\src\\main\\assets\\spk";
	char reportPath[MAX_PATH] = ".\\SPK_CRCFILE.ini";
	bool checkOnly = false;

	for (int i = 1; i < argc; i++)
	{
		const char* arg = argv[i];

		if (_strnicmp(arg, "--ini:", 6) == 0)			strncpy_s(iniPath, arg + 6, _TRUNCATE);
		else if (_strnicmp(arg, "--client:", 9) == 0)	strncpy_s(clientRoot, arg + 9, _TRUNCATE);
		else if (_strnicmp(arg, "--data:", 7) == 0)		strncpy_s(dataRoot, arg + 7, _TRUNCATE);
		else if (_strnicmp(arg, "--out:", 6) == 0)		strncpy_s(outPath, arg + 6, _TRUNCATE);
		else if (_strnicmp(arg, "--template:", 11) == 0) strncpy_s(templatePath, arg + 11, _TRUNCATE);
		else if (_strnicmp(arg, "--android:", 10) == 0)	strncpy_s(androidPath, arg + 10, _TRUNCATE);
		else if (_strnicmp(arg, "--report:", 9) == 0)	strncpy_s(reportPath, arg + 9, _TRUNCATE);
		else if (_stricmp(arg, "--check") == 0)			checkOnly = true;
		else if (_stricmp(arg, "--mode:spk") == 0)		/* varsayilan */;
		else if (_stricmp(arg, "--help") == 0 || _stricmp(arg, "-h") == 0)
		{
			printf("GetMainInfo SPK modu (2d.1):\n"
				"  --ini:<path>      GetEngine.ini (varsayilan .\\GetEngine.ini)\n"
				"  --client:<path>   istemci koku (varsayilan ..\\Client)\n"
				"  --data:<path>     veri klasoru (varsayilan .\\Data)\n"
				"  --out:<path>      cikti klasoru (varsayilan <client>\\Data\\SPK)\n"
				"  --template:<path> ServerData sablonu (varsayilan .\\ServerData.template.bmd)\n"
				"  --android:<path>  Android asset klasoru (varsa kopyalanir)\n"
				"  --report:<path>   SPK_CRCFILE.ini (varsayilan .\\SPK_CRCFILE.ini)\n"
				"  --check           uretmeden dogrula\n");
			return 0;
		}
		else
		{
			printf("[UYARI] Bilinmeyen arguman yoksayildi: %s\n", arg);
		}
	}

	if (outPath[0] == 0)
	{
		sprintf_s(outPath, "%s\\Data\\SPK", clientRoot);
	}

	SPK_ENGINE_CONFIG cfg;

	if (SPK_LoadEngineConfig(iniPath, &cfg) == false)
	{
		printf("[HATA] GetEngine.ini bulunamadi: %s\n", iniPath);
		return 1;
	}

	if (checkOnly != false)
	{
		return SPK_CheckOutputs(&cfg, outPath);
	}

	// şablon çözümü: açık yol -> varsayılan dosya -> mevcut çıktı (D4/D5'e kadar)
	if (GetFileAttributesA(templatePath) == INVALID_FILE_ATTRIBUTES)
	{
		char existing[MAX_PATH];
		sprintf_s(existing, "%s\\ServerData.bmd", outPath);

		if (GetFileAttributesA(existing) != INVALID_FILE_ATTRIBUTES)
		{
			strncpy_s(templatePath, existing, _TRUNCATE);
		}
		else
		{
			printf("[HATA] ServerData sablonu yok: %s\n", templatePath);
			printf("       D4/D5 tam jeneratoru gelene kadar sablon gerekir (docs/20).\n");
			return 2;
		}
	}

	SPK_MakeDir(outPath);

	char connectIpOut[MAX_PATH];
	sprintf_s(connectIpOut, "%s\\ConnectIP.bmd", outPath);

	if (SPK_WriteConnectIP(&cfg, connectIpOut) == false)
	{
		printf("[HATA] ConnectIP.bmd yazilamadi: %s\n", connectIpOut);
		return 3;
	}

	char serverDataOut[MAX_PATH];
	sprintf_s(serverDataOut, "%s\\ServerData.bmd", outPath);

	unsigned long crcSDBMD = 0;

	if (SPK_WriteServerData(&cfg, templatePath, clientRoot, serverDataOut, &crcSDBMD) == false)
	{
		printf("[HATA] ServerData.bmd yazilamadi (sablon: %s)\n", templatePath);
		return 3;
	}

	// rapor CRC'leri
	unsigned long crcCIBMD = 0, crcMEXE = 0, crcPBMD = 0;
	char crcPath[MAX_PATH];

	SPK_FileCrc32(connectIpOut, &crcCIBMD);

	sprintf_s(crcPath, "%s\\Engine.exe", clientRoot);

	if (SPK_FileCrc32(crcPath, &crcMEXE) == false)
	{
		crcMEXE = 0;
	}

	sprintf_s(crcPath, "%s\\Data\\Player\\player.bmd", clientRoot);

	if (SPK_FileCrc32(crcPath, &crcPBMD) == false)
	{
		crcPBMD = 0;
	}

	if (SPK_WriteCrcReport(clientRoot, dataRoot, reportPath, crcMEXE, crcPBMD, crcSDBMD, crcCIBMD) == false)
	{
		printf("[UYARI] SPK_CRCFILE.ini yazilamadi: %s\n", reportPath);
	}

	// Android kopyası (klasör varsa; canlı araç da koşullu kopyalar)
	if (GetFileAttributesA(androidPath) != INVALID_FILE_ATTRIBUTES)
	{
		char androidFile[MAX_PATH];

		sprintf_s(androidFile, "%s\\connectip.bmd", androidPath);
		SPK_CopyFile(connectIpOut, androidFile);

		sprintf_s(androidFile, "%s\\serverdata.bmd", androidPath);
		SPK_CopyFile(serverDataOut, androidFile);
	}

	printf("[OK] ConnectIP.bmd  : %s (CRC 0x%X)\n", connectIpOut, crcCIBMD);
	printf("[OK] ServerData.bmd : %s (CRC 0x%X)\n", serverDataOut, crcSDBMD);
	printf("[OK] Rapor          : %s (MEXE 0x%X, PBMD 0x%X)\n", reportPath, crcMEXE, crcPBMD);
	printf("[BILGI] Sablon      : %s (D4/D5 tam jenerator bekliyor)\n", templatePath);

	return 0;
}
