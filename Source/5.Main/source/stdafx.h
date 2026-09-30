// stdafx.h : include file for standard system include files,
#pragma once

//warining
#pragma warning( disable : 4067 ) 
#pragma warning( disable : 4786 ) 
#pragma warning( disable : 4800 ) 
#pragma warning( disable : 4996 ) 
#pragma warning( disable : 4244 )
#pragma warning( disable : 4237 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4503 ) 
#pragma warning( disable : 4267 ) 
#pragma warning( disable : 4091 ) 
#pragma warning( disable : 4819 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4702 )
#pragma warning( disable : 4838 )
//==Tat Thong Bao Build
#pragma warning( disable : 5208 4018 4477 4789 4101 4700)
//#pragma warning( disable : 4482 )
//#pragma warning( disable : 4700 )
//#pragma warning( disable : 4748 )
//#pragma warning( disable : 4786 )
#pragma warning( disable : 28159 )
#pragma warning( disable : 26812 )

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// NOTE: MUIG tabani windows.h'yi NOMINMAX OLMADAN include eder; kod tabanindaki
// ciplak min()/max() cagrilari bu makrolara dayanir, std::min/std::max kullanan
// yeni katman ise (std::max) parantezli stile yazilmistir. Donor ortamina parite
// icin NOMINMAX TANIMLANMAZ.

#if defined(_MSC_VER) && !defined(_WIN64)
#ifndef _USE_32BIT_TIME_T
	#define _USE_32BIT_TIME_T
#endif //_USE_32BIT_TIME_T
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

// Paylasimli (PC + Android) dosyalarda kullanilan SDL tarzi genis tip;
// Android tarafinda SDL header'lari, PC tarafinda burada tanimlanir.
#if !defined(__ANDROID__) && !defined(MU_IOS)
typedef unsigned long long Uint64;
#endif

#if !defined(__ANDROID__) && !defined(MU_IOS)
#pragma warning( push, 3 )

#include <windows.h>
#include <random>
//windows
#include <WinSock2.h>
#include <mmsystem.h>
#include <shellapi.h>
#endif

//c runtime
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <stdarg.h>
#if !defined(__ANDROID__) && !defined(MU_IOS)
#include <malloc.h>
#include <tchar.h>
#include <mbstring.h>
#include <conio.h>
#endif

#include <string>
#include <list>
#include <map>
#include <deque>
#include <algorithm>
#include <vector>
#include <queue>

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <codecvt>


#if !defined(__ANDROID__) && !defined(MU_IOS)
#pragma warning( pop )
#endif

//opengl
#if defined(__ANDROID__) || defined(MU_IOS)
#include "Platform/PlatformDefs.h"
#include "Platform/PlatformGL.h"
#else
#include "Platform/MobileTime.h"
#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/gl.h>
#endif

//patch
//winmain
#include "Winmain.h"
#include "Defined_Global.h"

//client
#include "_define.h"
#include "_enum.h"
#include "_types.h"
#include "_struct.h"	
#include "w_WindowMessageHandler.h"
#include "_GlobalFunctions.h"
#include "_TextureIndex.h"	
#include "InfoHelperFunctions.h"
#include "UIDefaultBase.h"
#include "NewUICommon.h"
#include "./Math/ZzzMathLib.h"
#include "ZzzOpenglUtil.h"

#include "PrintPlayer.h"

#define CURL_STATICLIB

#if !defined(__ANDROID__) && !defined(MU_IOS)
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"Normaliz.lib")
#ifdef _DEBUG
#pragma comment(lib,"..\\ExternalObject\\curl\\libcurl_a_debug.lib")
#else
#pragma comment(lib,"..\\ExternalObject\\curl\\libcurl_a.lib")

#endif
#include "ExternalObject\\FreeImage.h"
#pragma comment(lib, "..\\ExternalObject\\FreeImage.lib")


#include "ExternalObject\\detours.h"
#pragma comment(lib, "..\\ExternalObject\\detours.lib")


#pragma comment(lib, "..\\ExternalObject\\CBGAMEGUARD.lib")
namespace CBGAMEGUARD
{

	class PublicClass {
	public:
		static void LoadEntryProc();
		static void LoadAttach();
		static void SetOffsetMemory(size_t Offset, int Type);
		static void SetTypeMain(int Type);
	};
}
#endif
