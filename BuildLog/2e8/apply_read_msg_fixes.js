// apply_read_msg_fixes.js - Sunucu kaynaklarında yok sayılan dönüş değerleri
// (okuma + mesaj). Byte-exact latin1, CRLF-duyarlı, idempotent.
const fs = require('fs');
const path = require('path');
const R = path.resolve(__dirname, '..', '..');
let fail = 0;

function readBuf(p) { return fs.readFileSync(p, 'latin1'); }
function writeBuf(p, s) { fs.writeFileSync(p, Buffer.from(s, 'latin1')); }

function sub(text, oldStr, newStr, tag, expect = 1) {
  // idempotence kontrolu: yeni metnin IKI satir-sonu varyantina da bak
  if (text.indexOf(newStr) >= 0 || text.indexOf(newStr.replace(/\n/g, '\r\n')) >= 0) {
    console.log(`ATLANDI [${tag}]`);
    return text;
  }
  const variants = /\r\n/.test(text)
    ? [[oldStr.replace(/\n/g, '\r\n'), newStr.replace(/\n/g, '\r\n')], [oldStr, newStr]]
    : [[oldStr, newStr], [oldStr.replace(/\n/g, '\r\n'), newStr.replace(/\n/g, '\r\n')]];
  for (const [o, n] of variants) {
    const c = text.split(o).length - 1;
    if (c === expect) { console.log(`UYGULANDI [${tag}]`); return text.split(o).join(n); }
    if (c > expect) { console.log(`HATA [${tag}]: ${c} kez eslesti (${expect} bekleniyordu)`); fail++; return text; }
  }
  console.log(`HATA [${tag}]: anchor bulunamadi`);
  fail++;
  return text;
}
const P = (rel) => path.join(R, rel);

// -------------------------------------------- 1) PacketManager.h : beyan
{
  const p = P('Source/4.GameServer/GameServer/PacketManager.h');
  let s = readBuf(p);
  s = sub(s,
`private:
	#if(GAMESERVER_UPDATE>=701)`,
`private:
	bool ReadExact(HANDLE file,void* lpBuffer,DWORD dwSize,DWORD* lpRead,char* name);

	#if(GAMESERVER_UPDATE>=701)`, 'PacketManager.h ReadExact beyani');
  writeBuf(p, s);
}

// -------------------------------------------- 2) PacketManager.cpp : yardimci + kontroller
{
  const p = P('Source/4.GameServer/GameServer/PacketManager.cpp');
  let s = readBuf(p);

  // yardimciyi LoadKey'den hemen once koy
  s = sub(s,
`bool CPacketManager::LoadKey(char* name,WORD header,bool type) // OK
{`,
`bool CPacketManager::ReadExact(HANDLE file,void* lpBuffer,DWORD dwSize,DWORD* lpRead,char* name)
{
	// Donus degerinin yaninda OKUNAN BAYT SAYISI da denetlenir: kirpik
	// dosyada ReadFile TRUE donebilir ama istenen baytlarin tamami
	// okunmamis olur; bu durumda asagidaki tablolar yarim birakilir ve
	// paket sifreleme sessizce bozulur.
	if(ReadFile(file,lpBuffer,dwSize,lpRead,0) == 0)
	{
		gLog.Output(LOG_GENERAL,"[PacketManager] ReadFile basarisiz [%s] GetLastError=%d istenen=%lu",name,GetLastError(),(unsigned long)dwSize);
		return false;
	}

	if(lpRead != NULL && *lpRead != dwSize)
	{
		gLog.Output(LOG_GENERAL,"[PacketManager] Eksik okuma [%s] okunan=%lu istenen=%lu",name,(unsigned long)*lpRead,(unsigned long)dwSize);
		return false;
	}

	return true;
}

bool CPacketManager::LoadKey(char* name,WORD header,bool type) // OK
{`, 'PacketManager.cpp ReadExact tanimi');

  // 1) header okumasi
  s = sub(s,
`\tDWORD size;

\tReadFile(file,&HeaderInfo,sizeof(HeaderInfo),&size,0);
`,
`\tDWORD size;

\tif(this->ReadExact(file,&HeaderInfo,sizeof(HeaderInfo),&size,name) == false)
\t{
\t\tCloseHandle(file);
\t\treturn 0;
\t}
`, 'LoadKey HeaderInfo okumasi');

  // 2) Modulus
  s = sub(s,
`\tDWORD table[4];

\tReadFile(file,table,sizeof(table),&size,0);

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Modulus[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`,
`\tDWORD table[4];

\tif(this->ReadExact(file,table,sizeof(table),&size,name) == false)
\t{
\t\tCloseHandle(file);
\t\treturn 0;
\t}

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Modulus[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`, 'LoadKey Modulus okumasi');

  // 3) Key
  s = sub(s,
`\tReadFile(file,table,sizeof(table),&size,0);

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Key[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`,
`\tif(this->ReadExact(file,table,sizeof(table),&size,name) == false)
\t{
\t\tCloseHandle(file);
\t\treturn 0;
\t}

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Key[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`, 'LoadKey Key okumasi');

  // 4) Xor
  s = sub(s,
`\tReadFile(file,table,sizeof(table),&size,0);

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Xor[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`,
`\tif(this->ReadExact(file,table,sizeof(table),&size,name) == false)
\t{
\t\tCloseHandle(file);
\t\treturn 0;
\t}

\tfor(int n=0;n < 4;n++)
\t{
\t\tlpData->Xor[n] = this->m_SaveLoadXor[n]^table[n];
\t}
`, 'LoadKey Xor okumasi');

  writeBuf(p, s);
}

