#include "stdafx.h"
#include "Protect.h"
#include "SPKData.h"
#include ".\\Utilities\\CCRC32.H"
#include "Util.h"
#include "MainLoad.h"
#include "BuffIcon.h"
#include "TrayMode.h"
#include "APICB.h"
MainLoad gMainLoad;

MainLoad::MainLoad()
{
	
}
MainLoad::~MainLoad()
{

}
DWORD StartAddress(void* lpThreadParameter)
{
	HANDLE v1;
	HANDLE v2;

	while (TRUE)
	{
		::Sleep(60000);

		v1 = GetCurrentProcess();
		SetProcessWorkingSetSize(v1, 0xFFFFFFFF, 0xFFFFFFFF);

		v2 = GetCurrentProcess();
		SetThreadPriority(v2, -2);
	}

	return 0;
}

bool MainLoad::Load()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartAddress, 0, 0, 0);

	//=== SPK istemci format katmani (Faz 2d.0 + 2e.2) — ONCE okunur.
	// Canli SPK paketinde Data\Local\CBGetMain.bin / CBTextInfo.bin YOKTUR (docs/07 §7);
	// kimlik + paket bilgisi SPK.ini ve Data\SPK\*.bmd'den gelir. MUIG dosyalari
	// paketde yoksa istemci SPK-first olarak devam eder (docs/22).
	bool spkLoaded = (gSPKData.Load(".\\Data\\SPK") != false);
	bool spkMode = (spkLoaded != false && gSPKData.m_ServerDataLoaded != false);

	bool mainFileLoaded = (gProtect.ReadMainFile(".\\Data\\Local\\CBGetMain.bin") != 0);

	if (mainFileLoaded == 0 && spkMode == false)
	{
		MessageBox(0, "Config corrupt! ReadMainFile", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return 0;
	}

	bool textFileLoaded = (gProtect.ReadTextFile(".\\Data\\Local\\CBTextInfo.bin") != 0);

	if (textFileLoaded == 0 && spkMode == false)
	{
		MessageBox(0, "Config corrupt! ReadTextFile", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return 0;
	}

	// SPK alanlari MUIG alanlarini ezer; SPK yoksa/bozuksa MUIG hatti aynen korunur
	// (alan bazli fallback — bkz. docs/19); yukarida SPK zaten bir kez okundu.
	if (spkLoaded != false)
	{
		if (gSPKData.m_ConnectIPLoaded != false)
		{
			strcpy_s(gProtect.m_MainInfo.IpAddress, sizeof(gProtect.m_MainInfo.IpAddress), gSPKData.m_IpAddress);

			// 2e.2: port da SPK'dan (ConnectIP 0x20 IpAddressPort / 0x22 AntiPort).
			if (gSPKData.m_IpAddressPort != 0)
			{
				gProtect.m_MainInfo.IpAddressPort = gSPKData.m_IpAddressPort;
			}
		}

		if (gSPKData.m_ServerDataLoaded != false)
		{
			strcpy_s(gProtect.m_MainInfo.ClientVersion, sizeof(gProtect.m_MainInfo.ClientVersion), gSPKData.m_ClientVersion);
			strcpy_s(gProtect.m_MainInfo.ClientSerial, sizeof(gProtect.m_MainInfo.ClientSerial), gSPKData.m_ClientSerial);

			if (gSPKData.m_ClientName[0] != 0)     strcpy_s(gProtect.m_MainInfo.ClientName, sizeof(gProtect.m_MainInfo.ClientName), gSPKData.m_ClientName);
			if (gSPKData.m_CustomerName[0] != 0)   strcpy_s(gProtect.m_MainInfo.CustomerName, sizeof(gProtect.m_MainInfo.CustomerName), gSPKData.m_CustomerName);
			if (gSPKData.m_WindowName[0] != 0)     strcpy_s(gProtect.m_MainInfo.WindowName, sizeof(gProtect.m_MainInfo.WindowName), gSPKData.m_WindowName);
			if (gSPKData.m_ScreenShotPath[0] != 0) strcpy_s(gProtect.m_MainInfo.ScreenShotPath, sizeof(gProtect.m_MainInfo.ScreenShotPath), gSPKData.m_ScreenShotPath);

			if (gSPKData.m_MaxAttackSpeed[0] != 0) gProtect.m_MainInfo.DWMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[0];
			if (gSPKData.m_MaxAttackSpeed[1] != 0) gProtect.m_MainInfo.DKMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[1];
			if (gSPKData.m_MaxAttackSpeed[2] != 0) gProtect.m_MainInfo.FEMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[2];
			if (gSPKData.m_MaxAttackSpeed[3] != 0) gProtect.m_MainInfo.MGMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[3];
			if (gSPKData.m_MaxAttackSpeed[4] != 0) gProtect.m_MainInfo.DLMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[4];
			if (gSPKData.m_MaxAttackSpeed[5] != 0) gProtect.m_MainInfo.SUMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[5];
			if (gSPKData.m_MaxAttackSpeed[6] != 0) gProtect.m_MainInfo.RFMaxAttackSpeed = gSPKData.m_MaxAttackSpeed[6];
		}
	}

	// GS port araligi (CProtect::CheckSocketPort): SPK paketinde CBGetMain.bin yok → MUIG
	// degeri 0 kalir ve TUM portlar reddedilir. SPK.ini [SPK] GSPortMin/GSPortMax varsa
	// onlar; yoksa canli GS araligi (55901-55999, ServerList.xml) uygulanir.
	if (gSPKData.m_GSPortMin > 0 && gSPKData.m_GSPortMax >= gSPKData.m_GSPortMin)
	{
		gProtect.m_MainInfo.GSPortMin = gSPKData.m_GSPortMin;
		gProtect.m_MainInfo.GSPortMax = gSPKData.m_GSPortMax;
	}
	else if (mainFileLoaded == 0)
	{
		gProtect.m_MainInfo.GSPortMin = SPK_DEFAULT_GS_PORT_MIN;
		gProtect.m_MainInfo.GSPortMax = SPK_DEFAULT_GS_PORT_MAX;
	}

	// FpsLimit MUIG'den gelir; SPK paketinde karsiligi ServerData 0x55C (DefaultFPS).
	if (mainFileLoaded == 0 && gSPKData.m_DefaultFps > 0.0f)
	{
		gProtect.m_MainInfo.FpsLimit = (DWORD)gSPKData.m_DefaultFps;
	}

	if (mainFileLoaded == 0)
	{
		g_ErrorReport.Write("[SPK] CBGetMain.bin yok - konfigurasyon Data\\SPK\\ServerData.bmd'den uygulandi (SPK-first).\n");
	}

	if (textFileLoaded == 0)
	{
		g_ErrorReport.Write("[SPK] CBTextInfo.bin yok - metin/tooltip tablolari bos kaldi (SPK-first).\n");
	}

	// EncDec anahtari CustomerName + ClientSerial'den turetilir (ENCRYPT_STATE=1); bu yuzden
	// SPK/MUIG alan birlesimi TAMAMLANDIKTAN SONRA hesaplanmali (2e.2 duzeltmesi).
	gProtect.LoadEncDec();

	gProtect.CheckPluginFile();
	gProtect.CheckLauncher();
	gProtect.CheckInstance();

	//=== Set IP Serrial 
	szServerIpAddress = gProtect.m_MainInfo.IpAddress;
	g_ServerPort = gProtect.m_MainInfo.IpAddressPort;
	memcpy(Serial, gProtect.m_MainInfo.ClientSerial, sizeof(Serial));
	Version[0] = (BYTE)gProtect.m_MainInfo.ClientVersion[0] + 1;
	Version[1] = (BYTE)gProtect.m_MainInfo.ClientVersion[2] + 2;
	Version[2] = (BYTE)gProtect.m_MainInfo.ClientVersion[3] + 3;
	Version[3] = (BYTE)gProtect.m_MainInfo.ClientVersion[5] + 4;
	Version[4] = (BYTE)gProtect.m_MainInfo.ClientVersion[6] + 5;

	if (gProtect.m_MainInfo.LoadAntihack)
	{
		
		
		

	}
	
	gCustomMessage.LoadEng(gProtect.m_MainInfo.EngCustomMessageInfo);
	gCustomMessage.LoadVtm(gProtect.m_MainInfo.VtmCustomMessageInfo);

	gCustomBattleGloves.Load(gProtect.m_MainInfo.CustomGloves);
	gCustomJewel.Load(gProtect.m_MainInfo.CustomJewelInfo);
	gCustomWing.Load(gProtect.m_MainInfo.CustomWingInfo);
	gCustomItem.Load(gProtect.m_MainInfo.CustomItemInfo);
	gCustomItem.LoadRingPen(gProtect.m_MainInfo.CustomRingPenInfo);

	gCloak.Load(gProtect.m_MainInfo.m_CustomCloak);
	gCloak.LoadCEffect(gProtect.m_MainInfo.m_CustomCEffect);

	gCustomWingEffect.Load(gProtect.m_MainInfo.CustomWingEffectInfo);
	gDynamicWingEffect.Load(gProtect.m_MainInfo.DynamicWingEffectInfo);


	gCustomBow.Load(gProtect.m_MainInfo.CustomBowInfo);
	ItemTRSData.Load(gProtect.m_MainInfo.CustomPosition);

	gCustomMonster.Load(gProtect.m_MainInfo.CustomMonsters);
	gCustomMonster.LoadBossClass(gProtect.m_MainInfo.CustomBossClass);
	gNPCName.Load(gProtect.m_MainInfo.CustomNPCName);
	gCustomMonsterGlow.LoadGlow(gProtect.m_MainInfo.m_CustomMonsterGlow);
	gCustomMonsterGlow.LoadBrightness(gProtect.m_MainInfo.m_CustomMonsterbrightness);

	gCustomMap.OpenScritp(gProtect.m_MainInfo.m_MapInfo);
	gCustomPet2.Load(gProtect.m_MainInfo.CustomPetInfo);
	gCustomCEffectPet.Load(gProtect.m_MainInfo.m_PetCEffectBMD);
	gCustomCEffectPet.LoadGlow(gProtect.m_MainInfo.RenderMeshPet);
	gggJCEffectMonster.Load(gProtect.m_MainInfo.m_CustomMonsterEffect);
	//=============

	//==Text FIle
	gIconBuff.LoadEng(gProtect.m_TextInfo.m_TooltipTRSDataEng);
	gIconBuff.LoadVTM(gProtect.m_TextInfo.m_TooltipTRSDataVTM);

	GInfo.loadnInformation(gProtect.m_TextInfo.m_TRSTooltipData);
	GInfo.loadnInformationSet(gProtect.m_TextInfo.m_TRSTooltipSetData);
	GInfo.loadnText(gProtect.m_TextInfo.m_TRSTooltipText);

	gCustomBuyVip.Load(gProtect.m_MainInfo.CustomBuyVipInfo); 

#if (CB_ANTIHACKGGNEW)
	gAPICB.Init();
#endif
	SetTargetFps(gProtect.m_MainInfo.FpsLimit);

	gCustomCommandInfo.Load(gProtect.m_MainInfo.CustomCommandInfo);
	gCustomDmgColor.Load(gProtect.m_MainInfo.CustomDmgColor); //Dmg Color
	return 1;
}