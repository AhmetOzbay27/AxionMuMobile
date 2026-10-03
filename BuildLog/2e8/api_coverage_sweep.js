// api_coverage_sweep.js - "TUM" iddiasini denetler: 4 agacta satir basinda
// cagri (ama sonucu kullanilmayan) tum API adlarini toplar; tarayicinin
// READ/MSG/IO listelerinde OLMAYANlari listeler. Bunlar ya baska bir
// sinifa ait ya da hic kapsanmiyor demektir.
const fs = require('fs'); const path = require('path');
const REPO = path.resolve(__dirname, '..', '..');
const ROOTS = ['Source/1.ConnectServer','Source/2.DataServer','Source/3.JoinServer','Source/4.GameServer'];
const KNOWN = new Set(['ReadFile','ReadFileEx','ReadProcessMemory','ReadConsole','fread','_read','_read_s','recv','recvfrom','send','sendto','GetQueuedCompletionStatus','PeekNamedPipe','GetOverlappedResult','SendMessage','SendMessageA','SendMessageW','SendMessageTimeout','PostMessage','PostThreadMessage','SendNotifyMessage','SendDlgItemMessage','SendDlgItemMessageA','SendDlgItemMessageW','FindWindow','FindWindowA','FindWindowW','FindWindowEx','FindWindowExA','FindWindowExW','GetMessage','PeekMessage','TranslateMessage','DispatchMessage','SendMessageCallback','WriteFile','WriteFileEx','WriteConsole','SetFilePointer','SetFilePointerEx','CreateFile','DeleteFile','RemoveDirectory','CreateDirectory','MoveFile','CopyFile','DeviceIoControl','CreateFileMapping','MapViewOfFile','UnmapViewOfFile','FlushFileBuffers','fwrite','fseek','ftell','fclose','_commit','VirtualAlloc','VirtualFree','socket','bind','listen','accept','connect','closesocket','select','setsockopt','ioctlsocket','WSAStartup','WSACleanup','CreateThread','WaitForSingleObject','WaitForMultipleObjects']);
function* walk(d){for(const e of fs.readdirSync(d,{withFileTypes:true})){const p=path.join(d,e.name);if(e.isDirectory()){if(!/^(Release|Debug|\.vs|obj|bin)$/i.test(e.name))yield* walk(p);}else if(/\.(cpp|h)$/i.test(e.name))yield p;}}
const hits = {};
for (const r of ROOTS){ const root=path.join(REPO,r); if(!fs.existsSync(root))continue;
  for(const f of walk(root)){ const L=fs.readFileSync(f,'latin1').split(/\r?\n/);
    for(let i=0;i<L.length;i++){ const t=L[i].trim();
      if(/^(if|while|for|switch|return|else if)\b/.test(t))continue;
      if(/^\(void\)/.test(t))continue;
      if(t.includes('=')&&t.indexOf('=')<t.indexOf('('))continue;
      if(/^[\w:]+\s*=/.test(t))continue;
      const m=/^(?:::)?([A-Za-z_]\w*)\s*\(/.exec(t); if(!m)continue;
      const a=m[1]; if(KNOWN.has(a))continue;
      if(a==='GetMessage')continue;
      (hits[a]=hits[a]||[]).push(path.relative(REPO,f)+':'+(i+1)); } } }
const keys=Object.keys(hits).sort();
console.log('### Listelerde OLMAYAN, sonucu kullanilmayan cagri: '+keys.length+' farkli API');
for(const k of keys){const v=hits[k];console.log(String(v.length).padStart(4)+'x  '+k.padEnd(26)+'ilk: '+v[0]);}
