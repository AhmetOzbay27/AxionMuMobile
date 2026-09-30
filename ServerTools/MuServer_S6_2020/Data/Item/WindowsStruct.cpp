#include "stdafx.h"
#include "Interface.h"
#include "WindowsStruct.h"
#include "Import.h"
#include "Util.h"
#include "TMemory.h"
#include "Offset.h"
#include "Object.h"
#include "Defines.h"
#include "PrintPlayer.h"
#include "User.h"
#include "SItemOption.h"
#include "Common.h"


signed int __cdecl ColorMoney(unsigned int a1)
{
	signed int color1 = eWhite; // eax@2

	if(a1 >= 1000 && a1 < 1000000)
	{
		color1 = eShinyGreen;
	}
	else if(a1 >= 1000000 && a1 < 10000000)
	{
		color1 = eGold;
	}
	else if(a1 >= 1000000 && a1 < 100000000)
	{
		color1 = eOrange;
	}
	else if(a1 >= 100000000)
	{
		color1 = eRed;
	}
	return color1;
}

void pDrawZenAndRud(int a1)  //-- ok
{
	float Y;
	float X;
	DWORD v23;
	Y = *(DWORD*)(a1 + 40);
	X = *(DWORD*)(a1 + 36);
	v23 = *(DWORD*)(*(DWORD*)0x8128AC4 + 5956);
	*(float*)(0x00D24E88); //Width

	char MoneyBuff1[50], MoneyBuff2[50];
	ZeroMemory(MoneyBuff1, sizeof(MoneyBuff1));
	ZeroMemory(MoneyBuff2, sizeof(MoneyBuff2));

	pGetMoneyFormat(v23, MoneyBuff1, 0);
	pGetMoneyFormat(Coin1, MoneyBuff2, 0);
	DWORD color1 = eWhite;
	DWORD color2 = eWhite;

	if(v23 > 0)
	{
		color1 = ColorMoney(v23);
	}
	if(Coin1 > 0)
	{
		color2 = ColorMoney(Coin1);
	}

	//-- Texto del Inventario
	gInterface.DrawFormat(eGold, X, Y + 27, 190, 3, pGetTextLine(pTextLineThis, 223));
	//-- Texto Money
	gInterface.DrawFormat(color1, X + 32, Y + 385, 55, 4, "%s", MoneyBuff1);
	//-- Texto Ruud
	gInterface.DrawFormat(color2, X + 32, Y + 398, 55, 4, "%s", MoneyBuff2);
}

#pragma optimize("t",on)

__declspec (naked) void RenderMoveSlotInventory()
{
	static DWORD Addr_Jmp = 0x0083445D;
	static DWORD CallMe = 0x007DC240;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP + 0xC]
		ADD ECX,215//215
		PUSH ECX                                 ; /Arg2
		MOV EDX,DWORD PTR SS:[EBP + 0x8]         ; |
		ADD EDX,15//22                               ; |
		PUSH EDX                                 ; |Arg1
		MOV EAX,DWORD PTR SS:[EBP - 0x4]         ; |
		MOV ECX,DWORD PTR DS:[EAX + 0x18]        ; |
		CALL [CallMe]                            ; \main1.007DC240
		JMP[Addr_Jmp]
	}
}

__declspec(naked) void RenderSlotEquip()  //-- ok
{
	static DWORD This; // ST00_4@1
	static DWORD Addrs = 0x00836871;
	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV This,EAX
	}
	//-- pet
	*(DWORD *)(This + 204) = *(DWORD *)(This + 36) + 14;//-- X
	*(DWORD *)(This + 208) = *(DWORD *)(This + 40) + 54;//-- Y
	*(DWORD *)(This + 212) = 35;//-- W
	*(DWORD *)(This + 216) = 40;//-- H
	*(DWORD *)(This + 220) = 51522;//-- Texture
	//-- weapon(L)
	*(DWORD *)(This + 44) = *(DWORD *)(This + 36) + 14;
	*(DWORD *)(This + 48) = *(DWORD *)(This + 40) + 97;
	*(DWORD *)(This + 52) = 35;
	*(DWORD *)(This + 56) = 56;
	*(DWORD *)(This + 60) = 51522;
	//-- Gloves
	*(DWORD *)(This + 144) = *(DWORD *)(This + 36) + 14;
	*(DWORD *)(This + 148) = *(DWORD *)(This + 40) + 155;
	*(DWORD *)(This + 152) = 35;
	*(DWORD *)(This + 156) = 40;
	*(DWORD *)(This + 160) = 61523;
	//-- helm
	*(DWORD *)(This + 84) = *(DWORD *)(This + 36) + 78;
	*(DWORD *)(This + 88) = *(DWORD *)(This + 40) + 54;
	*(DWORD *)(This + 92) = 35;
	*(DWORD *)(This + 96) = 40;
	*(DWORD *)(This + 100) = 61524;
	//-- Armor
	*(DWORD *)(This + 104) = *(DWORD *)(This + 36) + 78;
	*(DWORD *)(This + 108) = *(DWORD *)(This + 40) + 97;
	*(DWORD *)(This + 112) = 36;
	*(DWORD *)(This + 116) = 56;
	*(DWORD *)(This + 120) = 51522;
	//-- Pants
	*(DWORD *)(This + 124) = *(DWORD *)(This + 36) + 78;
	*(DWORD *)(This + 128) = *(DWORD *)(This + 40) + 155;
	*(DWORD *)(This + 132) = 36;
	*(DWORD *)(This + 136) = 40;
	*(DWORD *)(This + 140) = 51522;
	//-- wings
	*(DWORD *)(This + 184) = *(DWORD *)(This + 36) + 122;
	*(DWORD *)(This + 188) = *(DWORD *)(This + 40) + 54;
	*(DWORD *)(This + 192) = 55;
	*(DWORD *)(This + 196) = 40;
	*(DWORD *)(This + 200) = 51522;
	//-- weapon(R)
	*(DWORD *)(This + 64) = *(DWORD *)(This + 36) + 142;
	*(DWORD *)(This + 68) = *(DWORD *)(This + 40) + 97;
	*(DWORD *)(This + 72) = 35;
	*(DWORD *)(This + 76) = 56;
	*(DWORD *)(This + 80) = 51522;
	//-- Bootas
	*(DWORD *)(This + 164) = *(DWORD *)(This + 36) + 142;
	*(DWORD *)(This + 168) = *(DWORD *)(This + 40) + 155;
	*(DWORD *)(This + 172) = 35;
	*(DWORD *)(This + 176) = 40;
	*(DWORD *)(This + 180) = 51522;
	//-- anillo 2
	*(DWORD *)(This + 264) = *(DWORD *)(This + 36) + 117;
	*(DWORD *)(This + 268) = *(DWORD *)(This + 40) + 169;
	*(DWORD *)(This + 272) = 22;
	*(DWORD *)(This + 276) = 25;
	*(DWORD *)(This + 280) = 51522;
	//-- pendiente
	*(DWORD *)(This + 224) = *(DWORD *)(This + 36) + 52;
	*(DWORD *)(This + 228) = *(DWORD *)(This + 40) + 68;
	*(DWORD *)(This + 232) = 22;
	*(DWORD *)(This + 236) = 25;
	*(DWORD *)(This + 240) = 51522;
	// anillo 1
	*(DWORD *)(This + 244) = *(DWORD *)(This + 36) + 52;
	*(DWORD *)(This + 248) = *(DWORD *)(This + 40) + 169;
	*(DWORD *)(This + 252) = 22;
	*(DWORD *)(This + 256) = 25;
	*(DWORD *)(This + 260) = 51522;

	_asm
	{
		JMP [Addrs]
	}
}