// -------------------------------------------- 3) GameServer.cpp : combo box + status bar + TranslateMessage
{
  const p = P('Source/4.GameServer/GameServer/GameServer.cpp');
  let s = readBuf(p);

  // GERCEK HATA: LB_ADDSTRING LB_ERR donerse LB_SETITEMDATA -1 ile cagriliyor
  s = sub(s,
`\t\t\t\t\t\tint pos = SendMessage(hWndComboBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)fulltext));
\t\t\t\t\t\tSendMessage(hWndComboBox, LB_SETITEMDATA, pos, (LPARAM) gObj[n].Account);`,
`\t\t\t\t\t\tint pos = (int)SendMessage(hWndComboBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)fulltext));

\t\t\t\t\t\tif(pos == LB_ERR || pos < 0)
\t\t\t\t\t\t{
\t\t\t\t\t\t\t// LB_ADDSTRING basarisiz oldu; LB_SETITEMDATA -1 ile
\t\t\t\t\t\t\t// cagrilirsa sessizce yanlis yere veri yazilir.
\t\t\t\t\t\t\tcontinue;
\t\t\t\t\t\t}

\t\t\t\t\t\tif(SendMessage(hWndComboBox, LB_SETITEMDATA, (WPARAM)pos, (LPARAM)gObj[n].Account) == LB_ERR)
\t\t\t\t\t\t{
\t\t\t\t\t\t\t// Oge eklendi ama hesap bilgisi baglanamadi -> ogeyi geri al.
\t\t\t\t\t\t\tSendMessage(hWndComboBox, LB_DELETESTRING, (WPARAM)pos, 0);
\t\t\t\t\t\t}`, 'GameServer.cpp combo box');

  // SB_SETPARTS: basarida 0 doner, sifir disi = hata
  s = sub(s,
`            SendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths);`,
`            // SB_SETPARTS basarisinda 0 doner; sifir disi deger hata anlamina gelir.
            if(SendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths) != 0)
            {
                gLog.Output(LOG_GENERAL,"[GameServer] SB_SETPARTS basarisiz (hWnd=%d)",(int)hWndStatusBar);
            }`, 'GameServer.cpp SB_SETPARTS');

  // SB_SETTEXT: donus degeri ONCEKI metnin uzunlugu (0 = hata degil, metin yok demek).
  // Hata sinyali olmadigi icin bilerek yok sayiliyor; bunu (void) ile acikca belirtiyoruz.
  for (let i = 0; i <= 5; i++) {
    const arg = i === 5 ? 'NULL' : 'text';
    // 0..4 boslukla girintili, 5 ise tab ile girintili
    const oldS = i === 5
      ? `\t\t\tSendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`
      : `            SendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`;
    const newS = i === 5
      ? `\t\t\t// SB_SETTEXT'in donusu onceki metnin uzunlugudur; hata sinyali tasimaz.\n\t\t\t(void)SendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`
      : `            // SB_SETTEXT'in donusu onceki metnin uzunlugudur; hata sinyali tasimaz.\n            (void)SendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`;
    s = sub(s, oldS, newS, `GameServer.cpp SB_SETTEXT ${i}`);
  }

  s = sub(s,
`\t\t\tTranslateMessage(&msg);`,
`\t\t\t// Donus degeri yalnizca karakter mesajlari icin TRUE'dur; hata sinyali
\t\t\t// tasimaz, bu yuzden bilerek yok sayiliyor.
\t\t\t(void)TranslateMessage(&msg);`, 'GameServer.cpp TranslateMessage');

  writeBuf(p, s);
}

// -------------------------------------------- 4) ServerDisplayer.cpp : status bar
{
  const p = P('Source/4.GameServer/GameServer/ServerDisplayer.cpp');
  let s = readBuf(p);

  s = sub(s,
`\tSendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths);`,
`\t// SB_SETPARTS basarisinda 0 doner; sifir disi deger hata anlamina gelir.
\tif(SendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths) != 0)
\t{
\t\tgLog.Output(LOG_GENERAL,"[ServerDisplayer] SB_SETPARTS basarisiz (hWnd=%d)",(int)hWndStatusBar);
\t}`, 'ServerDisplayer SB_SETPARTS');

  for (let i = 0; i <= 5; i++) {
    const arg = i === 5 ? 'NULL' : 'text';
    s = sub(s,
      `\tSendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`,
      `\t// SB_SETTEXT'in donusu onceki metnin uzunlugudur; hata sinyali tasimaz.
\t(void)SendMessage(hWndStatusBar, SB_SETTEXT, ${i},(LPARAM)${arg});`,
      `ServerDisplayer SB_SETTEXT ${i}`);
  }

  writeBuf(p, s);
}

// -------------------------------------------- 5) Diger sunucular: TranslateMessage
for (const f of ['Source/1.ConnectServer/ConnectServer/ConnectServer.cpp',
  'Source/2.DataServer/DataServer/DataServer.cpp',
  'Source/3.JoinServer/JoinServer/JoinServer.cpp']) {
  const p = P(f);
  let s = readBuf(p);
  s = sub(s,
`\t\t\tTranslateMessage(&msg);`,
`\t\t\t// Donus degeri yalnizca karakter mesajlari icin TRUE'dur; hata sinyali
\t\t\t// tasimaz, bu yuzden bilerek yok sayiliyor.
\t\t\t(void)TranslateMessage(&msg);`, `${path.basename(f)} TranslateMessage`);
  writeBuf(p, s);
}

console.log(fail === 0 ? '### DUZELTME TAMAM' : `### ${fail} HATA`);
process.exit(fail === 0 ? 0 : 1);