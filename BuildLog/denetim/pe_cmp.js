const fs=require('fs');
const A=fs.readFileSync('BuildLog/getmain_head.exe');
const B=fs.readFileSync('GetMain/GetMainInfo.exe');
function pe(b){const e=b.readUInt32LE(0x3c);const n=b.readUInt16LE(e+6);const os=b.readUInt16LE(e+20);const ts=b.readUInt32LE(e+8);const st=e+24+os;const s=[];for(let i=0;i<n;i++){const o=st+i*40;s.push({nm:b.toString('ascii',o,o+8).replace(/\x00+$/,''),rs:b.readUInt32LE(o+16),pt:b.readUInt32LE(o+20),vs:b.readUInt32LE(o+8),va:b.readUInt32LE(o+12)});}return{e,n,os,ts,st,s,hz:s[0].pt};}
const pa=pe(A),pb=pe(B);
console.log('COFF_TS HEAD='+pa.ts+' ('+new Date(pa.ts*1000).toISOString()+') WT='+pb.ts+' ('+new Date(pb.ts*1000).toISOString()+')');
let hd=0;const hof=[];for(let i=0;i<Math.min(pa.hz,pb.hz);i++)if(A[i]!==B[i]){hd++;if(hof.length<50)hof.push(i);}
console.log('header_diffs='+hd+' at '+JSON.stringify(hof));
const diffs=[];const lim=Math.min(A.length,B.length);
for(let j=0;j<lim;j++)if(A[j]!==B[j])diffs.push(j);
if(A.length!==B.length)console.log('LENGTH DIFF',A.length,B.length);
function secOf(o){for(const s of pa.s){if(o>=s.pt&&o<s.pt+s.rs)return s.nm;}return o<pa.hz?'HEADER':'OUTSIDE';}
const bySec={};for(const o of diffs){const k=secOf(o);bySec[k]=(bySec[k]||0)+1;}
console.log('total_diffs='+diffs.length+' by_section='+JSON.stringify(bySec));
console.log('diff_offsets='+JSON.stringify(diffs));
for(const t of [[A,'HEAD'],[B,'WT']]){const i=t[0].indexOf('RSDS',190000);console.log('RSDS at '+i+' age='+(i>=0?t[0].readUInt32LE(i+20):'-')+' ['+t[1]+']');}
function dx(b,o,n){let s='';for(let i=o;i<o+n;i++)s+=b[i].toString(16).padStart(2,'0')+' ';return s;}
console.log('188257 A: '+dx(A,188257,28));console.log('188257 B: '+dx(B,188257,28));
console.log('190841 A: '+dx(A,190841,40));console.log('190841 B: '+dx(B,190841,40));
