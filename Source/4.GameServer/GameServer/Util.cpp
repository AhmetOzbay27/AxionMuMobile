#include "stdafx.h"
#include "Util.h"
#include "GameMain.h"
#include "HackCheck.h"
#include "ItemManager.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "ThemidaSDK.h"
#include "Viewport.h"
#include "DSProtocol.h"
#include "Map.h"

std::mt19937 seed;
std::uniform_int_distribution<int> dist;
short RoadPathTable[MAX_ROAD_PATH_TABLE] = {-1,-1,0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0};

int SafeGetItem(int index) // OK
{
	return CHECK_ITEM(index);
}

flt GetRoundValue(float value) // OK
{
	float integral;

	if(modf(value,&integral) > 0.5f)
	{
		return ceil(value);
	}

	return floor(value);
}

BYTE GetNewOptionCount(BYTE NewOption) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_EXC_OPTION;n++)
	{
		if((NewOption & (1 << n)) != 0)
		{
			count++;
		}
	}

	return count;
}

BYTE GetSocketOptionCount(BYTE SocketOption[5]) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		if(SocketOption[n] != 0xFF)
		{
			count++;
		}
	}

	return count;
}

BYTE GetPathPacketDirPos(int px,int py) // OK
{
	if(px <= -1 && py <= -1)
	{
		return 0;
	}
	else if(px <= -1 && py == 0)
	{
		return 7;
	}
	else if(px <= -1 && py >= 1)
	{
		return 6;
	}
	else if(px == 0 && py <= -1)
	{
		return 1;
	}
	else if(px == 0 && py >= 1)
	{
		return 5;
	}
	else if(px >= 1 && py <= -1)
	{
		return 2;
	}
	else if(px >= 1 && py == 0)
	{
		return 3;
	}
	else if(px >= 1 && py >= 1)
	{
		return 4;
	}

	return 0;
}

void PacketArgumentDecrypt(char* out_buff,char* in_buff,int size) // OK
{
	BYTE XorTable[3] = {0xFC,0xCF,0xAB};

	for(int n=0;n < size;n++)
	{
		out_buff[n] = in_buff[n]^XorTable[n%3];
	}
}

void ErrorMessageBox(char* message,...) // OK
{
	VM_START

	char buff[256];

	memset(buff,0,sizeof(buff));

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	MessageBox(0,buff,"Error",MB_OK | MB_ICONERROR);

	VM_END

	ExitProcess(0);
}

void LogAdd(eLogColor color,char* text,...) // OK
{
	tm today;
	time_t ltime;
	time(&ltime);

	if(localtime_s(&today,&ltime) != 0)
	{
		return;
	}

	char time[32];

	if(asctime_s(time,sizeof(time),&today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;
	va_start(arg,text);
	vsprintf_s(temp,text,arg);
	va_end(arg);

	char log[1024];

	wsprintf(log,"%.8s %s",&time[11],temp);

	gServerDisplayer.LogAddText(color,log,strlen(log));
}

void LogAddConnect(eLogColor color,char* text,...) // OK
{
	tm today;
	time_t ltime;
	time(&ltime);

	if(localtime_s(&today,&ltime) != 0)
	{
		return;
	}

	char time[32];

	if(asctime_s(time,sizeof(time),&today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;
	va_start(arg,text);
	vsprintf_s(temp,text,arg);
	va_end(arg);

	char log[1024];

	wsprintf(log,"%.8s %s",&time[11],temp);

	gServerDisplayer.LogAddTextConnect(color,log,strlen(log));
}

bool DataSend(int aIndex,BYTE* lpMsg,DWORD size) // OK
{
#if(OFFLINE_MODE == TRUE)
	if (gObj[aIndex].m_OfflineMode == 1)
	{
		return 0;
	}
#endif

	return gSocketManager.DataSend(aIndex,lpMsg,size);
}

void DataSendAll(BYTE* lpMsg,int size) // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0)
		{
			DataSend(n,lpMsg,size);
		}
	}
}

bool DataSendSocket(SOCKET socket,BYTE* lpMsg,DWORD size) // OK
{
	if(socket == INVALID_SOCKET)
	{
		return 0;
	}

	#if(ENCRYPT_STATE==1)

	EncryptData(lpMsg,size);

	#endif

	int count=0,result=0;

	while(size > 0)
	{
		if((result=send(socket,(char*)&lpMsg[count],size,0)) == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return 0;
			}
		}
		else
		{
			count += result;
			size -= result;
		}
	}

	return 1;
}

void MsgSendV2(LPOBJ lpObj,BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_USER && OBJECT_BOTS)
		{
			DataSend(lpObj->VpPlayer2[n].index,lpMsg,size);
		}
	}
}

void CloseClient(int aIndex) // OK
{
	if(gObj[aIndex].CacheSendOnlogin) GDCharacterInfoSaveSend(aIndex); //fix 
	gSocketManager.Disconnect(aIndex);
}

