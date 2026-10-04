const fs = require('fs');
const A = fs.readFileSync(process.argv[2]);
const B = fs.readFileSync(process.argv[3]);
function pe(b){const e=b.readUInt32LE(0x3c);const n=b.readUInt16LE(e+6);const os=b.readUInt16LE(e+20);const ts=b.readUInt32LE(e+8);const st=e+24+os;const s=[];for(let i=0;i<n;i++){const o=st+i*40;s.push({nm:b.toString('ascii',o,o+8).replace(/\x00+$/,''),rs:b.readUInt32LE(o+16),pt:b.readUInt32LE(o+20)});}return{e,n,os,ts,s,hz:s[0].pt};}
const pa=pe(A),pb=pe(B);
console.log('len A='+A.length+' B='+B.length);
console.log('COFF_TS A='+pa.ts+' B='+pb.ts+' ('+(pb.ts-pa.ts)+' sn)');
let hd=0;const hof=[];for(let i=0;i<Math.min(pa.hz,pb.hz);i++)if(A[i]!==B[i]){hd++;if(hof.length<20)hof.push(i);}
console.log('header_diffs='+hd+' '+JSON.stringify(hof));
const lim=Math.min(A.length,B.length);const diffs=[];
for(let j=0;j<lim;j++)if(A[j]!==B[j])diffs.push(j);
function secOf(o){for(const s of pa.s){if(o>=s.pt&&o<s.pt+s.rs)return s.nm;}return o<pa.hz?'HEADER':'OUTSIDE';}
const by={};for(const o of diffs){const k=secOf(o);by[k]=(by[k]||0)+1;}
console.log('total_diffs='+diffs.length+' by_section='+JSON.stringify(by));
console.log('ilk 30 diff ofset='+JSON.stringify(diffs.slice(0,30)));
