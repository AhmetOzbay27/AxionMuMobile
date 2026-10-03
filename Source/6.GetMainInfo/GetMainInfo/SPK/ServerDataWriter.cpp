// ServerDataWriter.cpp — Faz 2d.1 (D3): ServerData.bmd üreticisi (uygulama).
#include "stdafx.h"
#include "ServerDataWriter.h"
#include "SpkUtil.h"

#define SPK_SERVERDATA_SIZE	1089576
#define SPK_SERVERDATA_XOR	0x20

// --- yama yardımcıları (decode edilmiş tampon üzerinde) ---

// slotu tamamen 0'lar, sonra dizeyi en çok slot boyu kadar kopyalar
static void SPK_PatchSlot(BYTE* buf, int offset, int slotSize, const char* text)
{
	memset(buf + offset, 0, slotSize);

	int len = (int)strlen(text);

	if (len > slotSize)
	{
		len = slotSize;
	}

	memcpy(buf + offset, text, len);
}

static void SPK_PatchByte(BYTE* buf, int offset, int value)
{
	buf[offset] = (BYTE)(value & 0xFF);
}

static void SPK_PatchInt32(BYTE* buf, int offset, int value)
{
	memcpy(buf + offset, &value, 4);
}

static void SPK_PatchDword(BYTE* buf, int offset, DWORD value)
{
	memcpy(buf + offset, &value, 4);
}

static void SPK_PatchFloat(BYTE* buf, int offset, float value)
{
	memcpy(buf + offset, &value, 4);
}

bool SPK_WriteServerData(const SPK_ENGINE_CONFIG* cfg, const char* templatePath,
	const char* clientRoot, const char* outPath, unsigned long* outFileCrc)
{
	BYTE* buffer = new BYTE[SPK_SERVERDATA_SIZE];

	if (SPK_ReadAll(templatePath, buffer, SPK_SERVERDATA_SIZE) == false)
	{
		delete[] buffer;
		return false;
	}

	for (int i = 0; i < SPK_SERVERDATA_SIZE; i++)
	{
		buffer[i] ^= SPK_SERVERDATA_XOR;
	}

	// --- header alanları (ini aynası) ---
	for (int i = 0; i < SPK_MAX_SERVER; i++)
	{
		SPK_PatchSlot(buffer, 0x200 + (i * 0x20), 32, cfg->ServerName[i]);
	}

	SPK_PatchSlot(buffer, 0x2A0, 32, cfg->ClientName);
	SPK_PatchSlot(buffer, 0x2C0, 32, cfg->CustomerName);
	SPK_PatchSlot(buffer, 0x2E0, 32, cfg->WindowName);
	SPK_PatchSlot(buffer, 0x3E0, 64, cfg->ScreenShotPath);
	SPK_PatchSlot(buffer, 0x4E0, 8, cfg->ClientVersion);
	SPK_PatchSlot(buffer, 0x4E8, 16, cfg->ClientSerial);

	for (int i = 0; i < SPK_MAX_MENU; i++)
	{
		SPK_PatchByte(buffer, 0x4F9 + i, cfg->Menu[i]);
	}

	SPK_PatchByte(buffer, 0x50D, cfg->ButtonCharracter);
	SPK_PatchByte(buffer, 0x50E, cfg->ButtonShopJwBless);
	SPK_PatchByte(buffer, 0x50F, cfg->ButtonShopJwSoul);
	SPK_PatchByte(buffer, 0x510, cfg->ButtonShopChaos);
	SPK_PatchByte(buffer, 0x511, cfg->ButtonShopWcoinC);
	SPK_PatchByte(buffer, 0x512, cfg->ButtonShopWcoinP);
	SPK_PatchByte(buffer, 0x513, cfg->ButtonShopWcoinG);
	SPK_PatchByte(buffer, 0x515, cfg->ButtonShopZens);

	for (int i = 0; i < SPK_MAX_RANKING; i++)
	{
		SPK_PatchByte(buffer, 0x516 + i, cfg->Ranking[i]);
	}

	SPK_PatchByte(buffer, 0x51D, cfg->SkillManaPet);
	SPK_PatchByte(buffer, 0x51E, cfg->EnableCoinTitle);
	SPK_PatchByte(buffer, 0x51F, cfg->TextTips3Line);
	SPK_PatchByte(buffer, 0x521, cfg->RF_GLOVE);
	SPK_PatchByte(buffer, 0x522, cfg->MG_HELM);
	SPK_PatchByte(buffer, 0x524, cfg->CreateCharSeason);
	SPK_PatchByte(buffer, 0x525, cfg->ButtonClassUP);
	SPK_PatchByte(buffer, 0x526, cfg->JewelBankTab);
	SPK_PatchByte(buffer, 0x52B, cfg->MaxLevelDanhHieu);
	SPK_PatchByte(buffer, 0x52C, cfg->MaxLevelQuanHam);
	SPK_PatchByte(buffer, 0x52D, cfg->MaxLevelTuChan);
	SPK_PatchByte(buffer, 0x52E, cfg->MaxLevelHonHoan);
	SPK_PatchByte(buffer, 0x52F, cfg->MaxGameInstances);

	for (int i = 0; i < SPK_MAX_SPEED; i++)
	{
		SPK_PatchInt32(buffer, 0x530 + (i * 4), (int)cfg->MaxAttackSpeed[i]);
	}

	SPK_PatchByte(buffer, 0x54C, cfg->ReconnectTime);

	// 0x554: istemci exe CRC'si (canlı araç: <clientRoot>\<ClientName>; bulunamazsa 0)
	{
		char clientPath[MAX_PATH];
		sprintf_s(clientPath, "%s\\%s", clientRoot, cfg->ClientName);

		unsigned long clientCrc = 0;

		if (SPK_FileCrc32(clientPath, &clientCrc) == false)
		{
			clientCrc = 0;
		}

		SPK_PatchDword(buffer, 0x554, clientCrc);
	}

	SPK_PatchFloat(buffer, 0x558, cfg->CameraDefault);
	SPK_PatchFloat(buffer, 0x55C, cfg->DefaultFPS);

	// --- encode + yaz ---
	for (int i = 0; i < SPK_SERVERDATA_SIZE; i++)
	{
		buffer[i] ^= SPK_SERVERDATA_XOR;
	}

	bool ok = SPK_WriteAll(outPath, buffer, SPK_SERVERDATA_SIZE);

	SPK_BufferCrc32(buffer, SPK_SERVERDATA_SIZE, outFileCrc);

	delete[] buffer;

	return ok;
}