__declspec(naked) void RenderSlotFix()  //-- ok
{
	static DWORD Addrs = 0x007DBA76;
	
	_asm
	{
		JMP [Addrs]
	}
}

__declspec (naked) void RemoveButton()  //-- ok
{
	static DWORD Addrs = 0x00836A58;
	
	_asm
	{
		JMP [Addrs]
	}
}
#pragma optimize("t",off)

void RenderWindowsCharacter(int a1)	//-- incompleto
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(a1 + 20);
	x = (double)*(signed int *)(a1 + 16);
	pDrawGUI(61526, x, y, 190.0, 429.0);
}

void RemakeInventario(DWORD This)
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(This + 40);
	x = (double)*(signed int *)(This + 36);

	//-- Windows
	pDrawGUI(61522, x, y, 190.0, 429.0);

	//-- TextAncestral
	if((signed int)(unsigned __int8)SetOption1((int) pUserStat()) <=0 && (signed int)(unsigned __int8)SetOption2((int) pUserStat()) <=0)
	{
		gInterface.DrawButtonRender(eButtonAncient, x + 18.0f, 195.0f, 0, 0.0f);
		gInterface.DrawFormat(eGray100, x + 20, 197, 41, 1, pGetTextLine(pTextLineThis, 989));
	}
	else
	{
		gInterface.DrawButtonRender(eButtonAncient, x + 18.0f, 195.0f, 0, 34.0f);
		gInterface.DrawFormat(eWhite, x + 20, 197, 41, 1, pGetTextLine(pTextLineThis, 989));
	}
	
	//-- socket
	if ( !sub_969000((void *)0x986C1B8) )
	{
		pDrawButton(61525, x + 65.0f, 195.0f, 43.0f, 17.0f, 0, 0.0);
		gInterface.DrawFormat(eGray100, x + 65, 197, 41, 1, pGetTextLine(pTextLineThis, 2651));
	}
	else
	{
		pDrawButton(61525, x + 65.0f, 195.0f, 43.0f, 17.0f, 0, 34.0f);
		gInterface.DrawFormat(eWhite, x + 65, 197, 41, 1, pGetTextLine(pTextLineThis, 2651));
	}

	gCItemSetOption.InitInfoTooltip(x, 195.0f);
	

	DWORD colorR = eWhite;
	bool repair = true;
	float scaleY = 34.0;

	if(gObjUser.getLevel < 50)
	{
		repair = false;
		scaleY = 0.0;
		colorR = eGray100;
	}

	if (gInterface.IsWorkZone(eButtonRepair) && repair == true)
	{
		if (gInterface.Data[eButtonRepair].OnClick)
		{
			gInterface.DrawButtonRender(eButtonRepair, x + 110, 195.0f, 0, 51.0f);
		}
		else
		{
			gInterface.DrawButtonRender(eButtonRepair, x + 110, 195.0f, 0, 17.0f);
		}
	}
	else
	{
		gInterface.DrawButtonRender(eButtonRepair, x + 110, 195.0f, 0, scaleY);
	}

	gInterface.DrawFormat(colorR, x + 110, 197, 43, 3, "Repair");
	
	//--
	if (gInterface.IsWorkZone(eButtonStorage))
	{
		if (gInterface.Data[eButtonStorage].OnClick)
		{
			gInterface.DrawButtonRender(eButtonStorage, x + 90, 387, 0, 51.0f);
		}
		else
		{
			gInterface.DrawButtonRender(eButtonStorage, x + 90, 387, 0, 17.0f);
		}
	}
	else
	{
		gInterface.DrawButtonRender(eButtonStorage, x + 90, 387, 0, 34.0);
	}

	gInterface.DrawFormat(eWhite, x + 90, 389, 43, 3, "Store");
	
	//--
	if (gInterface.IsWorkZone(eButtonInvExt))
	{
		if (gInterface.Data[eButtonInvExt].OnClick)
		{
			gInterface.DrawButtonRender(eButtonInvExt, x + 135, 387, 0, 51.0f);
		}
		else
		{
			gInterface.DrawButtonRender(eButtonInvExt, x + 135, 387, 0, 17.0f);
		}
	}
	else
	{
		gInterface.DrawButtonRender(eButtonInvExt, x + 135, 387, 0, 34.0);
	}

	gInterface.DrawFormat(eWhite, x + 135, 389, 43, 3, "Inv. Ext");

	//-- Repair
	ChangeButtonInfo((char *)(This + 288), x + 110, 195, 43.0, 17.0);
	//-- store
	ChangeButtonInfo((char *)(This + 632), x + 90, 387, 43.0, 17.0);
	//-- Inv Ext
	ChangeButtonInfo((char *)(This + 804), x + 135, 387, 43.0, 17.0);
	//-- Cerrar
	pDrawPuntero(This + 460, 1, 61529, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 460), x + 160, y + 27, 11, 12);
}

__declspec (naked) void RemoveButtonStore1()  //-- ok
{
	static DWORD Addrs = 0x0083BB67;
	
	_asm
	{
		JMP [Addrs]
	}
}

__declspec (naked) void RemoveButtonStore2()  //-- ok
{
	static DWORD Addrs = 0x0083BC07;
	
	_asm
	{
		JMP [Addrs]
	}
}

__declspec (naked) void RenderWindowsInventory()
{
	static DWORD Addr_JMP = 0x00836FF4;
	static DWORD This;

	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EAX, DWORD PTR SS:[EBP-4]
		MOV This, EAX
	}

	RemakeInventario(This);

	_asm
	{
		JMP[Addr_JMP]
	}
}

int SetTextSocket(int a1)
{
	return 1;
}

char SetTextAncestral(int This)
{
	HGDIOBJ v4; // ST18_4@1
	void *v5; // eax@1
	void *v6; // eax@1
	int v7; // eax@1
	int v8; // eax@2
	void *v9; // eax@3
	void *v10; // eax@4
	char v11; // al@5
	int v12; // ST04_4@5
	int v13; // ST00_4@5
	void *v14; // eax@5
	int v15; // eax@5
	char result; // al@5
	int v17; // [sp+14h] [bp-84h]@1
	char v18; // [sp+18h] [bp-80h]@5

	result = sub_4EC9B0(0xE8CDE8);
	if( result == 1)
		RenderTooltipAncestral_772EA0(This);

	return result;
}

__declspec (naked) void RenderDrawTitleShop()
{
	static DWORD Addr_JMP = 0x0084723D;
	static DWORD Addr1 = 0x00420150;
	static DWORD Addr2 = 0x0041FE10;

	_asm
	{
		PUSH EAX
		MOV ECX,DWORD PTR SS:[EBP-0x104]
		MOV EDX,DWORD PTR DS:[ECX+0x18]
		ADD EDX,28
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0x104]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		PUSH ECX
		CALL [Addr2]
		MOV ECX,EAX                              ; |
		CALL [Addr1]
		JMP[Addr_JMP]
	}
}

