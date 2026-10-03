// read_gap_probe.js - tarayicinin READ listesinde OLMAYAN okuma ailesi API'lerinin
// sonucu kullanilmayan cagri sayisini olcer. "OKUMA 0" iddiasinin sinirini burada
// sayisal olarak gosteririz.
const fs=require('fs'),path=require('path');
const REPO=path.resolve(__dirname,'..','..');
const ROOTS=['Source/1.ConnectServer','Source/2.DataServer','Source/3.JoinServer','Source/4.GameServer'];
const EXTRA=['GetPrivateProfileString','GetPrivateProfileStringA','GetPrivateProfileStringW','GetPrivateProfileInt','GetPrivateProfileIntA','GetPrivateProfileIntW','fgetc','fgets','ungetc','gets_s','FindFirstFile','FindFirstFileA','GetFileSize','GetFileSizeEx','localtime','WSARecv','Recv','InternetReadFile','UuidCreateSequential','GetPrivateProfileSectionNames','ReadDirectoryChangesW','FindNextFile'];
function* walk(d){for(const e of fs.readdirSync(d,{withFileTypes:true})){const p=path.join(d,e.name);if(e.isDirectory()){if(!/^(Release|Debug|\.vs|obj|bin)$/i.test(e.name))yield* walk(p);}else if(/\.(cpp|h)$/i.test(e.name))yield p;}}
const rows=[];
for(const r of ROOTS){const root=path.join(REPO,r);if(!fs.existsSync(root))continue;
 for(const f of walk(root)){const L=fs.readFileSync(f,'latin1').split(/\r?\n/);
  for(let i=0;i<L.length;i++){const t=L[i].trim();
   if(/^(if|while|for|switch|return|else if)\b/.test(t))continue;
   if(/^\(void\)/.test(t))continue;
   if(t.includes('=')&&t.indexOf('=')<t.indexOf('('))continue;
   if(/^[\w:]+\s*=/.test(t))continue;
   const m=/^(?:::)?(GetPrivateProfile\w*|fgetc|fgets|ungetc|gets_s|FindFirstFile\w*|GetFileSize\w*|localtime\w*|WSARecv|Recv|InternetReadFile|UuidCreateSequential|ReadDirectoryChangesW|FindNextFile)\s*\(/.exec(t);
   if(m)rows.push({api:m[1],f:path.relative(REPO,f),ln:i+1,t});}}}
const by={};for(const x of rows)(by[x.api]=by[x.api]||[]).push(x);
console.log('### READ listesinde olmayan okuma API cagrilari (sonuc kullanilmayan): '+rows.length);
for(const k of Object.keys(by).sort())console.log(String(by[k].length).padStart(5)+'x  '+k);
console.log('\n--- ornekler (ilk 3) ---');
for(const k of Object.keys(by).sort()){for(const x of by[k].slice(0,3))console.log(k.padEnd(24)+x.f+':'+x.ln+'  '+x.t.slice(0,80));}
