// GetEngineConfig.cpp — Faz 2d.1 (D1): [SuperKhung] ini okuyucu (uygulama).
#include "stdafx.h"
#include "GetEngineConfig.h"

static const char* SPK_INI_SECTION = "SuperKhung";

// sayisal anahtar: int oku (yoksa 0)
static int SPK_IniInt(const char* iniPath, const char* key)
{
	return GetPrivateProfileIntA(SPK_INI_SECTION, key, 0, iniPath);
}

// float anahtar: metin oku + atof (canli "240.0" bicimi; GetPrivateProfileInt kesir kaybeder)
static float SPK_IniFloat(const char* iniPath, const char* key)
{
	char text[32] = { 0 };
	GetPrivateProfileStringA(SPK_INI_SECTION, key, "", text, sizeof(text), iniPath);
	return (float)atof(text);
}

// metin anahtar: slot boyu kadar kopyala (slot dolduysa NUL zorlanmaz — canli ClientSerial
// 16/16 karakter doldurur ve dosyada NUL yoktur)
static void SPK_IniString(const char* iniPath, const char* key, char* out, int outSize)
{
	memset(out, 0, outSize);
	GetPrivateProfileStringA(SPK_INI_SECTION, key, "", out, outSize, iniPath);
}

bool SPK_LoadEngineConfig(const char* iniPath, SPK_ENGINE_CONFIG* cfg)
{
	memset(cfg, 0, sizeof(SPK_ENGINE_CONFIG));

	if (GetFileAttributesA(iniPath) == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	cfg->LoadOk = true;

	SPK_IniString(iniPath, "CustomerName", cfg->CustomerName, sizeof(cfg->CustomerName));
	SPK_IniString(iniPath, "ClientSerial", cfg->ClientSerial, sizeof(cfg->ClientSerial));
	SPK_IniString(iniPath, "ClientVersion", cfg->ClientVersion, sizeof(cfg->ClientVersion));
	SPK_IniString(iniPath, "IpAddress", cfg->IpAddress, sizeof(cfg->IpAddress));
	SPK_IniString(iniPath, "WindowName", cfg->WindowName, sizeof(cfg->WindowName));
	SPK_IniString(iniPath, "ClientName", cfg->ClientName, sizeof(cfg->ClientName));
	SPK_IniString(iniPath, "ScreenShotPath", cfg->ScreenShotPath, sizeof(cfg->ScreenShotPath));

	char key[32];

	for (int i = 0; i < SPK_MAX_SERVER; i++)
	{
		sprintf_s(key, "ServerName_%d", i + 1);
		SPK_IniString(iniPath, key, cfg->ServerName[i], sizeof(cfg->ServerName[i]));
	}

	cfg->IpAddressPort = (WORD)SPK_IniInt(iniPath, "IpAddressPort");
	cfg->AntiPort = (WORD)SPK_IniInt(iniPath, "AntiPort");

	cfg->MaxGameInstances = SPK_IniInt(iniPath, "MaxGameInstances");
	cfg->ReconnectTime = SPK_IniInt(iniPath, "ReconnectTime");
	cfg->CameraDefault = SPK_IniFloat(iniPath, "CameraDefault");
	cfg->DefaultFPS = SPK_IniFloat(iniPath, "DefaultFPS");
	cfg->SkillManaPet = SPK_IniInt(iniPath, "SkillManaPet");

	for (int i = 0; i < SPK_MAX_MENU; i++)
	{
		sprintf_s(key, "MENU_BUTTON_%02d", i + 1);
		cfg->Menu[i] = SPK_IniInt(iniPath, key);
	}

	cfg->ButtonCharracter = SPK_IniInt(iniPath, "ButtonCharracter");
	cfg->ButtonShopJwBless = SPK_IniInt(iniPath, "ButtonShopJwBless");
	cfg->ButtonShopJwSoul = SPK_IniInt(iniPath, "ButtonShopJwSoul");
	cfg->ButtonShopChaos = SPK_IniInt(iniPath, "ButtonShopChaos");
	cfg->ButtonShopWcoinC = SPK_IniInt(iniPath, "ButtonShopWcoinC");
	cfg->ButtonShopWcoinP = SPK_IniInt(iniPath, "ButtonShopWcoinP");
	cfg->ButtonShopWcoinG = SPK_IniInt(iniPath, "ButtonShopWcoinG");
	cfg->ButtonShopZens = SPK_IniInt(iniPath, "ButtonShopZens");

	for (int i = 0; i < SPK_MAX_RANKING; i++)
	{
		sprintf_s(key, "Ranking%d", i + 1);
		cfg->Ranking[i] = SPK_IniInt(iniPath, key);
	}

	cfg->MaxLevelDanhHieu = SPK_IniInt(iniPath, "MaxLevelDanhHieu");
	cfg->MaxLevelQuanHam = SPK_IniInt(iniPath, "MaxLevelQuanHam");
	cfg->MaxLevelTuChan = SPK_IniInt(iniPath, "MaxLevelTuChan");
	cfg->MaxLevelHonHoan = SPK_IniInt(iniPath, "MaxLevelHonHoan");
	cfg->EnableCoinTitle = SPK_IniInt(iniPath, "EnableCoinTitle");
	cfg->TextTips3Line = SPK_IniInt(iniPath, "TextTips3Line");
	cfg->RF_GLOVE = SPK_IniInt(iniPath, "RF_GLOVE");
	cfg->MG_HELM = SPK_IniInt(iniPath, "MG_HELM");
	cfg->CreateCharSeason = SPK_IniInt(iniPath, "CreateCharSeason");
	cfg->ButtonClassUP = SPK_IniInt(iniPath, "ButtonClassUP");
	cfg->JewelBankTab = SPK_IniInt(iniPath, "JewelBankTab");

	static const char* speedKeys[SPK_MAX_SPEED] = {
		"DWMaxAttackSpeed", "DKMaxAttackSpeed", "FEMaxAttackSpeed", "MGMaxAttackSpeed",
		"DLMaxAttackSpeed", "SUMaxAttackSpeed", "RFMaxAttackSpeed",
	};

	for (int i = 0; i < SPK_MAX_SPEED; i++)
	{
		cfg->MaxAttackSpeed[i] = (DWORD)SPK_IniInt(iniPath, speedKeys[i]);
	}

	return true;
}