__declspec (naked) void RenderSlotGeneral1()
{
	static DWORD Addr_JMP = 0x007DB5D0;
	static float witdh = 21.0f;
	_asm
	{
		PUSH ECX
		FLD DWORD PTR DS:[witdh]
		FSTP DWORD PTR SS:[ESP]
		PUSH ECX
		FLD DWORD PTR DS:[witdh]
		FSTP DWORD PTR SS:[ESP]
		MOV EAX,DWORD PTR SS:[EBP - 0x4]
		IMUL EAX,EAX, 0x14
		MOV ECX,DWORD PTR SS:[EBP - 0xB0]
		MOV EDX,DWORD PTR DS:[ECX + 0x2C]
		ADD EDX,EAX
		MOV DWORD PTR SS:[EBP - 0xBC],EDX
		FILD DWORD PTR SS:[EBP - 0xBC]
		PUSH ECX
		FSTP DWORD PTR SS:[ESP]
		MOV EAX,DWORD PTR SS:[EBP - 0x8]
		IMUL EAX,EAX, 0x14
		MOV ECX,DWORD PTR SS:[EBP - 0xB0]
		MOV EDX,DWORD PTR DS:[ECX + 0x28]
		ADD EDX,EAX
		MOV DWORD PTR SS:[EBP - 0xC0],EDX
		FILD DWORD PTR SS:[EBP - 0xC0]
		PUSH ECX
		FSTP DWORD PTR SS:[ESP]
		PUSH 61527
		JMP [Addr_JMP]
	}
}

//=======================================
//-- Render Windows Shop
//=======================================
signed int __cdecl sub_5C1130(unsigned int a1)
{
	signed int result; // eax@2

	if ( a1 < 0x989680 )
	{
		if ( a1 < 0xF4240 )
		{
			if ( a1 < 0x186A0 )
				result = -6890241;
			else
				result = -15152896;
		}
		else
		{
			result = -16738561;
		}
	}
	else
	{
		result = -16776961;
	}
	return result;
}

void RenderWindowsShop(int a1)
{
	float y; // ST08_4@1
	float x; // ST04_4@1
	char MoneyBuff1 [50];
	DWORD v23;
	DWORD Color;
	y = (double)*(signed int *)(a1 + 24);
	x = (double)*(signed int *)(a1 + 20);

	pDrawGUI(61526, x, y, 190.0, 429.0);
	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, pGetTextLine(pTextLineThis, 230));

	if ( *(BYTE *)(a1 + 36) )
	{
		gInterface.DrawFormat(eTextShop, x + 40, y + 368, 170, 1, pGetTextLine(pTextLineThis, 239));
		//-- Total Zen Repair
		v23 = *(DWORD *)0x81F6BC8;
		pDrawGUI(61528, x + 97, y + 365, 81.0, 16.0);
		pGetMoneyFormat((double)v23, MoneyBuff1, 0);

		Color = eWhite;

		if(v23 > 0)
		{
			Color = ColorMoney(v23);
		}

		gInterface.DrawFormat(Color, x + 117, y + 368, 73, 1, MoneyBuff1);
		
		//--
		if (gInterface.IsWorkZone(eButtonRepairShop))
		{
			if (gInterface.Data[eButtonRepairShop].OnClick)
			{
				gInterface.DrawButtonRender(eButtonRepairShop, x + 54, 390, 0, 51.0f);
			}
			else
			{
				gInterface.DrawButtonRender(eButtonRepairShop, x + 54, 390, 0, 17.0f);
			}
		}
		else
		{
			gInterface.DrawButtonRender(eButtonRepairShop, x + 54, 390, 0, 34.0);
		}

		gInterface.DrawFormat(eWhite, x + 54, 392, 43, 3, "Repair");
		//-- Buttons Repair
		pDrawPuntero(a1 + 40, 1, 51522, 0, 0, 0);
		ChangeButtonInfo((char *)(a1 + 40), *(DWORD *)(a1 + 20) + 54, *(DWORD *)(a1 + 24) + 390, 43, 17);

		if (gInterface.IsWorkZone(eButtonRepairAll))
		{
			if (gInterface.Data[eButtonRepairAll].OnClick)
			{
				gInterface.DrawButtonRender(eButtonRepairAll, x + 98, 390, 0, 51.0f);
			}
			else
			{
				gInterface.DrawButtonRender(eButtonRepairAll, x + 98, 390, 0, 17.0f);
			}
		}
		else
		{
			gInterface.DrawButtonRender(eButtonRepairAll, x + 98, 390, 0, 34.0);
		}

		gInterface.DrawFormat(eWhite, x + 98, 392, 43, 3, "Repair All");

		pDrawPuntero(a1 + 212, 1, 51522, 0, 0, 0);
		ChangeButtonInfo((char *)(a1 + 212), x + 98, y + 390, 43, 17);
	}
	//-- slot shop
	if ( *(DWORD *)(a1 + 16) )
	{
		*(DWORD *)(*(DWORD *)(a1 + 16) + 44) = 55;
	}
}

__declspec (naked) void WindowsShop()
{
	static DWORD Addr_JMP = 0x008471A4;
	static DWORD a1;
	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV a1, EAX
	}

	RenderWindowsShop(a1);

	_asm
	{
		JMP[Addr_JMP]
	}
}
//=======================================
//-- Render Windows Inv. Ext.
//=======================================
__declspec (naked) void RenderMoveSlotExt()
{
	static DWORD Addr_JMP = 0x0083C5F1;
	static DWORD Addr_Call = 0x009CEBF0;

	_asm
	{
		FILD DWORD PTR DS:[EAX + 0x4]
		FSTP DWORD PTR SS:[EBP - 0x4]
		FLD DWORD PTR SS:[EBP - 0x4]
		CALL [Addr_Call]
		ADD EAX, 15//-- 20
		PUSH EAX
		FLD DWORD PTR SS:[EBP-0x8]
		CALL [Addr_Call]
		PUSH EAX                                  ; |Arg1
		MOV EAX,DWORD PTR SS:[EBP-0xC]            ; |
		MOV ECX,DWORD PTR SS:[EBP-0x10]           ; |
		MOV ECX,DWORD PTR DS:[ECX+EAX*0x4+0x18]   ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderSlotMove()  //-- ok
{
	static DWORD Addrs = 0x007DC259;
	static DWORD This;
	static DWORD a2;
	static DWORD a3;

	_asm
	{
		PUSH EBP
		MOV EBP,ESP
		PUSH ECX
		MOV DWORD PTR SS:[EBP-0x4],ECX
		MOV EAX,DWORD PTR SS:[EBP-0x4]
		MOV This, EAX
		MOV ECX,DWORD PTR SS:[EBP+0x8]
		MOV a2, ECX
		MOV EAX,DWORD PTR SS:[EBP+0xC]
		MOV a3, EAX
	}

	*(DWORD *)(This + 40) = a2;
	*(DWORD *)(This + 44) = a3 + 5;

	_asm
	{
		JMP [Addrs]
	}
}

void pDrawRenderImg(DWORD id, float X, float Y,float W, float H)
{
	Y += 20;

	pDrawGUI(id, X, Y, W, H);
}

void RenderWindowsInventoryExt(int a1)
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(a1 + 24);
	x = (double)*(signed int *)(a1 + 20);

	pDrawGUI(61526, x, y, 190.0, 429.0);

	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, pGetTextLine(pTextLineThis, 3323));
	//61529
	pDrawPuntero(a1 + 52, 1, 61529, 0, 0, 0);
	ChangeButtonInfo((char *)(a1 + 52), x + 160, y + 27, 11, 12);

}

__declspec (naked) void RenderWindowsExt()
{
	static DWORD Addr_JMP = 0x007D56A4;
	static DWORD This;

	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EAX, DWORD PTR SS:[EBP-4]
		MOV This, EAX
	}

	RenderWindowsInventoryExt(This);

	_asm
	{
		JMP[Addr_JMP]
	}
}

