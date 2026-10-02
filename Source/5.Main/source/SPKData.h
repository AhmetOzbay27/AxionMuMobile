// SPKData.h: interface for the CSPKData class (Faz 2d.0 — SPK istemci format katmani).
//
// Amac: canli SPK istemcisinin (Engine.exe) veri hattini 5.Main'e ogretmek:
//   Data\SPK\ConnectIP.bmd  — 36 B; ilk 12 bayt XOR 0x20 = sunucu IP (docs/07 §2)
//   Data\SPK\ServerData.bmd — 1.089.576 B; tum baytlar XOR 0x20; header alanlari (docs/07 §3a)
//   SPK.ini                 — [Version] MainCode, [FontConfig], [SPK] (canli istemci konfigi)
//   Data\SPK\               — SPK asset/config klasoru (varlik + yol yardimcisi)
//
// Kanit kaynagi: docs/07-SPK-BMD-FORMAT.md + docs/08-GETMAININFO-TASARIM.md §5 (okuyucu
// sozde kodu) + canli ornek dosyalar (Client and Tools\Client\Data\SPK\, SPK.ini).
// AMAC (2e.2 sonrasi): SPK paketinde Data\Local\CBGetMain.bin / CBTextInfo.bin YOKTUR;
// bu katman tek basina istemciyi ayakta tutar. SPK.ini istemci KÖKÜNDE (canli duzen;
// Engine.exe stringleri ".\SPK.ini"), yedek olarak Data\SPK\SPK.ini de denenir.
// Bilinmeyen/ileri alanlar (opsiyon tuple'lari, LEVEL tablolari) bu katmanin kapsami disinda —
// onlar istemci asset katmaninda (2d.0 sonrasi) ele alinacak.
#pragma once

#define SPK_DATA_MAX_SERVER		4		// ServerData 0x200/0x220/0x240/0x260
#define SPK_DATA_PATH_SIZE		260
#define SPK_SERVER_DATA_SIZE	1089576	// canli dosya sabit boyu (docs/07 §3)

// ConnectIP.bmd
#define SPK_IP_SLOT_SIZE		12		// canli sabit IP slotu (docs/07 §2)
#define SPK_OFF_IP_ADDRESS_PORT	0x20	// u16 LE = sunucu portu (canli: 44405)
#define SPK_OFF_ANTI_PORT		0x22	// u16 LE = AntiPort (canli: 55858)

// SPK.ini [SPK] uzantilari (bizim istemci; canli SPK.ini'de yoksa 0 kalir)
#define SPK_DEFAULT_GS_PORT_MIN	55901	// canli GS port araligi (ServerList.xml 55901-55919)
#define SPK_DEFAULT_GS_PORT_MAX	55999

// ServerData.bmd header ofsetleri (decode; docs/07 §3a)
#define SPK_OFF_SERVER_NAME		0x200	// x4, 32 B slot
#define SPK_OFF_CLIENT_NAME		0x2A0	// 32 B — ClientExeName
#define SPK_OFF_CUSTOMER_NAME	0x2C0	// 32 B
#define SPK_OFF_WINDOW_NAME		0x2E0	// 32 B
#define SPK_OFF_SCREENSHOT_PATH	0x3E0	// ~64 B
#define SPK_OFF_CLIENT_VERSION	0x4E0	// 8 B
#define SPK_OFF_CLIENT_SERIAL	0x4E8	// 16 B
#define SPK_OFF_MAX_ATTACK_SPEED	0x530	// 7 x int32 = 65000 (DW/DK/FE/MG/DL/SU/RF; canli dosyada 0x530-0x54B dogrulandi)
#define SPK_OFF_CAMERA_DEFAULT	0x558	// float = 45.0 (canli dosyada dogrulandi; docs/07'de ~0x55A tahminiydi)
#define SPK_CAMERA_FPS_OFFSET	4		// 0x55C: float = 240.0 (FPS x10, canli)

