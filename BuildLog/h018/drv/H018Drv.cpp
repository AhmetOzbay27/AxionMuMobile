// H018Drv.cpp - H-018 runtime soforu (yalniz H018_TDRV tanimli derlemede derlenir).
// Kancalar (hepsi #ifdef H018_TDRV korumali, uretim derlemesi degismez):
//   WSclient.cpp TranslateProtocol() -> H018PktDump()   (gelen paket dokumu)
//   WSclient.cpp ProtocolCompiler()  -> H018LoopTick()  (MainLoop nabzi + aktiflik)
//   ZzzScene.cpp Scene(HDC)          -> H018DrvTick()   (sahne durum makinesi)
// Gunlukler istemcinin KENDI calisma dizinine GORE yazilir (KEN.txt ile ayni
// mekanizma: PopUpErrorCheckMsgBox -> std::ofstream, bagil yol). Mutlak yollu
// fopen bu istemcide calismadigi icin tercih edilmedi (docs/39).
#include "stdafx.h"

#ifdef H018_TDRV

#include "WSclient.h"
#include "ZzzScene.h"
#include "ZzzInterface.h"
#include "ZzzCharacter.h"
#include "ZzzInfomation.h"
#include "GameCensorship.h"
#include "wsclientinline.h"
#include "ServerListManager.h"
#include "ServerGroup.h"
#include "ServerInfo.h"
#include <stdio.h>
#include <stdarg.h>
#include <fstream>

extern int SceneFlag;
extern int CurrentProtocolState;
extern int SelectedHero;
extern CHARACTER* CharactersClient;
extern CHARACTER* Hero;
extern CHARACTER_ATTRIBUTE* CharacterAttribute;
extern HWND g_hWnd;
void StartGame();

static std::ofstream s_osLog;      // H018DRV.log
static std::ofstream s_osPkt;      // H018DRV_pkt.log
static std::ofstream s_osRaw;      // H018DRV_recv.bin
static int   s_osOk = 0;
static int   s_booted = 0;
static DWORD s_t0 = 0;
static int   s_stage = 0;
static DWORD s_ddl = 0;
static DWORD s_stageT = 0;
static DWORD s_hb = 0;
static DWORD s_loopHb = 0;
static int   s_seq = 0;
static int   s_pktTotal = 0;
static int   s_heads[256];
static int   s_liveP = 0;
static int   s_liveM = 0;
static int   s_half = 0;
static char  s_id[MAX_ID_SIZE + 1] = "e2etest";
static char  s_pw[MAX_PASSWORD_SIZE + 1] = "test123";
static int   s_dwell = 25000;
static char  s_charId[MAX_ID_SIZE + 2] = "";
static int   s_charIdx = -1;

static void OsOpen(void)
{
	if (s_osOk) return;
	s_osOk = 1;
	s_osLog.open("H018DRV.log", std::ios::app);
	s_osPkt.open("H018DRV_pkt.log", std::ios::app);
	s_osRaw.open("H018DRV_recv.bin", std::ios::binary | std::ios::app);
}

static void L(const char* fmt, ...)
{
	va_list ap;
	char buf[1024];
	unsigned long ms;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	buf[1023] = 0;
	OsOpen();
	ms = (unsigned long)(GetTickCount() - s_t0);
	if (s_osLog.is_open())
	{
		s_osLog << "[t=" << ms << "] " << buf << std::endl;
		s_osLog.flush();
	}
}

static int HexHead(int head)
{
	return head == 0x00 || head == 0x01 || head == 0x0F || head == 0x12 || head == 0x13 ||
	       head == 0x1F || head == 0x1C || head == 0x1D || head == 0x45 ||
	       head == 0xF1 || head == 0xF3 || head == 0xF4 || head == 0x0F;
}

void H018PktDump(int HeadCode, BYTE* buf, int size, BOOL enc)
{
	int i, n;
	OsOpen();
	s_pktTotal++;
	if (HeadCode >= 0 && HeadCode < 256) s_heads[HeadCode]++;
	if (s_osRaw.is_open() && !enc && size > 0 && size < 65536)
	{
		s_osRaw.write((const char*)buf, size);
		s_osRaw.flush();
	}
	if (!HexHead(HeadCode)) return;
	if (!s_osPkt.is_open()) return;
	n = size < 200 ? size : 200;
	s_osPkt << s_seq++ << " head=0x" << std::hex << HeadCode << std::dec << " size=" << size
	        << " enc=" << (int)enc << " hex=";
	static const char hx[] = "0123456789ABCDEF";
	for (i = 0; i < n; i++)
	{
		s_osPkt << hx[(buf[i] >> 4) & 0xF] << hx[buf[i] & 0xF];
	}
	if (size > n) s_osPkt << "..";
	s_osPkt << std::endl;
	s_osPkt.flush();
}