void RenderWindowsGensBattle(int a1)	//-- incompleto
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(a1 + 20);
	x = (double)*(signed int *)(a1 + 16);
	pDrawGUI(61526, x, y, 190.0, 429.0);
	//-- Cerrar
	//pDrawPuntero(a1 + 24, 1, 61529, 0, 0, 0);
	//ChangeButtonInfo((char *)(a1 + 24), x + 160, y + 27, 11, 12);
}

void LoadWindows(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(61526, x, y, w, h);
}

void LoadWindowsNone(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(51522, x, y, w, h);
}

__declspec (naked) void InitWindowsChaos()
{
	static DWORD Addr_JMP = 0x0082CB50;
	_asm
	{
		JMP [Addr_JMP]
	}
}

void RenderMuHelper(int This)
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(This + 204);
	x = (double)*(signed int *)(This + 200);

	//-- Windows
	pDrawGUI(61526, x, y, 190.0, 429.0);
	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, pGetTextLine(pTextThis(),3536));

	if(*(DWORD *)(This + 216) != 0)
	{
		if( gInterface.IsWorkZone(ButtonStartAttack) )
		{
			if( gInterface.Data[ButtonStartAttack].OnClick )
			{
				if(offhelper == 0)
				{
					offhelper = 1;
				}
				else
				{
					offhelper = 0;
				}
				gInterface.Data[ButtonStartAttack].OnClick = false;
			}
		}
		if(offhelper == 1)
		{
			gInterface.DrawButtonRender(ButtonStartAttack, x + 79, y + 100, 0, 0);
		}
		else
		{
			gInterface.DrawButtonRender(ButtonStartAttack, x + 79, y + 100, 0, 15);
		}
		gInterface.DrawFormat(eGold, x + 79 + 16, y + 100, 50, 1, "MuOffHelper");
	}
}

void EventMuOffhelper(DWORD Event)
{
	if((GetTickCount() - gInterface.Data[ButtonStartAttack].EventTick) < 1000)
	{
		return;
	}

	if( gInterface.IsWorkZone(ButtonStartAttack) )
	{
		if( Event == WM_LBUTTONDOWN )
		{
			gInterface.Data[ButtonStartAttack].OnClick = true;
			return;
		}
		// ----
		gInterface.Data[ButtonStartAttack].OnClick = false;
		
		// ----
		gInterface.Data[ButtonStartAttack].EventTick = GetTickCount();
		// ----
	}
}

__declspec(naked) void RenderWindowsGuildMaker()
{
	static DWORD Addrs = 0x007D1884;
	static DWORD RenderBits = 0x00790B50;
	_asm
	{
		PUSH ECX                                 ; /Arg4
		FLD DWORD PTR DS:[0xD24E88]                ; |
		FSTP DWORD PTR SS:[ESP]                  ; |
		MOV EAX,DWORD PTR SS:[EBP-0x6C]            ; |
		FILD DWORD PTR DS:[EAX+0x14]               ; |
		PUSH ECX                                 ; |Arg3
		FSTP DWORD PTR SS:[ESP]                  ; |
		MOV ECX,DWORD PTR SS:[EBP-0x6C]            ; |
		FILD DWORD PTR DS:[ECX+0x10]               ; |
		PUSH ECX                                 ; |Arg2
		FSTP DWORD PTR SS:[ESP]                  ; |
		PUSH 61526                                ; |Arg1 = 00007A5A
		CALL RenderBits                      ; \main1.00790B50
		JMP[Addrs]
	}
}

void RenderBaul(int This)
{
	float y; // ST08_4@1
	float x; // ST04_4@1
	signed int v23; // [sp+10h] [bp-9Ch]@8
	signed int v29; // [sp+10h] [bp-9Ch]@8
	int v30; // [sp+24h] [bp-88h]@1
	int v33; // [sp+24h] [bp-88h]@1

	y = (double)*(signed int *)(This + 20);
	x = (double)*(signed int *)(This + 16);

	//-- Windows
	pDrawGUI(61526, x, y, 190.0, 429.0);
	
	char titulo [64];
	wsprintf(titulo,"%s (%s)",pGetTextLine(pTextThis(),234), pGetTextLine(pTextThis(),240));
	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, titulo);


	char BufferZen1[64];
	char BufferZen2[64];
	pDrawGUI(61528, x + 70, y + 356, 81.0, 16.0);
	pGetMoneyFormat((double)*(DWORD *)(*(DWORD *)0x8128AC4 + 5960), BufferZen1, 0);
	gInterface.DrawFormat(ColorMoney((double)*(DWORD *)(*(DWORD *)0x8128AC4 + 5960)), x + 90, y + 358, 55, 4, BufferZen1);


	v23 = *(WORD *)0x87935D8 + *(WORD *)(*(DWORD *)0x8128AC8 + 14);
	v33 = (double)v23 * (double)v23 * 0.04;
	if ( *(BYTE *)(This + 548) )
		v30 = 2 * *(WORD *)(*(DWORD *)0x8128AC8 + 14);
	else
		v30 = 0;
	v33 += v30;
	if ( v33 >= 1 )
		v29 = v33;
	else
		v29 = 1;
	v33 = v29;
	if ( v29 < 1000 )
	{
		if ( v33 >= 100 )
			v33 = 10 * (v33 / 10);
	}
	else
	{
		v33 = 100 * (v33 / 100);
	}

	pDrawGUI(61528, x + 70, y + 372, 81.0, 16.0);
	gInterface.DrawFormat(eGold, x + 5, y + 374, 70, 3, pGetTextLine(pTextThis(), 266));
	pGetMoneyFormat((double)v33, BufferZen1, 0);
	gInterface.DrawFormat(ColorMoney((double)v33), x + 90, y + 374, 55, 4, BufferZen1);

	//-- meter zen
	pDrawPuntero(This + 24, 1, 51522, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 24), x + 9, y + 390, 43, 17);
	if (gInterface.IsWorkZone(eButton1))
		gInterface.DrawButtonRender(eButton1, x + 9, y + 390, 0, 17.0f);
	else
		gInterface.DrawButtonRender(eButton1, x + 9, y + 390, 0, 34.0);
	gInterface.DrawFormat(eGold, x + 9, y + 392, 43, 3, pGetTextLine(pTextThis(), 235));

	//-- sacar zen
	pDrawPuntero(This + 196, 1, 51522, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 196), x + 52, y + 390, 43, 17);

	if (gInterface.IsWorkZone(eButton2))
		gInterface.DrawButtonRender(eButton2, x + 52, y + 390, 0, 17.0f);
	else
		gInterface.DrawButtonRender(eButton2, x + 52, y + 390, 0, 34.0);
	gInterface.DrawFormat(eGold, x + 52, y + 392, 43, 3, pGetTextLine(pTextThis(), 236));
	//-- Lock/Unlock
	pDrawPuntero(This + 368, 1, 51522, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 368), x + 95, y + 390, 43, 17);
	if (gInterface.IsWorkZone(eButton3))
		gInterface.DrawButtonRender(eButton3, x + 95, y + 390, 0, 17.0f);
	else
		gInterface.DrawButtonRender(eButton3, x + 95, y + 390, 0, 34.0);

	if ( *(BYTE *)(This + 548) )
		gInterface.DrawFormat(eGold, x + 95, y + 392, 43, 3, "Unlock");
	else
		gInterface.DrawFormat(eGold, x + 95, y + 392, 43, 3, "Lock");

	//-- Btn Expansion
	pDrawPuntero(This + 572, 1, 51522, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 572), x + 138, y + 390, 43, 17);

	if (gInterface.IsWorkZone(eButton4))
		gInterface.DrawButtonRender(eButton4, x + 138, y + 390, 0, 17.0f);
	else
		gInterface.DrawButtonRender(eButton4, x + 138, y + 390, 0, 34.0);
	gInterface.DrawFormat(eGold, x + 138, y + 392, 43, 3, pGetTextLine(pTextThis(), 240));

	if ( *(DWORD *)(This + 540) )
	{
		*(DWORD *)(*(DWORD *)(This + 540) + 44) = 55;
	}
}

