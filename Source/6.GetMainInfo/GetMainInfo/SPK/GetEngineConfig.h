// GetEngineConfig.h — Faz 2d.1 (D1): SPK GetEngine.ini okuyucu.
//
// Kanıt: canlı GetMain\GetEngine.ini + canlı GetMainInfo.exe (SPK/GetMain MU 5.2)
// davranış deneyleri (BuildLog/2d1: sentinel ini -> ServerData bayt haritası).
// Bölüm: [SuperKhung] (tek bölüm; şema sabit). Tüm anahtarlar GetPrivateProfile* ile
// okunur; bulunmayan anahtar 0 doğar (canlı araç varsayılanı).
//
// Eşlenen ini anahtarları (kanıt: round A/B deneyleri, docs/20):
//   metin : CustomerName, ClientSerial, ClientVersion, IpAddress, WindowName,
//           ClientName, ScreenShotPath, ServerName_1..4
//   sayısal: IpAddressPort (ConnectIP 0x20; ServerData'da YOK), AntiPort (ConnectIP 0x22),
//           MaxGameInstances, ReconnectTime, CameraDefault, DefaultFPS, SkillManaPet,
//           MENU_BUTTON_01..20, ButtonCharracter, ButtonShopJwBless/JwSoul/JwChaos/
//           WcoinC/WcoinP/WcoinG/Zens, Ranking1..7, MaxLevelDanhHieu/QuanHam/TuChan/
//           HonHoan, EnableCoinTitle, TextTips3Line, RF_GLOVE, MG_HELM,
//           CreateCharSeason, ButtonClassUP, JewelBankTab, MaxAttackSpeed ailesi.
//   Etkisiz (canlı araç yazmıyor): TabInfoStats (deneyde iz yok — dokümanda açık kalem).

#pragma once

#define SPK_MAX_SERVER		4
#define SPK_MAX_MENU		20
#define SPK_MAX_RANKING		7
#define SPK_MAX_SPEED		7

struct SPK_ENGINE_CONFIG
{
	bool LoadOk;							// ini dosyası açılabildi mi

	// --- [SuperKhung] metin alanları
	char CustomerName[32];
	char ClientSerial[17];
	char ClientVersion[9];
	char IpAddress[32];
	char WindowName[33];
	char ClientName[33];
	char ScreenShotPath[64];
	char ServerName[SPK_MAX_SERVER][33];

	// --- portlar (ConnectIP.bmd gövdesi)
	WORD IpAddressPort;						// decode offset 0x20 (u16 LE)
	WORD AntiPort;							// decode offset 0x22 (u16 LE)

	// --- ServerData header bayt/float alanları
	int MaxGameInstances;					// 0x52F
	int ReconnectTime;						// 0x54C
	float CameraDefault;					// 0x558 (float)
	float DefaultFPS;						// 0x55C (float; ham deger)
	int SkillManaPet;						// 0x51D
	int Menu[SPK_MAX_MENU];					// 0x4F9..0x50C (MENU_BUTTON_01..20, byte)
	int ButtonCharracter;					// 0x50D
	int ButtonShopJwBless;					// 0x50E
	int ButtonShopJwSoul;					// 0x50F
	int ButtonShopChaos;					// 0x510 (ini anahtarı "ButtonShopChaos"; Jw öneki YOK)
	int ButtonShopWcoinC;					// 0x511
	int ButtonShopWcoinP;					// 0x512
	int ButtonShopWcoinG;					// 0x513
	int ButtonShopZens;						// 0x515
	int Ranking[SPK_MAX_RANKING];			// 0x516..0x51C (Ranking1..7)
	int EnableCoinTitle;					// 0x51E
	int TextTips3Line;						// 0x51F
	int RF_GLOVE;							// 0x521
	int MG_HELM;							// 0x522
	int CreateCharSeason;					// 0x524
	int ButtonClassUP;						// 0x525
	int JewelBankTab;						// 0x526
	int MaxLevelDanhHieu;					// 0x52B
	int MaxLevelQuanHam;					// 0x52C
	int MaxLevelTuChan;						// 0x52D
	int MaxLevelHonHoan;					// 0x52E
	DWORD MaxAttackSpeed[SPK_MAX_SPEED];	// 0x530.. (DW,DK,FE,MG,DL,SU,RF; int32)
};

// iniPath: ör. ".\\GetEngine.ini". cfg sıfırlanır; LoadOk ini varlığına bağlıdır.
bool SPK_LoadEngineConfig(const char* iniPath, SPK_ENGINE_CONFIG* cfg);
