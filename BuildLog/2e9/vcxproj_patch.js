const fs=require('fs');
const R='C:/Axion Mu Source/';
let fail=0;
function ins(p,anchor,line){let s=fs.readFileSync(R+p,'utf8');if(s.indexOf(line)>=0){console.log('  zaten var: '+p);return;}
 if(s.indexOf(anchor)<0){console.log('  ANCHOR YOK: '+p);fail++;return;}
 s=s.replace(anchor,line+'\r\n'+anchor);
 fs.writeFileSync(R+p,s,'utf8');console.log('  eklendi: '+line.trim());}

const V='Source/2.DataServer/DataServer/DataServer.vcxproj';
ins(V,'    <ClInclude Include="QueryManager.h" />','    <ClInclude Include="DataStore.h" />');
ins(V,'    <ClCompile Include="QueryManager.cpp" />','    <ClCompile Include="DataStore.cpp" />');

// filters
const F='Source/2.DataServer/DataServer/DataServer.vcxproj.filters';
if(fs.existsSync(R+F)){
  let s=fs.readFileSync(R+F,'utf8');
  if(s.indexOf('DataStore.cpp')>=0){console.log('  filters zaten var');}
  else{
    const m=s.match(/    <ClInclude Include="QueryManager\.h" >[\s\S]*?<\/ClInclude>/);
    if(!m){console.log('  filters: QueryManager.h bulunamadi');fail++;}
    else{
      s=s.replace(m[0],'    <ClInclude Include="DataStore.h">\r\n      <Filter>Header Files</Filter>\r\n    </ClInclude>\r\n'+m[0]);
      const m2=s.match(/    <ClCompile Include="QueryManager\.cpp" >[\s\S]*?<\/ClCompile>/);
      if(m2){
        s=s.replace(m2[0],'    <ClCompile Include="DataStore.cpp">\r\n      <Filter>Source Files</Filter>\r\n    </ClCompile>\r\n'+m2[0]);
        console.log('  filters guncellendi');
      } else {console.log('  filters: QueryManager.cpp bulunamadi');fail++;}
      fs.writeFileSync(R+F,s,'utf8');
    }
  }
} else console.log('  filters dosyasi yok (atlandi)');

console.log(fail===0?'### PROJE KAYDI TAMAM':`### ${fail} HATA`);
process.exit(fail===0?0:1);