__declspec(naked) void RenderWindowsBaul()
{
	static DWORD Addr_JMP = 0x00857A8C;
	static DWORD This;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-0x4]            ; |
		MOV This, EAX
	}

	RenderBaul(This);

	_asm
	{
		JMP[Addr_JMP]
	}
}


__declspec(naked) void RenderWindowsStore()
{
	static DWORD Addr_JMP = 0x00841F47;
	static DWORD RenderBits = 0x00790B50;

	_asm
	{
		PUSH ECX                                 ; /Arg4
		FLD DWORD PTR DS:[0xD24E88]              ; |
		FSTP DWORD PTR SS:[ESP]                  ; |
		MOV EAX,DWORD PTR SS:[EBP-0x6C]          ; |
		FILD DWORD PTR DS:[EAX+0x18]             ; |
		PUSH ECX                                 ; |Arg3
		FSTP DWORD PTR SS:[ESP]                  ; |
		MOV ECX,DWORD PTR SS:[EBP-0x6C]          ; |
		FILD DWORD PTR DS:[ECX+0x14]             ; |
		PUSH ECX                                 ; |Arg2
		FSTP DWORD PTR SS:[ESP]                  ; |
		PUSH 61526                               ; |Arg1 = 00007A5A
		CALL RenderBits                          ; \main1.00790B50
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderTittleCommand()
{
	static DWORD Addr_JMP = 0x0078E4CA;
	static DWORD Addr1_Call = 0x00402320;
	static DWORD Addr2_Call = 0x0041FE10;

	_asm
	{
		PUSH 0
		PUSH 3
		PUSH 0
		PUSH 0x48
		PUSH 0x3AA                                 ; /Arg1 = 000003AA
		MOV ECX,0x08128ADC                   ; |
		CALL Addr1_Call                      ; \main1.00402320
		PUSH EAX
		MOV ECX,DWORD PTR SS:[EBP-0x1C]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX, 27
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0x1C]
		MOV ECX,DWORD PTR DS:[EAX+0x10]
		ADD ECX, 0x3C
		PUSH ECX
		CALL Addr2_Call
		MOV ECX,EAX
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderTittleQuestGlobal()
{
	static DWORD Addr_JMP = 0x00843B37;
	static DWORD Addr1_Call = 0x00521FE0;
	static DWORD Addr2_Call = 0x0041FE10;

	_asm
	{
		PUSH 0
		PUSH 3
		PUSH 0
		PUSH 0x0BE
		MOV ECX,0x00EBCF60                   ; |
		CALL Addr1_Call                      ; \main1.00402320
		PUSH EAX
		MOV ECX,DWORD PTR SS:[EBP-0xC]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX, 27
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0xC]
		MOV ECX,DWORD PTR DS:[EAX+0x10]
		PUSH ECX
		CALL Addr2_Call
		MOV ECX,EAX
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderGensPointText()
{
	static DWORD Addr_JMP = 0x00843DC4;
	static DWORD Addr_Call = 0x0041FE10;

	_asm
	{
		PUSH 0
		PUSH 3
		PUSH 0
		PUSH 0xBE
		LEA EDX,DWORD PTR SS:[EBP-0x20]
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0x24]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		ADD ECX,51
		PUSH ECX
		MOV EDX,DWORD PTR SS:[EBP-0x24]
		MOV EAX,DWORD PTR DS:[EDX+0x10]
		PUSH EAX
		CALL Addr_Call
		MOV ECX,EAX                              ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderGensPointInfo()
{
	static DWORD Addr_JMP = 0x00843BCE;
	static DWORD Addr_Call = 0x0041FE10;

	_asm
	{
		PUSH 0
		PUSH 1
		PUSH 0
		PUSH 0
		MOV ECX,DWORD PTR SS:[EBP-0xC]
		MOV EDX,DWORD PTR DS:[ECX+0xC34]
		IMUL EDX,EDX,0x7
		ADD EDX,DWORD PTR SS:[EBP-0x4]
		SHL EDX,0x6
		MOV EAX,DWORD PTR SS:[EBP-0xC]
		LEA ECX,DWORD PTR DS:[EAX+EDX+0x374]
		PUSH ECX
		MOV EDX,DWORD PTR SS:[EBP-0xC]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		MOV ECX,DWORD PTR SS:[EBP-0x4]
		IMUL ECX,ECX,0xF
		LEA EDX,DWORD PTR DS:[EAX+ECX+0x50]
		PUSH EDX
		MOV EAX,DWORD PTR SS:[EBP-0xC]
		MOV ECX,DWORD PTR DS:[EAX+0x10]
		ADD ECX,0xD
		PUSH ECX
		CALL Addr_Call
		MOV ECX,EAX                             ; |
		JMP[Addr_JMP]
	}
}

void RenderGensPointTexture(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(id, x, y + 21, w, h);
}

__declspec(naked) void StoreButtonsTest()
{
	static DWORD Addr_JMP = 0x0078E485;
	static DWORD Addr_Call = 0x0041FE10;

	_asm
	{
		JMP[Addr_JMP]
	}
}

void RenderTooltipAncestral_772EA0(int ThisR)
{
	static DWORD Addr = 0x00835C70;
	static DWORD Addr_Call = 0x00772EA0;
	static DWORD This = 0x00772EA0;

	This = ThisR;

	_asm
	{
		PUSH 1                              ; /Arg5 = 00000000
		PUSH 0                              ; |Arg4 = 00000000
		MOV EAX,This                        ; |
		PUSH EAX                            ; |Arg3
		PUSH Addr                           ; |Arg2 = 00815040
		PUSH ECX                            ; |Arg1
		FLD DWORD PTR DS:[0xD2CA40]         ; |
		FSTP DWORD PTR SS:[ESP]             ; |
		MOV ECX, This                       ; |
		MOV ECX, DWORD PTR DS:[ECX + 0x14]  ; |
		CALL Addr_Call                      ; \main1.00772EA0
	}
}

__declspec(naked) void RenderHelperTittle()
{
	static DWORD Addr_JMP = 0x007F673B;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-0x2D8]
		MOV EDX,DWORD PTR DS:[ECX+0xCC]
		ADD EDX,27
		JMP[Addr_JMP]
	}
}

void RenderButtonQuestTb(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(id,(double) x + 1,(double) y + 26, w, h);
}

__declspec(naked) void RenderText1Quest()
{
	static DWORD Addr_JMP = 0x0083F6D9;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-0x4]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText2Quest()
{
	static DWORD Addr_JMP = 0x0083F731;

	_asm
	{
		MOV EDX,DWORD PTR SS:[EBP-0x4]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		ADD EAX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText3Quest()
{
	static DWORD Addr_JMP = 0x0083F81D;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		ADD ECX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText4Quest()
{
	static DWORD Addr_JMP = 0x0083F875;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-4]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText5Quest()
{
	static DWORD Addr_JMP = 0x0083F8AD;

	_asm
	{
		MOV EDX,DWORD PTR SS:[EBP-4]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		ADD EAX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText6Quest()
{
	static DWORD Addr_JMP = 0x0083F961;

	_asm
	{
		MOV EDX,DWORD PTR SS:[EBP-4]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		ADD EAX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText7Quest()
{
	static DWORD Addr_JMP = 0x0083F9B9;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		ADD ECX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText8Quest()
{
	static DWORD Addr_JMP = 0x0083F9F1;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-4]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText9Quest()
{
	static DWORD Addr_JMP = 0x0083F769;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		ADD ECX,59
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderQuestPlayerTittle()
{
	static DWORD Addr_JMP = 0x0083EBB0;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-4]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX,27
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderButtonImg()
{
	static DWORD Addr_JMP = 0x0083F37C;

	_asm
	{
		PUSH 0                                   ; /Arg5 = 00000000
		PUSH 0                                   ; |Arg4 = 00000000
		PUSH 0                                   ; |Arg3 = 00000000
		PUSH 61529                                ; |Arg2 = 00007A8A
		PUSH 1                                   ; |Arg1 = 00000001
		MOV ECX,DWORD PTR SS:[EBP-0x10]            ; |
		ADD ECX,0x18                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderButtonInfo()
{
	static DWORD Addr_JMP = 0x0083F3A2;

	_asm
	{
		PUSH 0x1D                                  ; /Arg4 = 0000001D
		PUSH 0x24                                  ; |Arg3 = 00000024
		MOV EAX,DWORD PTR SS:[EBP-0x10]            ; |
		MOV ECX,DWORD PTR DS:[EAX+0x14]            ; |
		ADD ECX,27                              ; |
		PUSH ECX                                 ; |Arg2
		MOV EDX,DWORD PTR SS:[EBP-0x10]            ; |
		MOV EAX,DWORD PTR DS:[EDX+0x10]            ; |
		ADD EAX,160                               ; |
		PUSH EAX                                 ; |Arg1
		MOV ECX,DWORD PTR SS:[EBP-0x10]            ; |
		ADD ECX,0x18                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderQuestTabPane1()
{
	static DWORD Addr_JMP = 0x0083F4F0;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-4]             ; |
		MOV EDX,DWORD PTR DS:[ECX+20]            ; |
		ADD EDX,53                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderQuestTabPane2()
{
	static DWORD Addr_JMP = 0x0083F51F;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]             ; |
		MOV ECX,DWORD PTR DS:[EAX+20]            ; |
		ADD ECX,53                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderQuestTabPane3()
{
	static DWORD Addr_JMP = 0x0083F555;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]             ; |
		MOV ECX,DWORD PTR DS:[EAX+20]            ; |
		ADD ECX,53                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderQuestTabPane4()
{
	static DWORD Addr_JMP = 0x0083F58B;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-4]             ; |
		MOV ECX,DWORD PTR DS:[EAX+20]            ; |
		ADD ECX,53                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderInfoQuest1()
{
	static DWORD Addr_JMP = 0x0083ED89;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-12]            ; |
		MOV EDX,DWORD PTR DS:[ECX+20]            ; |
		ADD EDX,80                               ; |
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderInfoQuest2()
{
	static DWORD Addr_JMP = 0x0083EDE7;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-0xC]
		MOV ECX,DWORD PTR DS:[EAX+20]
		ADD ECX,98
		JMP[Addr_JMP]
	}
}
//  /$ 55             PUSH EBP


void windowsquestPlayer(int This)
{
	float y; // ST08_4@1
	float x; // ST04_4@1

	y = (double)*(signed int *)(This + 20);
	x = (double)*(signed int *)(This + 16);

	//-- Windows
	pDrawGUI(61526, x, y, 190.0, 429.0);
	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, "Quest");
	//-- Cerrar
	pDrawPuntero(This + 24, 1, 61529, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 24), x + 160, y + 27, 11, 12);

	pDrawPuntero(This + 196, 1, 61525, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 196), x + 50, y + 392, 43, 17);

	pDrawPuntero(This + 368, 1, 61525, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 368), x + 94, y + 392, 43, 17);

}

__declspec(naked) void RenderWindowsQuest()
{
	static DWORD Addrs = 0x0083EB44;
	static DWORD RenderBits = 0x00790B50;
	static DWORD This;
	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EDX,DWORD PTR SS:[EBP-4]
		MOV This, EDX
	}
	
	windowsquestPlayer(This);

	_asm
	{
		JMP[Addrs]
	}
}

void ImprimirTitulo(int x, int y, int LineText)
{
	gInterface.DrawFormat(eGold, x, y + 27, 190, 3, pGetTextLine(pTextLineThis, LineText));
}

void PrintButtonClose(int This, int x, int y)
{
	//-- Cerrar
	pDrawPuntero(This, 1, 61529, 0, 0, 0);
	ChangeButtonInfo((char *)(This), x + 160, y + 27, 11, 12);
}

__declspec(naked) void SetTittleParty()
{
	static DWORD Addr_JMP = 0x0084A6B2;
	static DWORD x;
	static DWORD y;
	static DWORD This;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-0x18]
		MOV ECX,DWORD PTR DS:[EAX+0x14]
		MOV y, ECX
		MOV EDX,DWORD PTR SS:[EBP-0x18]
		MOV EAX,DWORD PTR DS:[EDX+0x10]
		MOV x, EAX
	}
	
	ImprimirTitulo(x, y, 190);

	_asm
	{
		JMP[Addr_JMP]
	}
}

__declspec(naked) void SetCloseParty()
{
	static DWORD Addr_JMP = 0x0084A127;
	static DWORD Addr1_Call = 0x00779350;
	static DWORD Addr2_Call = 0x00779410;

	_asm
	{
		PUSH 0                                   ; /Arg5 = 00000000
		PUSH 0                                   ; |Arg4 = 00000000
		PUSH 0                                   ; |Arg3 = 00000000
		PUSH 61529                               ; |Arg2 = 00007A8A
		PUSH 1                                   ; |Arg1 = 00000001
		MOV ECX,DWORD PTR SS:[EBP-0x10]          ; |
		ADD ECX,0x18                             ; |
		CALL Addr1_Call                      ; \main1.00779350
		PUSH 12                                  ; /Arg4 = 0000001D
		PUSH 11                                  ; |Arg3 = 00000024
		MOV EAX,DWORD PTR SS:[EBP-0x10]          ; |
		MOV ECX,DWORD PTR DS:[EAX+0x14]          ; |
		ADD ECX,27                               ; |
		PUSH ECX                                 ; |Arg2
		MOV EDX,DWORD PTR SS:[EBP-0x10]          ; |
		MOV EAX,DWORD PTR DS:[EDX+0x10]          ; |
		ADD EAX,160                              ; |
		PUSH EAX                                 ; |Arg1
		MOV ECX,DWORD PTR SS:[EBP-0x10]            ; |
		ADD ECX,0x18                               ; |
		CALL Addr2_Call                      ; \main1.00779410
		JMP[Addr_JMP]
	}
}

__declspec(naked) void SetTittlePet()
{
	static DWORD Addr_JMP = 0x0084D1CD;
	static DWORD x;
	static DWORD y;

	_asm
	{
		MOV EAX,DWORD PTR SS:[EBP-0x10C]
		MOV ECX,DWORD PTR DS:[EAX+0xE8]
		MOV y, ECX
		MOV EDX,DWORD PTR SS:[EBP-0x10C]
		MOV EAX,DWORD PTR DS:[EDX+0xE4]
		MOV x, EAX
	}
	
	ImprimirTitulo(x, y, 1217);

	_asm
	{
		JMP[Addr_JMP]
	}
}

void LoadTextureImg(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(id, x, y + 15, w, h);
}

void LoadTextureMoney(DWORD id, float x, float y, float w, float h)
{
	pDrawGUI(61528, x + 97, y + 10, 81.0, 16.0);
}

void OpenWindowsTrade(int This)
{
	float x;
	float y;
	y = (double)*(signed int *)(This + 20);
	x = (double)*(signed int *)(This + 16);

	LoadWindows(1, x, y, 190, 429);
		
	ImprimirTitulo(x, y, 226);
	//-- Nombre de PJ1
	pDrawGUI(31563, x + 11, y + 52.0, 170.0, 26.0);
	//-- Nombre de PJ2
	pDrawGUI(31563, x + 11, y + 243, 171.0, 26.0);


	if ( *(DWORD *)(This + 376) )
	{
		*(DWORD *)(*(DWORD *)(This + 376) + 44) = 81;
	}

	if ( *(DWORD *)(This + 380) )
	{
		*(DWORD *)(*(DWORD *)(This + 380) + 44) = 273;
	}
	char bufferZen1[64];
	char bufferZen2[64];
	pGetMoneyFormat((double)*(signed int *)(This + 3828), bufferZen1, 0);
	pGetMoneyFormat((double)*(signed int *)(This + 3832), bufferZen2, 0);
	pDrawGUI(61528, x + 97, y + 165, 81.0, 16.0);
	gInterface.DrawFormat(ColorMoney((double)*(signed int *)(This + 3828)), x + 117, y + 167, 55,4,bufferZen1);
	pDrawGUI(61528, x + 97, y + 356, 81.0, 16.0);
	gInterface.DrawFormat(ColorMoney((double)*(signed int *)(This + 3832)), x + 117, y + 358, 55,4,bufferZen2);

	pDrawButton(61525, x + 138, y + 186, 43.0f, 17.0f, 0, ((*(BYTE *)(This + 3840) != 0 ) ? 17.0f : 34.0f ));
	gInterface.DrawFormat(eWhite, x + 138, y + 188, 43, 3, "Ok");
	pDrawButton(61525, x + 138, y + 390, 43.0f, 17.0f, 0, ((*(BYTE *)(This + 3841) != 0 ) ? 17.0f : 34.0f ));
	gInterface.DrawFormat(eWhite, x + 138, y + 392, 43, 3, "Ok");

	//-- Cerrar
	pDrawPuntero(This + 24, 1, 61529, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 24), x + 160, y + 27, 11, 12);
	
	pDrawPuntero(This + 196, 1, 51522, 0, 0, 0);
	ChangeButtonInfo((char *)(This + 196), x + 64, y + 390, 43, 17);
	if (gInterface.IsWorkZone(eButton5))
		gInterface.DrawButtonRender(eButton5, x + 64, y + 390, 0, 17.0f);
	else
		gInterface.DrawButtonRender(eButton5, x + 64, y + 390, 0, 34.0);
	gInterface.DrawFormat(eWhite, x + 64, y + 392, 43, 3, pGetTextLine(pTextThis(), 227));
}

__declspec(naked) void RenderTrade()
{
	static DWORD Addr_JMP = 0x00864766;
	static DWORD This;

	_asm
	{
		MOV DWORD PTR SS:[EBP-0xC],ECX
		MOV This, ECX

	}

	OpenWindowsTrade(This);

	_asm
	{
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderTextTrade()
{
	static DWORD Addr_JMP = 0x00864988;

	_asm
	{
		MOV EDX,DWORD PTR SS:[EBP-0x94]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		ADD EAX,60
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText1Trade()
{
	static DWORD Addr_JMP = 0x00864A42;

	_asm
	{
		MOV EDX,DWORD PTR SS:[EBP-0x94]
		MOV EAX,DWORD PTR DS:[EDX+0x14]
		ADD EAX,60
		JMP[Addr_JMP]
	}
}

__declspec(naked) void RenderText2Trade()
{
	static DWORD Addr_JMP = 0x00864A7A;

	_asm
	{
		MOV ECX,DWORD PTR SS:[EBP-0x94]
		MOV EDX,DWORD PTR DS:[ECX+0x14]
		ADD EDX,60
		JMP[Addr_JMP]
	}
}


void OpenWindowsCommand(int This)
{
	float x;
	float y;
	y = (double)*(signed int *)(This + 20);
	x = (double)*(signed int *)(This + 16);

	LoadWindows(1, x, y, 190, 429);

	for (int i = 0; i < 11; ++i )
	{
		ChangeButtonInfo((char *)(This + 172 * i + 24), x + (190 / 2) - (110 / 2), y + 55 + (31 * i), 110, 30);
	}
}

__declspec(naked) void RenderWindowsComand()
{
	static DWORD Addr_JMP = 0x0078E7E7;
	static DWORD This;
	//
	_asm
	{
		MOV DWORD PTR SS:[EBP-4],ECX
		MOV EDX, DWORD PTR SS:[EBP-4]
		MOV This,EDX 
	}

	OpenWindowsCommand(This);

	_asm
	{
		JMP[Addr_JMP]
	}
}

void InitSeason15()
{
	SetRange((LPVOID)0x00864B12, 5, ASM::NOP);
	SetRange((LPVOID)0x00864B92, 5, ASM::NOP);
	SetRange((LPVOID)0x008647F8, 5, ASM::NOP);
	SetCompleteHook(0xE9, 0x00864A6E, &RenderText2Trade);
	SetCompleteHook(0xE9, 0x00864A36, &RenderText1Trade);
	SetCompleteHook(0xE9, 0x0086497C, &RenderTextTrade);
	SetCompleteHook(0xE9, 0x00864386, &RenderTrade);

	SetCompleteHook(0xE9, 0x0084D189, &SetTittlePet);
	SetCompleteHook(0xE8, 0x0084D01A, &LoadWindows);
	SetCompleteHook(0xE8, 0x0084D05A, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084D0AC, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084D116, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084D16E, &LoadWindowsNone);

	SetCompleteHook(0xE9, 0x0084A0E9, &SetCloseParty);
	SetCompleteHook(0xE9, 0x0084A67A, &SetTittleParty);
	SetCompleteHook(0xE8, 0x0084A546, &LoadWindows);
	SetCompleteHook(0xE8, 0x0084A57A, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084A5BA, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084A60C, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0084A652, &LoadWindowsNone);

	SetRange((LPVOID)0x0083E6A0, 5, ASM::NOP);
	SetCompleteHook(0xE9, 0x0083ED80, &RenderInfoQuest1);
	SetCompleteHook(0xE9, 0x0083EDDE, &RenderInfoQuest2);
	SetCompleteHook(0xE9, 0x0083F760, &RenderText9Quest);
	SetCompleteHook(0xE9, 0x0083F9E8, &RenderText8Quest);
	SetCompleteHook(0xE9, 0x0083F9B0, &RenderText7Quest);
	SetCompleteHook(0xE9, 0x0083F958, &RenderText6Quest);
	SetCompleteHook(0xE9, 0x0083F8A4, &RenderText5Quest);
	SetCompleteHook(0xE9, 0x0083F86C, &RenderText4Quest);
	SetCompleteHook(0xE9, 0x0083F814, &RenderText3Quest);
	SetCompleteHook(0xE9, 0x0083F728, &RenderText2Quest);
	SetCompleteHook(0xE9, 0x0083F6D0, &RenderText1Quest);
	SetCompleteHook(0xE9, 0x0083F4E7, &RenderQuestTabPane1);
	SetCompleteHook(0xE9, 0x0083F516, &RenderQuestTabPane2);
	SetCompleteHook(0xE9, 0x0083F54C, &RenderQuestTabPane3);
	SetCompleteHook(0xE9, 0x0083F582, &RenderQuestTabPane4);
	SetCompleteHook(0xE8, 0x0083F618, &RenderButtonQuestTb);
	SetCompleteHook(0xE8, 0x0083F690, &RenderButtonQuestTb);
	SetCompleteHook(0xE8, 0x0083F7D4, &RenderButtonQuestTb);
	SetCompleteHook(0xE8, 0x0083F918, &RenderButtonQuestTb);
	SetCompleteHook(0xE9, 0x0083EA16, &RenderWindowsQuest);

	//SetRange((LPVOID)0x007F6753, 5, ASM::NOP);
	//-- Windows Helper
	SetCompleteHook(0xE8, 0x007F65A6, &LoadWindows);
	SetCompleteHook(0xE8, 0x007F65E6, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x007F6638, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x007F66A2, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x007F66FA, &LoadWindowsNone);
	//-- Windows MuHelper2
	SetCompleteHook(0xE8, 0x0080C848, &LoadWindows);
	SetCompleteHook(0xE8, 0x0080C87C, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0080C8BC, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0080C90E, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0080C954, &LoadWindowsNone);
	//SetCompleteHook(0xE9, 0x007F6577, &RenderWindowsMuHelper);

	SetCompleteHook(0xE9, 0x00843B87, &RenderGensPointInfo);
	SetCompleteHook(0xE9, 0x00843D9D, &RenderGensPointText);
	SetCompleteHook(0xE8, 0x00843D4F, &RenderGensPointTexture);
	SetCompleteHook(0xE8, 0x0084371E, &RenderWindowsGensBattle);
	SetCompleteHook(0xE9, 0x00843B09, &RenderTittleQuestGlobal);

	SetCompleteHook(0xE9, 0x00841E26, &RenderWindowsStore);
	SetCompleteHook(0xE9, 0x0078E497, &RenderTittleCommand);
	SetCompleteHook(0xE9, 0x0078E6A6, &RenderWindowsComand);

	SetRange((LPVOID)0x00858016, 5, ASM::NOP);
	SetRange((LPVOID)0x00857898, 5, ASM::NOP);
	SetCompleteHook(0xE9, 0x00857923, &RenderWindowsBaul);
	SetCompleteHook(0xE9, 0x007D1763, &RenderWindowsGuildMaker);
	//-- Windows ChaosMachine
	SetCompleteHook(0xE8, 0x0082CB4B, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0082CB00, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0082CAA5, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0082CA5C, &LoadWindowsNone);
	SetCompleteHook(0xE8, 0x0082CA22, &LoadWindows);
	//--
	SetCompleteHook(0xE9, 0x007DC240, &RenderSlotMove);
	SetCompleteHook(0xE9, 0x007DB575, &RenderSlotGeneral1);
	SetCompleteHook(0xE9, 0x007DB749, &RenderSlotFix);
	//-- WINDOWS INVENTORY
	SetCompleteHook(0xE9, 0x00836EC6, &RenderWindowsInventory);
	SetRange((LPVOID)0x00835116, 5, ASM::NOP);
	SetOp((LPVOID)0x00835116, pDrawZenAndRud, ASM::CALL);
	SetCompleteHook(0xE8, 0x0083511E, &SetTextAncestral);
	SetCompleteHook(0xE8, 0x00835126, &SetTextSocket);
	SetCompleteHook(0xE9, 0x0083BAF4, &RemoveButtonStore1);
	SetCompleteHook(0xE9, 0x0083BB94, &RemoveButtonStore2);
	SetCompleteHook(0xE9, 0x008368FD, &RemoveButton);
	SetCompleteHook(0xE9, 0x00834441, &RenderMoveSlotInventory);
	SetCompleteHook(0xE9, 0x00836514, &RenderSlotEquip);
	//-- WINDOWS SHOP
	SetCompleteHook(0xE9, 0x00847076, &WindowsShop);
	SetRange((LPVOID)0x00847036, 5, ASM::NOP);
	SetRange((LPVOID)0x00847046, 5, ASM::NOP);
	//-- WINDOWS INVENTORY EXT.
	SetCompleteHook(0xE8, 0x007D5576, &RenderWindowsExt);
	SetRange((LPVOID)0x007D4FA6, 5, ASM::NOP);
	SetCompleteHook(0xE9, 0x0083C5CF, &RenderMoveSlotExt);
	SetCompleteHook(0xE8, 0x007D582D, &pDrawRenderImg);
	SetCompleteHook(0xE8, 0x007D587C, &pDrawRenderImg);
	SetCompleteHook(0xE8, 0x007D58CF, &pDrawRenderImg);
	//--
	//Local call from 0083E690
	//SetCompleteHook(0xE9, 0x0083EA10, &RenderWindowsQuest);
	//Local call from 0084371E
	//Local call from 0082C6EE
	//SetCompleteHook(0xE8, 0x0082C6EE, &RenderWindowsChaosMachine);
	//-- IncompletoMuOffHelper
	//SetCompleteHook(0xE9, 0x007F64F0, &RenderWindowsHelper);
	//Local call from 0077F7EE
	//SetCompleteHook(0xE8, 0x0077F7EE, &RenderWindowsCharacter);


	/*//RenderModel
	SetByte((PVOID)(0x007DD511+2), 18);
	SetByte((PVOID)(0x007DD52C+2), 18);
	SetByte((PVOID)(0x007DD547+2), 18);
	SetByte((PVOID)(0x007DD55A+2), 18);
	//-- clic
	SetByte((PVOID)(0x007DC4D0 + 1), 18);
	SetByte((PVOID)(0x007DC4E3+1), 18);

	//-- Slot
	SetByte((LPVOID)(0x007DBB81 + 1),18);
	SetByte((LPVOID)(0x007DBB9A + 1),18);
	SetByte((LPVOID)(0x007DBBB0 + 1),18);
	SetByte((LPVOID)(0x007DBBC9 + 1),18);
	SetByte((LPVOID)(0x007DBBDF + 1),18);
	//-- Slot click
	SetByte((LPVOID)(0x007DBB06 + 2),18);
	SetByte((LPVOID)(0x007DBB23 + 2),18);
	SetByte((LPVOID)(0x007D95A7 + 2),18);
	SetByte((LPVOID)(0x007D958A + 2),18);
	//--
	SetCompleteHook(0xE9, 0x007DC922, &RenderNumberStack);
	SetCompleteHook(0xE9, 0x007DD50A, &RenderSlotItem);
	SetCompleteHook(0xE9, 0x007DC4C9, &RenderSlotClick);
	//-- Render Click
	SetCompleteHook(0xE9, 0x007D95A7, &RenderSlotGeneral8);
	SetCompleteHook(0xE9, 0x007D958A, &RenderSlotGeneral7);
	SetCompleteHook(0xE9, 0x007DBB23, &RenderSlotGeneral6);
	SetCompleteHook(0xE9, 0x007DBB06, &RenderSlotGeneral5);
	//--
	SetCompleteHook(0xE9, 0x007DC133, &RenderSlotGeneral4);
	SetCompleteHook(0xE9, 0x007DBC99, &RenderSlotGeneral3);
	SetCompleteHook(0xE9, 0x007DB47A, &RenderSlotGeneral2);*/
}