// SPKData.cpp: implementation of the CSPKData class (Faz 2d.0).
//
// Canli SPK istemcisi (Engine.exe) veri hatti; format: docs/07-SPK-BMD-FORMAT.md.
// ConnectIP.bmd: 36 B = 12 B IP (XOR 0x20) + 20 B dolgu + 4 B CRC — CRC alani
//   okunmaz (algoritma canli Engine.exe'de; docs/07 §6/1 acik kalem).
// ServerData.bmd: 1.089.576 B; tumu XOR 0x20; header alanlari docs/07 §3a.
// SPK.ini: [Version] MainCode + [FontConfig] + [SPK] (canli ornek dosyadan).
// Kapsam disi: kanat/item opsiyon tuple'lari ve LEVEL tablolari (asset katmani;
// 2d.0 sonrasi), ConnectIP CRC dogrulamasi, Config<Lang> varyant secimi.

#include "stdafx.h"
#include "SPKData.h"

CSPKData gSPKData;

static const BYTE SPK_XOR_KEY = 0x20;

// ---------------------------------------------------------------------------
// yardimcilar
// ---------------------------------------------------------------------------

static bool SPK_IsPrintText(char* text)
{
	for (int i = 0; text[i] != 0; i++)
	{
		if ((BYTE)text[i] < 0x20 || (BYTE)text[i] > 0x7E)
		{
			return false;
		}
	}

	return true;
}

// slot icinden NUL'a kadar okur; slot sonu bosluklari kirpar (canli slotlar 0x00 dolgulu)
static void SPK_ReadSlot(BYTE* buffer, int offset, int slotSize, char* out, int outSize)
{
	int count = 0;

	while (count < slotSize && count < outSize - 1 && buffer[offset + count] != 0)
	{
		out[count] = (char)buffer[offset + count];
		count++;
	}

	while (count > 0 && (out[count - 1] == ' ' || out[count - 1] == '\t'))
	{
		count--;
	}

	out[count] = 0;
}

// ini degerlerinde bas/son bosluk kirpma (canli SPK.ini 'MainCode = 1.03.34' bicimi)
static void SPK_TrimValue(char* text)
{
	int start = 0;

	while (text[start] == ' ' || text[start] == '\t')
	{
		start++;
	}

	if (start > 0)
	{
		memmove(text, text + start, strlen(text + start) + 1);
	}

	int length = (int)strlen(text);

	while (length > 0 && (text[length - 1] == ' ' || text[length - 1] == '\t'))
	{
		text[--length] = 0;
	}
}

// ---------------------------------------------------------------------------
// CSPKData
// ---------------------------------------------------------------------------

CSPKData::CSPKData()
{
	this->Reset();
}

void CSPKData::Reset()
{
	memset(this, 0, sizeof(CSPKData));
}