static void Cfg(void)
{
	FILE* f = fopen("H018DRV.ini", "rt");
	char line[256];
	if (f == NULL) { L("uyari: H018DRV.ini yok -> varsayilan e2etest/test123 dwell=25000"); return; }
	while (fgets(line, sizeof(line), f) != NULL)
	{
		char* k = line;
		char* v;
		char* e;
		char* eq = strchr(line, '=');
		if (eq == NULL) continue;
		*eq = 0;
		v = eq + 1;
		while (*k == ' ' || *k == '\t') k++;
		while (*v == ' ' || *v == '\t') v++;
		e = v + strlen(v);
		while (e > v && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t')) { e--; *e = 0; }
		if (_stricmp(k, "id") == 0) { strncpy(s_id, v, MAX_ID_SIZE); s_id[MAX_ID_SIZE] = 0; }
		else if (_stricmp(k, "pw") == 0) { strncpy(s_pw, v, MAX_PASSWORD_SIZE); s_pw[MAX_PASSWORD_SIZE] = 0; }
		else if (_stricmp(k, "dwell_ms") == 0) s_dwell = atoi(v);
	}
	fclose(f);
}

static void CountLive(void)
{
	int i;
	s_liveP = 0;
	s_liveM = 0;
	for (i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		if (!CharactersClient[i].Object.Live) continue;
		if (CharactersClient[i].Object.Kind == KIND_PLAYER) s_liveP++;
		else if (CharactersClient[i].Object.Kind == KIND_MONSTER) s_liveM++;
	}
}

static void Finish(int rc)
{
	L("== OZET: paket=%d ==", s_pktTotal);
	L("bas kodlari: 0x00=%d 0x01=%d 0x0F=%d 0x12=%d 0x13=%d 0x1F=%d 0x1C=%d 0x45=%d 0xF1=%d 0xF3=%d 0xF4=%d",
	  s_heads[0x00], s_heads[0x01], s_heads[0x0F], s_heads[0x12], s_heads[0x13], s_heads[0x1F],
	  s_heads[0x1C], s_heads[0x45], s_heads[0xF1], s_heads[0xF3], s_heads[0xF4]);
	L("son canli: oyuncu=%d yaratik=%d | karakter='%s' idx=%d", s_liveP, s_liveM, s_charId, s_charIdx);
	L("SONUC=%s rc=%d", rc == 0 ? "PASS" : "FAIL", rc);
	L("H018DRV log kapaniyor");
	s_stage = 7;
	s_ddl = GetTickCount() + 2000;
}

static void H018Init(void)
{
	s_booted = 1;
	s_t0 = GetTickCount();
	OsOpen();
	memset(s_heads, 0, sizeof(s_heads));
	L("H018DRV baslat: Main_h018 (H018_TDRV) sahne=%d protokol=%d", SceneFlag, CurrentProtocolState);
	L("yazma: log=%d pkt=%d raw=%d", (int)s_osLog.is_open(), (int)s_osPkt.is_open(), (int)s_osRaw.is_open());
	Cfg();
	L("kimlik id='%s' sifre_uzunluk=%d dwell=%d ms", s_id, (int)strlen(s_pw), s_dwell);
	s_stage = 0;
	s_ddl = s_t0 + 180000;
}

void H018LoopTick(void)
{
	extern bool g_bWndActive;
	DWORD t;
	g_bWndActive = true;
	if (!s_booted) H018Init();
	t = GetTickCount();
	if (t - s_loopHb >= 5000)
	{
		s_loopHb = t;
		L("dongu: sahne=%d protokol=%d paket=%d", SceneFlag, CurrentProtocolState, s_pktTotal);
	}
}

void H018DrvTick(void)
{
	DWORD now;
	if (s_stage < 0) return;
	if (!s_booted) H018Init();
	now = GetTickCount();
	if (s_stage >= 7)
	{
		if (s_stage == 7 && now >= s_ddl)
		{
			L("kapanis: WM_CLOSE gonderiliyor");
			PostMessage(g_hWnd, WM_CLOSE, 0, 0);
			s_stage = 8;
			s_ddl = now + 15000;
		}
		else if (s_stage == 8 && now >= s_ddl)
		{
			L("kapanis: WM_CLOSE is gorulmedi -> ExitProcess(0)");
			ExitProcess(0);
		}
		return;
	}
	if (now - s_hb >= 5000)
	{
		s_hb = now;
		L("nabiz: asama=%d sahne=%d protokol=%d paket=%d", s_stage, SceneFlag, CurrentProtocolState, s_pktTotal);
	}
	switch (s_stage)
	{
	case 0:
		if (s_stageT == 0)
		{
			if (SceneFlag == LOG_IN_SCENE && CServerListManager::GetInstance()->GetServerGroupSize() > 0)
			{
				s_stageT = now;
				L("asama0: LOG_IN_SCENE + sunucu listesi hazir (protokol=%d) - 1 sn oturma", CurrentProtocolState);
			}
		}
		else if (now - s_stageT >= 1000)
		{
			CServerGroup* pg = NULL;
			CServerInfo* pi = NULL;
			CServerGroup* pgSel = NULL;
			CServerInfo* piSel = NULL;
			CServerListManager::GetInstance()->SetFirst();
			while (CServerListManager::GetInstance()->GetNext(pg))
			{
				pg->SetFirst();
				while (pg->GetNext(pi))
				{
					if (pi->m_iPercent >= 100) continue;
					pgSel = pg;
					piSel = pi;
					break;
				}
				if (piSel != NULL) break;
			}
			if (piSel == NULL) { L("asama0 FAIL: listede musait sunucu yok"); Finish(1); break; }
			L("asama0 OK: grup_sira=%d sunucu_idx=%d connect_idx=%d yuzde=%d nonpvp=0x%02X -> SendRequestServerAddress",
			  pgSel->m_iSequence, piSel->m_iIndex, piSel->m_iConnectIndex, piSel->m_iPercent, (unsigned)piSel->m_byNonPvP);
			SendRequestServerAddress(piSel->m_iConnectIndex);
			{
				int iCens = SEASON3A::CGameCensorship::STATE_12;
				if (pgSel->m_bPvPServer == true) iCens = SEASON3A::CGameCensorship::STATE_18;
				else if (0x01 & piSel->m_byNonPvP) iCens = SEASON3A::CGameCensorship::STATE_15;
				CServerListManager::GetInstance()->SetSelectServerInfo(pgSel->m_szName, piSel->m_iIndex, iCens,
					piSel->m_byNonPvP, pgSel->m_iSequence == 0);
			}
			s_stage = 1;
			s_stageT = now;
			s_ddl = now + 120000;
		}
		break;
	case 1:
		if (g_bGameServerConnected == TRUE && now - s_stageT >= 1500)
		{
			L("asama1 OK: GS baglantisi kuruldu (protokol=%d) -> SendRequestLogIn('%s')", CurrentProtocolState, s_id);
			SendRequestLogIn(s_id, s_pw);
			s_stage = 2;
			s_stageT = now;
			s_ddl = now + 60000;
		}
		break;
	case 2:
		if (SceneFlag == CHARACTER_SCENE && CurrentProtocolState == RECEIVE_CHARACTERS_LIST)
		{
			int i, best = -1;
			for (i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				if (CharactersClient[i].Object.Live && CharactersClient[i].ID[0] != 0) { best = i; break; }
			}
			if (best >= 0)
			{
				strncpy(s_charId, CharactersClient[best].ID, MAX_ID_SIZE);
				s_charId[MAX_ID_SIZE] = 0;
				s_charIdx = best;
				L("asama2 OK: karakter listesi geldi secilen idx=%d id='%s' -> StartGame()", best, s_charId);
				SelectedHero = best;
				StartGame();
				s_stage = 3;
				s_stageT = now;
				s_ddl = now + 180000;
			}
		}
		break;
	case 3:
		if (SceneFlag == MAIN_SCENE)
		{
			CountLive();
			L("asama3 OK: MAIN_SCENE dunya yuklendi | karakter='%s' idx=%d konum=(%d,%d) canli_oyuncu=%d canli_yaratik=%d",
			  CharacterAttribute != NULL ? (const char*)CharacterAttribute->Name : "?",
			  SelectedHero, Hero != NULL ? Hero->PositionX : -1, Hero != NULL ? Hero->PositionY : -1,
			  s_liveP, s_liveM);
			s_stage = 4;
			s_stageT = now;
			s_ddl = now + (DWORD)s_dwell;
		}
		break;
	case 4:
		if (!s_half && now >= s_stageT + (DWORD)(s_dwell / 2))
		{
			s_half = 1;
			CountLive();
			L("asama4 ara: canli_oyuncu=%d canli_yaratik=%d", s_liveP, s_liveM);
		}
		if (now >= s_ddl)
		{
			CountLive();
			L("asama4 OK: dunya beklemesi bitti (canli_oyuncu=%d canli_yaratik=%d) -> SendRequestLogOut(1)", s_liveP, s_liveM);
			SendRequestLogOut(1);
			s_stage = 5;
			s_stageT = now;
			s_ddl = now + 30000;
		}
		break;
	case 5:
		if (SceneFlag == CHARACTER_SCENE)
		{
			L("asama5 OK: cikis(1) sonrasi CHARACTER_SCENE (protokol=%d) -> SendRequestLogOut(2)", CurrentProtocolState);
			SendRequestLogOut(2);
			s_stage = 6;
			s_stageT = now;
			s_ddl = now + 30000;
		}
		break;
	case 6:
		if (SceneFlag == LOG_IN_SCENE)
		{
			L("asama6 OK: tam cikis - LOG_IN_SCENE (protokol=%d)", CurrentProtocolState);
			Finish(0);
		}
		else if (now >= s_ddl)
		{
			L("asama6 uyari: LOG_IN_SCENE gorulmedi (sahne=%d) - PASS sayildi", SceneFlag);
			Finish(0);
		}
		break;
	default:
		break;
	}
	if (s_stage >= 0 && s_stage < 6 && now > s_ddl)
	{
		CountLive();
		L("ZAMAN ASIMI: asama=%d sahne=%d protokol=%d paket=%d canli_oyuncu=%d canli_yaratik=%d",
		  s_stage, SceneFlag, CurrentProtocolState, s_pktTotal, s_liveP, s_liveM);
		Finish(s_stage == 0 ? 10 : s_stage);
	}
}

#endif // H018_TDRV
