 005EEBC0: 55 push ebp
 005EEBC1: 8B EC mov ebp,esp
 005EEBC3: 83 E4 F0 and esp,0FFFFFFF0h
 005EEBC6: B8 68 20 00 00 mov eax,2068h
 005EEBCB: E8 90 5E 00 00 call __alloca_probe
 005EEBD0: A1 80 10 66 00 mov eax,dword ptr [___security_cookie]
 005EEBD5: 33 C4 xor eax,esp
 005EEBD7: 89 84 24 64 20 00 mov dword ptr [esp+2064h],eax
 00
 005EEBDE: 8B 45 08 mov eax,dword ptr [ebp+8]
 005EEBE1: 32 D2 xor dl,dl
 005EEBE3: 56 push esi
 005EEBE4: 57 push edi
 005EEBE5: 89 44 24 2C mov dword ptr [esp+2Ch],eax
 005EEBE9: B9 05 00 00 00 mov ecx,5
 005EEBEE: 8B 04 85 54 06 11 mov eax,dword ptr [eax*4+2110654h]
 02
 005EEBF5: 33 FF xor edi,edi
 005EEBF7: 89 44 24 28 mov dword ptr [esp+28h],eax
 005EEBFB: BE 10 27 00 00 mov esi,2710h
 005EEC00: 89 4C 24 18 mov dword ptr [esp+18h],ecx
 005EEC04: 88 54 24 17 mov byte ptr [esp+17h],dl
 005EEC08: 89 7C 24 24 mov dword ptr [esp+24h],edi
 005EEC0C: 0F 1F 40 00 nop dword ptr [eax]
 005EEC10: 8B 80 7C 03 00 00 mov eax,dword ptr [eax+37Ch]
 005EEC16: 80 3C 07 01 cmp byte ptr [edi+eax],1
 005EEC1A: 0F 85 D3 01 00 00 jne 005EEDF3
 005EEC20: 8A 4C 07 03 mov cl,byte ptr [edi+eax+3]
 005EEC24: 80 F9 01 cmp cl,1
 005EEC27: 74 09 je 005EEC32
 005EEC29: 80 F9 06 cmp cl,6
 005EEC2C: 0F 85 BD 01 00 00 jne 005EEDEF
 005EEC32: 0F B7 44 07 01 movzx eax,word ptr [edi+eax+1]
 005EEC37: 8B C8 mov ecx,eax
 005EEC39: 66 85 C0 test ax,ax
 005EEC3C: 0F 88 AD 01 00 00 js 005EEDEF
 005EEC42: 66 3B CE cmp cx,si
 005EEC45: 0F 8D A4 01 00 00 jge 005EEDEF
 005EEC4B: 98 cwde
 005EEC4C: 8B 34 85 54 06 11 mov esi,dword ptr [eax*4+2110654h]
 02
 005EEC53: 8B 16 mov edx,dword ptr [esi]
 005EEC55: 8B CA mov ecx,edx
 005EEC57: E8 D4 85 FF FF call ?gObjIsChangeSkin@@YA_NH@Z
 005EEC5C: 84 C0 test al,al
 005EEC5E: 0F 85 82 01 00 00 jne 005EEDE6
 005EEC64: 52 push edx
 005EEC65: E8 96 6C ED FF call ?GetDuelArenaBySpectator@CDuel@@QAEPAUDUEL_ARENA_INFO@@H@Z
 005EEC6A: 85 C0 test eax,eax
 005EEC6C: 0F 85 74 01 00 00 jne 005EEDE6
 005EEC72: F6 86 C0 01 00 00 test byte ptr [esi+1C0h],20h
 20
 005EEC79: 74 21 je 005EEC9C
 005EEC7B: 8B 86 40 06 00 00 mov eax,dword ptr [esi+640h]
 005EEC81: 33 C9 xor ecx,ecx
 005EEC83: 8A 10 mov dl,byte ptr [eax]
 005EEC85: 80 FA FF cmp dl,0FFh
 005EEC88: 74 09 je 005EEC93
 005EEC8A: 80 FA 12 cmp dl,12h
 005EEC8D: 0F 84 53 01 00 00 je 005EEDE6
 005EEC93: 41 inc ecx
 005EEC94: 83 C0 18 add eax,18h
 005EEC97: 83 F9 20 cmp ecx,20h
 005EEC9A: 7C E7 jl 005EEC83
 005EEC9C: 8B 0E mov ecx,dword ptr [esi]
 005EEC9E: 8B C1 mov eax,ecx
 005EECA0: C1 E8 08 shr eax,8
 005EECA3: 83 BE D8 01 00 00 cmp dword ptr [esi+1D8h],1
 01
 005EECAA: 88 44 24 30 mov byte ptr [esp+30h],al
 005EECAE: 88 4C 24 31 mov byte ptr [esp+31h],cl
 005EECB2: 75 0F jne 005EECC3
 005EECB4: 80 BE EC 01 00 00 cmp byte ptr [esi+1ECh],0
 00
 005EECBB: 75 06 jne 005EECC3
 005EECBD: 0C 80 or al,80h
 005EECBF: 88 44 24 30 mov byte ptr [esp+30h],al
 005EECC3: 8A 86 1C 01 00 00 mov al,byte ptr [esi+11Ch]
 005EECC9: 8A 8E DD 01 00 00 mov cl,byte ptr [esi+1DDh]
 005EECCF: FF 74 24 28 push dword ptr [esp+28h]
 005EECD3: 88 44 24 36 mov byte ptr [esp+36h],al
 005EECD7: 8A 86 1E 01 00 00 mov al,byte ptr [esi+11Eh]
 005EECDD: 88 44 24 37 mov byte ptr [esp+37h],al
 005EECE1: 8A 86 4A 03 00 00 mov al,byte ptr [esi+34Ah]
 005EECE7: 32 C8 xor cl,al
 005EECE9: 80 E1 07 and cl,7
 005EECEC: 32 C8 xor cl,al
 005EECEE: 88 8E 4A 03 00 00 mov byte ptr [esi+34Ah],cl
 005EECF4: 0F B7 86 5A 03 00 movzx eax,word ptr [esi+35Ah]
 00
 005EECFB: 0F 10 86 4A 03 00 movups xmm0,xmmword ptr [esi+34Ah]
 00
 005EED02: 66 89 44 24 48 mov word ptr [esp+48h],ax
 005EED07: 0F B7 46 7A movzx eax,word ptr [esi+7Ah]
 005EED0B: 66 89 44 24 52 mov word ptr [esp+52h],ax
 005EED10: 8A 86 3E 01 00 00 mov al,byte ptr [esi+13Eh]
 005EED16: 88 44 24 54 mov byte ptr [esp+54h],al
 005EED1A: 8A 86 40 01 00 00 mov al,byte ptr [esi+140h]
 005EED20: 0F 11 44 24 38 movups xmmword ptr [esp+38h],xmm0
 005EED25: 88 44 24 55 mov byte ptr [esp+55h],al
 005EED29: F3 0F 7E 46 72 movq xmm0,mmword ptr [esi+72h]
 005EED2E: 8A 86 20 01 00 00 mov al,byte ptr [esi+120h]
 005EED34: 56 push esi
 005EED35: 66 0F D6 44 24 4E movq mmword ptr [esp+4Eh],xmm0
 005EED3B: 88 44 24 27 mov byte ptr [esp+27h],al
 005EED3F: E8 CC 15 00 00 call ?CheckCustomEventPkViewport@CViewport@@QAE_NPAUOBJECTSTRUCT@@0@Z
 005EED44: 84 C0 test al,al
 005EED46: 75 08 jne 005EED50
 005EED48: 8A 86 15 01 00 00 mov al,byte ptr [esi+115h]
 005EED4E: EB 02 jmp 005EED52
 005EED50: B0 06 mov al,6
 005EED52: 8A 4C 24 1F mov cl,byte ptr [esp+1Fh]
 005EED56: 8D 94 24 8C 00 00 lea edx,[esp+8Ch]
 00
 005EED5D: 24 0F and al,0Fh
 005EED5F: C0 E1 04 shl cl,4
 005EED62: BF 24 00 00 00 mov edi,24h
 005EED67: C7 44 24 20 00 00 mov dword ptr [esp+20h],0
 00 00
 005EED6F: 0A C8 or cl,al
 005EED71: 8B 86 40 06 00 00 mov eax,dword ptr [esi+640h]
 005EED77: 03 54 24 18 add edx,dword ptr [esp+18h]
 005EED7B: 88 4C 24 52 mov byte ptr [esp+52h],cl
 005EED7F: 8D 77 FC lea esi,[edi-4]
 005EED82: 8A 08 mov cl,byte ptr [eax]
 005EED84: 80 F9 FF cmp cl,0FFh
 005EED87: 74 0F je 005EED98
 005EED89: 88 0A mov byte ptr [edx],cl
 005EED8B: 47 inc edi
 005EED8C: 8B 4C 24 20 mov ecx,dword ptr [esp+20h]
 005EED90: 42 inc edx
 005EED91: 41 inc ecx
 005EED92: 89 4C 24 20 mov dword ptr [esp+20h],ecx
 005EED96: EB 04 jmp 005EED9C
 005EED98: 8B 4C 24 20 mov ecx,dword ptr [esp+20h]
 005EED9C: 83 C0 18 add eax,18h
 005EED9F: 83 EE 01 sub esi,1
 005EEDA2: 75 DE jne 005EED82
 005EEDA4: 0F 10 44 24 30 movups xmm0,xmmword ptr [esp+30h]
 005EEDA9: 8A 54 24 17 mov dl,byte ptr [esp+17h]
 005EEDAD: BE 10 27 00 00 mov esi,2710h
 005EEDB2: 88 4C 24 53 mov byte ptr [esp+53h],cl
 005EEDB6: 8B 4C 24 18 mov ecx,dword ptr [esp+18h]
 005EEDBA: 8B 44 24 50 mov eax,dword ptr [esp+50h]
 005EEDBE: 0F 11 44 0C 68 movups xmmword ptr [esp+ecx+68h],xmm0
 005EEDC3: 0F 10 44 24 40 movups xmm0,xmmword ptr [esp+40h]
 005EEDC8: 0F 11 44 0C 78 movups xmmword ptr [esp+ecx+78h],xmm0
 005EEDCD: 89 84 0C 88 00 00 mov dword ptr [esp+ecx+88h],eax
 00
 005EEDD4: 03 CF add ecx,edi
 005EEDD6: 8B 7C 24 24 mov edi,dword ptr [esp+24h]
 005EEDDA: FE C2 inc dl
 005EEDDC: 89 4C 24 18 mov dword ptr [esp+18h],ecx
 005EEDE0: 88 54 24 17 mov byte ptr [esp+17h],dl
 005EEDE4: EB 0D jmp 005EEDF3
 005EEDE6: 8A 54 24 17 mov dl,byte ptr [esp+17h]
 005EEDEA: BE 10 27 00 00 mov esi,2710h
 005EEDEF: 8B 4C 24 18 mov ecx,dword ptr [esp+18h]
 005EEDF3: 8B 44 24 28 mov eax,dword ptr [esp+28h]
 005EEDF7: 83 C7 04 add edi,4
 005EEDFA: 89 7C 24 24 mov dword ptr [esp+24h],edi
 005EEDFE: 81 FF 2C 01 00 00 cmp edi,12Ch
 005EEE04: 0F 8C 06 FE FF FF jl 005EEC10
 005EEE0A: 84 D2 test dl,dl
 005EEE0C: 74 3B je 005EEE49
 005EEE0E: 88 54 24 6C mov byte ptr [esp+6Ch],dl
 005EEE12: 8B C1 mov eax,ecx
 005EEE14: 8B 54 24 2C mov edx,dword ptr [esp+2Ch]
 005EEE18: C1 E8 08 shr eax,8
 005EEE1B: 88 44 24 69 mov byte ptr [esp+69h],al
