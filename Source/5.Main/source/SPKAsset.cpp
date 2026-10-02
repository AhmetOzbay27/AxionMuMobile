// SPKAsset.cpp — 2e.4 (H-012): SPK-first varlik yolu cozumleyici.
//
// Canli SPK paketi tablolari Data\SPK\Config altinda tasir; bizim istemci (MUIG soyu)
// ayni tablolari Data\Local (<ve Data\Local\<Lang>\<Ad>_<Lang>) altinda arar. Bu modul
// istenen yolu SPK karsiligiyla esler; karsilik YOKSA isteği aynen dondurur.
//
// Eslenen kaliplar:
//   Data\Local\<Ad>.bmd                    -> Data\SPK\Config\<Ad>.bmd
//   Data\Local\<Lang>\<Ad>_<Lang>.<uzanti> -> Data\SPK\Config\<Ad>.<uzanti>
//   Data\Local\<Lang>\NpcName(<Lang>).txt  -> Data\SPK\Config\NpcName.txt
//   Data\Gate.bmd                          -> Data\SPK\Config\Gate.bmd
//
// Kapsam disi (icerik/sema eslemesi — 2e.4 ikinci yari): itemtooltip_<Lang>.bmd,
// itemleveltooltip_<Lang>.bmd, itemtooltiptext_<Lang>.bmd, Minimap_* (SPK'da karsiligi yok).

#include "stdafx.h"
#include "SPKData.h"

bool SPK_AssetExists(const char* path)
{
	if (path == NULL || path[0] == 0)
		return false;

	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
		return false;

	fclose(fp);
	return true;
}

char* SPK_ResolveAssetPath(const char* requested)
{
	static char s_Resolved[SPK_DATA_PATH_SIZE];

	if (requested == NULL || requested[0] == 0)
	{
		s_Resolved[0] = 0;
		return s_Resolved;
	}

	strncpy_s(s_Resolved, sizeof(s_Resolved), requested, _TRUNCATE);

	char candidate[SPK_DATA_PATH_SIZE] = { 0 };

	if (_strnicmp(requested, "Data\\Local\\", 11) == 0 || _strnicmp(requested, "Data/Local/", 11) == 0)
	{
		const char* p = requested + 11;
		// Ayrac '\' veya '/' olabilir (or. w_BuffScriptLoader: "data/local/Eng/...").
		const char* slash = strchr(p, '\\');
		const char* slash2 = strchr(p, '/');
		if (slash == NULL || (slash2 != NULL && slash2 < slash))
			slash = slash2;

		if (slash == NULL)
		{
			// Data\Local\<Ad> -> Data\SPK\Config\<Ad>
			sprintf_s(candidate, sizeof(candidate), "Data\\SPK\\Config\\%s", p);
		}
		else
		{
			// Data\Local\<Lang>\<Ad>_<Lang>.<uzanti>
			char lang[32] = { 0 };
			size_t langLen = (size_t)(slash - p);
			if (langLen > 0 && langLen < sizeof(lang))
			{
				memcpy(lang, p, langLen);
				lang[langLen] = 0;
			}

			char base[160] = { 0 };
			strncpy_s(base, sizeof(base), slash + 1, _TRUNCATE);

			char ext[16] = { 0 };
			char* dot = strrchr(base, '.');
			if (dot != NULL)
			{
				strncpy_s(ext, sizeof(ext), dot, _TRUNCATE);
				dot[0] = 0;
			}

			// "_<Lang>" son ekini kirp
			char* us = strrchr(base, '_');
			if (us != NULL && lang[0] != 0 && _stricmp(us + 1, lang) == 0)
				us[0] = 0;

			// "NpcName(<Lang>)" -> "NpcName"
			char* par = strchr(base, '(');
			if (par != NULL)
				par[0] = 0;

			sprintf_s(candidate, sizeof(candidate), "Data\\SPK\\Config\\%s%s", base, ext);
		}
	}
	else if (_stricmp(requested, "Data\\Gate.bmd") == 0)
	{
		strcpy_s(candidate, sizeof(candidate), "Data\\SPK\\Config\\Gate.bmd");
	}

	if (candidate[0] != 0 && SPK_AssetExists(candidate))
	{
		strcpy_s(s_Resolved, sizeof(s_Resolved), candidate);
	}

	return s_Resolved;
}