bool CSPKData::Load(char* path)
{
	this->Reset();

	strncpy_s(this->m_Path, SPK_DATA_PATH_SIZE, path, _TRUNCATE);

	this->m_DataSPKExists = (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
	this->m_ConnectIPLoaded = this->LoadConnectIP(path);
	this->m_ServerDataLoaded = this->LoadServerData(path);
	this->m_SPKIniLoaded = this->LoadSPKIni(path);

	if (this->m_SPKIniLoaded != false && this->m_ServerDataLoaded != false && this->m_MainCode[0] != 0 && this->m_ClientVersion[0] != 0)
	{
		this->m_VersionMismatch = (strcmp(this->m_MainCode, this->m_ClientVersion) != 0);
	}

	return (this->m_ConnectIPLoaded || this->m_ServerDataLoaded || this->m_SPKIniLoaded);
}

bool CSPKData::ReadFileAll(char* file, BYTE* buffer, int size)
{
	FILE* fp = 0;

	if (fopen_s(&fp, file, "rb") != 0 || fp == 0)
	{
		return false;
	}

	size_t read = fread(buffer, 1, size, fp);

	fclose(fp);

	return (read == (size_t)size);
}

bool CSPKData::LoadConnectIP(char* basePath)	// docs/07 §2 (36 B; ilk 12 B IP, 0x20/0x22 port cifti)
{
	char file[SPK_DATA_PATH_SIZE];

	sprintf_s(file, "%s\\ConnectIP.bmd", basePath);

	BYTE buffer[36];

	if (this->ReadFileAll(file, buffer, sizeof(buffer)) == false)
	{
		return false;
	}

	for (int i = 0; i < (int)sizeof(buffer); i++)
	{
		buffer[i] ^= SPK_XOR_KEY;
	}

	SPK_ReadSlot(buffer, 0, SPK_IP_SLOT_SIZE, this->m_IpAddress, sizeof(this->m_IpAddress));

	// 2e.2: ConnectIP trailer'i (0x20 IpAddressPort / 0x22 AntiPort) — 2d.1 olcumu:
	// canli 44405/55858; dosyada XOR 0x20 uygulanmis durumda (decode sonrasi okunur).
	WORD ipPort = 0;
	WORD antiPort = 0;

	memcpy(&ipPort, buffer + SPK_OFF_IP_ADDRESS_PORT, 2);
	memcpy(&antiPort, buffer + SPK_OFF_ANTI_PORT, 2);

	this->m_IpAddressPort = ipPort;
	this->m_AntiPort = antiPort;

	return (this->m_IpAddress[0] != 0 && SPK_IsPrintText(this->m_IpAddress));
}

bool CSPKData::LoadServerData(char* basePath)	// docs/07 §3a header alanlari
{
	char file[SPK_DATA_PATH_SIZE];

	sprintf_s(file, "%s\\ServerData.bmd", basePath);

	BYTE* buffer = new BYTE[SPK_SERVER_DATA_SIZE];

	if (this->ReadFileAll(file, buffer, SPK_SERVER_DATA_SIZE) == false)
	{
		delete[] buffer;
		return false;
	}

	for (int i = 0; i < SPK_SERVER_DATA_SIZE; i++)
	{
		buffer[i] ^= SPK_XOR_KEY;
	}

	for (int i = 0; i < SPK_DATA_MAX_SERVER; i++)
	{
		SPK_ReadSlot(buffer, SPK_OFF_SERVER_NAME + (i * 32), 32, this->m_ServerName[i], sizeof(this->m_ServerName[i]));
	}

	SPK_ReadSlot(buffer, SPK_OFF_CLIENT_NAME, 32, this->m_ClientName, sizeof(this->m_ClientName));
	SPK_ReadSlot(buffer, SPK_OFF_CUSTOMER_NAME, 32, this->m_CustomerName, sizeof(this->m_CustomerName));
	SPK_ReadSlot(buffer, SPK_OFF_WINDOW_NAME, 32, this->m_WindowName, sizeof(this->m_WindowName));
	SPK_ReadSlot(buffer, SPK_OFF_SCREENSHOT_PATH, 64, this->m_ScreenShotPath, sizeof(this->m_ScreenShotPath));
	SPK_ReadSlot(buffer, SPK_OFF_CLIENT_VERSION, 8, this->m_ClientVersion, sizeof(this->m_ClientVersion));
	SPK_ReadSlot(buffer, SPK_OFF_CLIENT_SERIAL, 16, this->m_ClientSerial, sizeof(this->m_ClientSerial));

	for (int i = 0; i < 7; i++)	// DW/DK/FE/MG/DL/SU/RF sinif hiz limitleri
	{
		int value = 0;
		memcpy(&value, buffer + SPK_OFF_MAX_ATTACK_SPEED + (i * 4), 4);
		this->m_MaxAttackSpeed[i] = (value > 0 && value < 1000000) ? (DWORD)value : 0;
	}

	float camera = 0.0f;
	memcpy(&camera, buffer + SPK_OFF_CAMERA_DEFAULT, 4);
	this->m_CameraDefault = (camera > 0.0f && camera < 360.0f) ? camera : 0.0f;	// docs/07'de ofset '~' isaretli; mantik disi deger sifirlanir

	float fps = 0.0f;
	memcpy(&fps, buffer + SPK_OFF_CAMERA_DEFAULT + SPK_CAMERA_FPS_OFFSET, 4);
	this->m_DefaultFps = (fps > 0.0f && fps < 1000.0f) ? fps : 0.0f;	// canli 240.0 — HAM deger (docs/20: 'x10 degil')

	delete[] buffer;

	// canli istemci de ServerData icerigini dogrular ("The input data is inconsistent!")
	// — bizim esik: kimlik alanlari printable ve bos degil
	return (this->m_ClientVersion[0] != 0 && SPK_IsPrintText(this->m_ClientVersion));
}

bool CSPKData::LoadSPKIni(char* basePath)	// canli SPK.ini ([Version] MainCode = 1.03.34)
{
	char file[SPK_DATA_PATH_SIZE];

	// 2e.2: canli pakette SPK.ini istemci KOKUNDE (Engine.exe: ".\SPK.ini"; 0x64894c);
	// 2d.0 testleri Data\SPK\SPK.ini duzenini kullaniyordu — once taban yol, sonra kök.
	sprintf_s(file, "%s\\SPK.ini", basePath);

	if (GetFileAttributesA(file) == INVALID_FILE_ATTRIBUTES)
	{
		sprintf_s(file, ".\\SPK.ini");

		if (GetFileAttributesA(file) == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}
	}

	strncpy_s(this->m_IniFile, SPK_DATA_PATH_SIZE, file, _TRUNCATE);

	GetPrivateProfileStringA("Version", "MainCode", "", this->m_MainCode, sizeof(this->m_MainCode), file);
	SPK_TrimValue(this->m_MainCode);

	GetPrivateProfileStringA("FontConfig", "FontName", "", this->m_FontName, sizeof(this->m_FontName), file);
	SPK_TrimValue(this->m_FontName);

	this->m_FontHeight = GetPrivateProfileIntA("FontConfig", "FontHeight", 0, file);
	this->m_Resolution = GetPrivateProfileIntA("FontConfig", "Resolution", 0, file);

	GetPrivateProfileStringA("FontConfig", "Lang", "", this->m_Lang, sizeof(this->m_Lang), file);
	SPK_TrimValue(this->m_Lang);

	this->m_BodyX20 = GetPrivateProfileIntA("SPK", "BODY_X20", 0, file);

	// 2e.2 uzantisi: SPK paketinde CBGetMain.bin (MUIG) olmadigi icin GS port araligi
	// buradan verilebilir; anahtarlar yoksa 0 kalir ve cagiran varsayilani uygular.
	this->m_GSPortMin = GetPrivateProfileIntA("SPK", "GSPortMin", 0, file);
	this->m_GSPortMax = GetPrivateProfileIntA("SPK", "GSPortMax", 0, file);

	return true;
}

char* CSPKData::GetPath(char* name, char* out, int size)
{
	sprintf_s(out, size, "%s\\%s", this->m_Path[0] != 0 ? this->m_Path : ".\\Data\\SPK", name);
	return out;
}

char* CSPKData::GetConfigPath(char* name, char* out, int size)
{
	char config[SPK_DATA_PATH_SIZE];

	sprintf_s(config, "%s\\Config", this->m_Path[0] != 0 ? this->m_Path : ".\\Data\\SPK");

	sprintf_s(out, size, "%s\\%s", config, name);
	return out;
}