void PostMessage1(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {0};

	wsprintf(buff,message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_WHISPER_SEND pMsg;

	pMsg.header.set(0x02,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void PostMessage2(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'~'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void PostMessage3(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'@'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void PostMessage4(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'$'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void PostMessagePK(char* name,char* message,char* text, char* MapName) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'~'};

	wsprintf(&buff[1],message,name,text, MapName);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,"[PK]",sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void PostMessageUserON(char* name,char* message) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'@'};

	wsprintf(&buff[1],message,name);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,"[UserON]",sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void PostMessageItemNotice(char* name, char* message) // OK
{
#if(GAMESERVER_UPDATE>=701)

	char buff[256] = { 0 };

	wsprintf(buff, message, name);

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n], buff);
		}
	}

#else

	char buff[256] = { '@' };

	wsprintf(&buff[1], message, name);

	int size = strlen(buff);

	size = ((size > MAX_CHAT_MESSAGE_SIZE) ? MAX_CHAT_MESSAGE_SIZE : size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	memcpy(pMsg.name, "[System]", sizeof(pMsg.name));

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg, pMsg.header.size);

#endif
}

void PostMessageNew(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_WHISPER_SEND pMsg;

	pMsg.header.set(0x02,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,"[POST]",sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSendAll((BYTE*)&pMsg,pMsg.header.size);
	#endif
}


void SetLargeRand() // OK
{
	std::random_device rd;
	seed = std::mt19937(rd());
	dist = std::uniform_int_distribution<int>(0,2147483647);
}

long GetLargeRand() // OK
{
	return dist(seed);
}


#if (FIXBUGCOIN)
int CheckCoinBB(int value)
{

	if (value < 0 || value >= INT_MAX)
	{
		return 0;
	}
	else {
		return value;
	}
}
bool CheckCoinBBT(int value)
{

	if (value < 0 || value >= INT_MAX)
	{
		return 1;
	}
	else {
		return 0;
	}
}
#endif

bool CheckMaxSizePacket(int Size, int MaxSize)
{
	if (Size >= MaxSize)
	{
		return 1;
	}
	return 0;
}

bool CheckChuoiKyTuDacBiet(const std::string& str)
{
	for (int i = 0; i < str.size(); i++)
	{
		if (!isdigit(str[i]) && !(str[i] >= 'a' && str[i] <= 'z') && !(str[i] >= 'A' && str[i] <= 'Z'))
		{
			return false;
		}
	}
	return true;
}

int getNumberOfExcOptions(int checksum)
{
	int optionscount = 0;
	int ExcOrgArr[6];

	ExcOrgArr[0] = 1;
	ExcOrgArr[1] = 2;
	ExcOrgArr[2] = 4;
	ExcOrgArr[3] = 8;
	ExcOrgArr[4] = 16;
	ExcOrgArr[5] = 32;

	if (checksum > 0)
	{
		for (int i = 0; i < 6; i++)
		{
			int and_val = checksum & ExcOrgArr[i];
			if (and_val != 0)
				optionscount += 1;
		}
	}

	return optionscount;
}

char* NumberFormat(int Number)
{


	if (Number < 0) { return "0"; }

	char OutPut[15];

	if (Number < 1000) {
		sprintf(OutPut, "%d", Number);
		return strdup(OutPut);
	}
	else if (Number < 1000000) {
		int part1 = Number % 1000;
		int part2 = (Number - part1) / 1000;
		sprintf(OutPut, "%d,%03d", part2, part1);
		return strdup(OutPut);
	}
	else if (Number < 1000000000) {
		int part1 = Number % 1000;
		Number = (Number - part1) / 1000;
		int part2 = Number % 1000;
		Number = (Number - part2) / 1000;
		int part3 = Number % 1000;

		sprintf(OutPut, "%d,%03d,%03d", part3, part2, part1);
		return strdup(OutPut);
	}
	else {
		int part1 = Number % 1000;
		Number = (Number - part1) / 1000;
		int part2 = Number % 1000;
		Number = (Number - part2) / 1000;
		int part3 = Number % 1000;
		int part4 = (Number - part3) / 1000;

		sprintf(OutPut, "%d,%03d,%03d,%03d", part4, part3, part2, part1);
		return strdup(OutPut);
	}
	return "0";
}

bool GetRandomItemDropLocation(int map, int* ox, int* oy, int tx, int ty, int count) // OK
{
	int x = (*ox);
	int y = (*oy);

	tx = ((tx < 1) ? 1 : tx);
	ty = ((ty < 1) ? 1 : ty);

	for (int n = 0; n < count; n++)
	{
		(*ox) = ((GetLargeRand() % (tx + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + x;
		(*oy) = ((GetLargeRand() % (ty + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + y;

		if (gMap[map].CheckAttr((*ox), (*oy), 4) == 0 && gMap[map].CheckAttr((*ox), (*oy), 8) == 0)
		{
			return 1;
		}
	}

	return 0;
}

int random(int minN, int maxN)
{
	srand((int)time(0));
	return minN + rand() % (maxN + 1 - minN);
}

// SPK (Faz 2b.2-C): donor Util.cpp:708/718/748/762 — ExportBMD/paket-yazma
// fonksiyonlari (Gate ExportBMD kullaniyor; 2b.2-C donor-tekil Export* metot
// zinciri icin alindi). BuxConvert bizde yoktu, donorden geldi.
void BuxConvert(BYTE* pbyBuffer, int Size)
{
	BYTE bBuxCode[3] = { 0xFC, 0xCF, 0xAB };

	for (int i = 0; i < Size; i++)
	{
		pbyBuffer[i] ^= bBuxCode[i % 3];
	}
}

DWORD GenerateCheckSum2(BYTE* pbyBuffer, int dwSize, WORD Key)
{
	int dwKey = Key;
	int dwResult = Key << 9;

	for (int dwChecked = 0; dwChecked <= dwSize - 4; dwChecked += 4)
	{
		DWORD dwTemp;

		memcpy(&dwTemp, pbyBuffer + dwChecked, 4);

		DWORD v4 = (Key + (dwChecked >> 2)) % 2;
		switch (v4)
		{
		case 0:
			dwResult ^= dwTemp;
			break;
		case 1:
			dwResult += dwTemp;
			break;
		}

		if (!(dwChecked % 0x10))
		{
			dwResult ^= (unsigned int)(dwResult + dwKey) >> ((dwChecked >> 2) % 8 + 1);
		}
	}
	return dwResult;
}

void PackFileEncrypt(const char* filename, BYTE* pbyBuffer, int MaxLine, int Size)
{
	FILE* fp = fopen(filename, "wb");

	if (fp != NULL)
	{
		DWORD MAX_BUFFER = (MaxLine * Size);
		BuxConvert(pbyBuffer, MAX_BUFFER);
		fwrite(pbyBuffer, MAX_BUFFER, 1u, fp);
		fclose(fp);
	}
}

void PackFileEncrypt(const char* filename, BYTE* pbyBuffer, int MaxLine, int Size, DWORD Key, bool WriteMax, bool CheckSum)
{
	FILE* fp = fopen(filename, "wb");
	if (fp != NULL)
	{
		DWORD MAX_BUFFER = (MaxLine * Size);
		BYTE* Buffer = new BYTE[MAX_BUFFER];
		if (WriteMax == true)
		{
			fwrite(&MaxLine, 4u, 1u, fp);
		}
		for (int i = 0; i < MaxLine; ++i)
		{
			BuxConvert(pbyBuffer, Size);
			memcpy((char*)(Buffer + Size * i), pbyBuffer, Size);
			fwrite((char*)(Buffer + Size * i), Size, 1u, fp);
			pbyBuffer += Size;
		}
		if (CheckSum)
		{
			DWORD dwCheckSum = GenerateCheckSum2(Buffer, MAX_BUFFER, Key);
			fwrite(&dwCheckSum, 4u, 1u, fp);
		}
		fclose(fp);
		delete[] Buffer;
	}
}
// SPK (E-06, Faz 2b.2-O): donor Util.cpp:871-914 — ServerDisplayer painter'lari
// (canli SPK hatti, gEventName uzerinden) WriteUnicode kullaniyor. IsCharUTF8
// donor'da mevcut degil bizde yok -> basit UTF-8 tespiti ile MultiByteToWideChar.
static bool Util_IsCharUTF8(LPCSTR lpString)
{
	if (lpString == 0)
	{
		return false;
	}

	int len = (int)strlen(lpString);

	for (int i = 0; i < len; i++)
	{
		if ((lpString[i] & 0x80) != 0)
		{
			return true;
		}
	}

	return false;
}

int ConvertCharToWideStr(std::wstring& wstrDest, LPCSTR lpString) // E-06
{
	wstrDest = L"";

	if (lpString == NULL || strlen(lpString) <= 0)
		return 0;

	int iConversionType;

	iConversionType = (Util_IsCharUTF8(lpString)) ? CP_UTF8 : 1252;

	int nLenOfWideCharStr = MultiByteToWideChar(iConversionType, 0, lpString, -1, NULL, 0);

	wchar_t* pwszStr = new wchar_t[nLenOfWideCharStr];

	MultiByteToWideChar(iConversionType, 0, lpString, -1, pwszStr, nLenOfWideCharStr);

	wstrDest += pwszStr;

	delete[] pwszStr;

	return nLenOfWideCharStr - 1;
}

BOOL WriteUnicode(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cbString) // E-06
{
	std::wstring wstrText = L"";
	ConvertCharToWideStr(wstrText, lpString);

	return TextOutW(hdc, nXStart, nYStart, wstrText.c_str(), wstrText.length());
}
