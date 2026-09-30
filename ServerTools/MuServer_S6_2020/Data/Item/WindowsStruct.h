#pragma once
struct flotantes
{
	int x;
	int y;
};

#define sub_7D9C50 ((bool(__stdcall*)(int a5))0x007D9C50)
#define sub_7DC2C0 ((int(__stdcall*)(int This, int a2))0x007DC2C0)
#define sub_7D9B50 ((void(__stdcall*)(int a4, int a5))0x007D9B50)
#define sub_7853F0 ((BYTE(__cdecl*)(char a4, int a5, int a6, int a7, int a8, int a9, int a10, char a11))0x007853F0)
#define sub_7DA7E0					((int(__stdcall *)(int a4, int a5))0x007DA7E0)
#define pDrawPuntero				((int(__thiscall*)(int This, char a5, int a6, char a7, char a8, char a9))0x00779350)
#define ChangeButtonInfo			((int(__thiscall*)(char *This, int X, int Y, int Width, int Height)) 0x00779410)
#define GetInstance					((int(__cdecl*)()) 0x00861110)
#define SetOption1					((char(__thiscall*)(int This))0x004EC950)
#define SetOption2					((char(__thiscall*)(int This))0x004EC970)

#define sub_969000					((BOOL(__thiscall*)(void * This))0x00969000)

#define sub_4EC9B0					((char(__thiscall*)(int This))0x004EC9B0)

#define sub_7DC240 ((int(__cdecl*)(int This, flotantes a2))0x007DC240)

void RenderTooltipAncestral_772EA0(int ThisR);
void MoverSlotItem(int This);

#define GetUIBaul		((int(__thiscall*)(int This))0x00861360)
void EventMuOffhelper(DWORD Event);
void InitSeason15();