class CSPKData
{
public:
	CSPKData();

	bool Load(char* path);		// path = ".\\Data\\SPK"; en az bir dosya okunursa true

	// durum bayraklari
	bool m_DataSPKExists;		// klasor var mi
	bool m_ConnectIPLoaded;		// ConnectIP.bmd okundu + IP dogrulandi
	bool m_ServerDataLoaded;	// ServerData.bmd okundu + alanlar dogrulandi
	bool m_SPKIniLoaded;		// SPK.ini okundu
	bool m_VersionMismatch;		// SPK.ini MainCode != ServerData ClientVersion

	// ConnectIP.bmd
	char m_IpAddress[32];		// ilk NUL'a kadar (canli: 45.87.120.29)
	WORD m_IpAddressPort;		// 0x20 (canli: 44405) — 0 ise okunmadi
	WORD m_AntiPort;			// 0x22 (canli: 55858) — 0 ise okunmadi

	// ServerData.bmd
	char m_ServerName[SPK_DATA_MAX_SERVER][33];	// sunucu secim listesi
	char m_ClientName[33];		// ClientName (canli: Engine.exe)
	char m_CustomerName[33];	// CustomerName (canli: AxionMu)
	char m_WindowName[33];		// WindowName
	char m_ScreenShotPath[64];	// ScreenShotPath
	char m_ClientVersion[9];	// ClientVersion (canli: 1.03.34)
	char m_ClientSerial[17];	// ClientSerial (canli: !571Axion@Mobile)
	DWORD m_MaxAttackSpeed[7];	// DW/DK/FE/MG/DL/SU/RF limitleri
	float m_CameraDefault;		// 0x558, canli 45.0
	float m_DefaultFps;			// 0x55C, canli 240.0 (ham; docs/20 ofset duzeltmesi)

	// SPK.ini
	char m_MainCode[16];		// [Version] MainCode (canli: 1.03.34)
	char m_IniFile[SPK_DATA_PATH_SIZE];	// okunan SPK.ini yolu (kök ya da Data\SPK)
	int m_GSPortMin;			// [SPK] GSPortMin (opsiyonel uzanti; yoksa 0)
	int m_GSPortMax;			// [SPK] GSPortMax (opsiyonel uzanti; yoksa 0)
	char m_FontName[32];		// [FontConfig] FontName
	int m_FontHeight;			// [FontConfig] FontHeight
	int m_Resolution;			// [FontConfig] Resolution
	char m_Lang[8];				// [FontConfig] Lang
	int m_BodyX20;				// [SPK] BODY_X20

	// yol yardimcilari (m_Path tabanli)
	char* GetPath(char* name, char* out, int size);			// ".\\Data\\SPK\\<name>"
	char* GetConfigPath(char* name, char* out, int size);	// ".\\Data\\SPK\\Config\\<name>"

private:
	bool ReadFileAll(char* file, BYTE* buffer, int size);	// tam boyut okuma
	bool LoadConnectIP(char* basePath);
	bool LoadServerData(char* basePath);
	bool LoadSPKIni(char* basePath);
	void Reset();

	char m_Path[SPK_DATA_PATH_SIZE];	// Load'a verilen taban yol
};

// ---- 2e.4 (H-012): SPK-first varlik yolu cozumleyici (SPKAsset.cpp) ----
// Istek "Data\Local\..." ise ve karsiligi Data\SPK\Config icinde VARSA SPK yolunu
// dondurur; aksi halde isteği aynen dondurur. Statik tampon — sonucu hemen kullanin.
// Kaliplar: Data\Local\<Ad> | Data\Local\<Lang>\<Ad>_<Lang> | NpcName(<Lang>) |
// Data\Gate.bmd. Icerik/sema eslemesi (or. itemtooltip_<Lang>) kapsam disi (2e.4 ikinci yari).
char* SPK_ResolveAssetPath(const char* requested);
bool SPK_AssetExists(const char* path);

extern CSPKData gSPKData;
