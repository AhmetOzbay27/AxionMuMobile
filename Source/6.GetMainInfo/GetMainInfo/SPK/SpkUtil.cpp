// SpkUtil.cpp — Faz 2d.1 ortak yardımcıları (uygulama).
#include "stdafx.h"
#include "SpkUtil.h"
#include "..\\..\\..\\5.Main\\source\\Utilities\\CCRC32.H"

bool SPK_ReadAll(const char* path, unsigned char* buffer, int size)
{
	FILE* fp = 0;

	if (fopen_s(&fp, path, "rb") != 0 || fp == 0)
	{
		return false;
	}

	size_t read = fread(buffer, 1, size, fp);

	fclose(fp);

	return (read == (size_t)size);
}

bool SPK_WriteAll(const char* path, const unsigned char* buffer, int size)
{
	FILE* fp = 0;

	if (fopen_s(&fp, path, "wb") != 0 || fp == 0)
	{
		return false;
	}

	size_t written = fwrite(buffer, 1, size, fp);

	fclose(fp);

	return (written == (size_t)size);
}

bool SPK_FileCrc32(const char* path, unsigned long* outCrc)
{
	CCRC32 crc;

	return (crc.FileCRC(path, outCrc, 1048576) != false);
}

void SPK_BufferCrc32(const unsigned char* data, int size, unsigned long* outCrc)
{
	CCRC32 crc;

	*outCrc = crc.FullCRC(data, (unsigned long)size);
}
