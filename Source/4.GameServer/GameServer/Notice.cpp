// Notice.cpp: implementation of the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Notice.h"
#include "Log.h"	// Faz 2b.2-N: Reload logları
#include "MemScript.h"
#include "Util.h"




CNotice gNotice;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotice::CNotice() // OK
{
	this->m_count = 0;

	this->m_NoticeValue = 0;

	this->m_NoticeTime = GetTickCount();
}

CNotice::~CNotice() // OK
{
}

void CNotice::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_count = 0;

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			NOTICE_INFO info;

			memset(&info,0,sizeof(info));

			strcpy_s(info.Message,lpMemScript->GetString());

			info.Type = lpMemScript->GetAsNumber();

			info.Count = lpMemScript->GetAsNumber();

			info.Opacity = lpMemScript->GetAsNumber();

			info.Delay = lpMemScript->GetAsNumber();

			info.Color = RGB(lpMemScript->GetAsNumber(),lpMemScript->GetAsNumber(),lpMemScript->GetAsNumber());

			info.Speed = lpMemScript->GetAsNumber();

			info.RepeatTime = lpMemScript->GetAsNumber()*1000;

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

	// Faz 2b.2-N: yolu sakla (reload icin)
	memset(this->m_Path,0,sizeof(this->m_Path));
	strcpy_s(this->m_Path,path);
}

// Faz 2b.2-N: config'i yeniden yükler (canlı SPK log deseni: 'Notice
// configuration reloaded'). /reload notice komutu çağırır; hata durumunda
// mevcut veriler korunur. Load m_count'u sıfırladığı için boş-dosya koruması
// GetCount() ile yapılır (E-05 deseni).
void CNotice::Reload() // Faz 2b.2-N
{
	if(this->m_Path[0] == 0)
	{
		LogAdd(LOG_RED,"[Notice] Reload skipped - config path not set yet");
		return;
	}

	NOTICE_INFO oldInfo[MAX_NOTICE];
	int oldCount = this->m_count;
	memcpy(oldInfo,this->m_NoticeInfo,sizeof(oldInfo));

	this->Load(this->m_Path);

	if(this->GetCount() == 0)
	{
		memcpy(this->m_NoticeInfo,oldInfo,sizeof(oldInfo));
		this->m_count = oldCount;	// Faz 2b.2-R: 2b.2-N'de eksikti — eski veri geri yüklense de m_count=0 kalıyordu
		LogAdd(LOG_RED,"[Notice] Reload failed - old data restored (%s)",this->m_Path);
		return;
	}

	LogAdd(LOG_BLUE,"[CNotice] Notice configuration reloaded.");
}

void CNotice::SetInfo(NOTICE_INFO info) // OK
{
	if(this->m_count < 0 || this->m_count >= MAX_NOTICE)
	{
		return;
	}

	this->m_NoticeInfo[this->m_count++] = info;
}

void CNotice::MainProc() // OK
{
	if(this->m_count == 0)
	{
		return;
	}

	NOTICE_INFO* lpInfo = &this->m_NoticeInfo[this->m_NoticeValue];

	if((GetTickCount()-this->m_NoticeTime) >= ((DWORD)lpInfo->RepeatTime))
	{
		this->m_NoticeValue = (((this->m_NoticeValue+1)>=this->m_count)?0:(this->m_NoticeValue+1));
		this->m_NoticeTime = GetTickCount();
		this->GCNoticeSendToAll(lpInfo->Type,lpInfo->Count,lpInfo->Opacity,lpInfo->Delay,lpInfo->Color,lpInfo->Speed,"%s",lpInfo->Message);
	}
}

void CNotice::GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);	
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CNotice::GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	gServerDisplayer.LogAddTextGlobal(LOG_BLUE,buff,strlen(buff));
}

// SCF BOT 
void CNotice::NewMessageDevTeam(int aIndex,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_DEV_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CNotice::NewNoticeSend(int aIndex,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND_NEW pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}