// log_stub.cpp - yalnizca test harness'i icin: DataStore.cpp gLog kullanir,
// testte dosya loglamaya ihtiyac yok. Uretim kodu DEGISTIRILMEZ.
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include "Log.h"

CLog gLog;

CLog::CLog() { this->m_count = 0; }
CLog::~CLog() {}

void CLog::Output(eLogType type,char* text,...)
{
	char buf[1024];
	va_list ap; va_start(ap,text);
	_vsnprintf_s(buf,sizeof(buf),_TRUNCATE,text,ap);
	va_end(ap);
	printf("      [log] %s\n",buf);
}

void CLog::AddLog(BOOL active,char* directory) {}
