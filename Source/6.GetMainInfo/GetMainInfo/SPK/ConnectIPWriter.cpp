// ConnectIPWriter.cpp — Faz 2d.1 (D2): ConnectIP.bmd üreticisi (uygulama).
#include "stdafx.h"
#include "ConnectIPWriter.h"

#define SPK_CONNECTIP_SIZE	36
#define SPK_CONNECTIP_XOR	0x20
#define SPK_IP_SLOT			12

bool SPK_WriteConnectIP(const SPK_ENGINE_CONFIG* cfg, const char* outPath)
{
	BYTE buffer[SPK_CONNECTIP_SIZE];

	memset(buffer, 0, sizeof(buffer));

	// IP dizesi: en çok 12 bayt; slot dolduysa NUL zorlanmaz (canlı "45.87.120.29" 12/12)
	int ipLen = (int)strlen(cfg->IpAddress);

	if (ipLen > SPK_IP_SLOT)
	{
		ipLen = SPK_IP_SLOT;
	}

	memcpy(buffer, cfg->IpAddress, ipLen);

	// port çifti (decode alanları)
	memcpy(buffer + 0x20, &cfg->IpAddressPort, 2);
	memcpy(buffer + 0x22, &cfg->AntiPort, 2);

	for (int i = 0; i < SPK_CONNECTIP_SIZE; i++)
	{
		buffer[i] ^= SPK_CONNECTIP_XOR;
	}

	FILE* fp = 0;

	if (fopen_s(&fp, outPath, "wb") != 0 || fp == 0)
	{
		return false;
	}

	size_t written = fwrite(buffer, 1, sizeof(buffer), fp);

	fclose(fp);

	return (written == sizeof(buffer));
}
