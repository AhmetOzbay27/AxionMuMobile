const fs=require('fs');
const R='C:/Axion Mu Source/';
let fail=0;
function edit(p,fns){let s=fs.readFileSync(R+p,'latin1');const o=s;for(const[find,rep]of fns){if(s.indexOf(find)<0){console.log('  HATA anchor yok: '+p+' :: '+find.slice(0,60));fail++;continue;}s=s.split(find).join(rep);}if(s!==o){fs.writeFileSync(R+p,s,'latin1');console.log('  guncellendi: '+p);}else{console.log('  degismedi: '+p);}}

// 1) DataStore.h: odbc32 baglanti
edit('Source/2.DataServer/DataServer/DataStore.h',[[
'#include <sqlext.h>\n',
'#include <sqlext.h>\n\n#pragma comment(lib,"odbc32.lib")\n'
]]);

// 2) DataServer.cpp: DataStore acilis + sema denetimi + kapanis
edit('Source/2.DataServer/DataServer/DataServer.cpp',[
['#include "SocketManager.h"','#include "SocketManager.h"\n#include "DataStore.h"'],
['\t\t\t\tgGuildManager.Init();',
 '\t\t\t\tgGuildManager.Init();\n\n\t\t\t\t// 14 tablo icin kalici veri katmani (2e9). Semada eksik tablo\n\t\t\t\t// varsa sunucu bozuk semada calismaya devam etmesin.\n\t\t\t\tchar szSchemaReport[1024];\n\t\t\t\tif(gDataStore.Open(DataServerODBC,DataServerUSER,DataServerPASS) == false)\n\t\t\t\t{\n\t\t\t\t\tLogAdd(LOG_RED,"[DataStore] MuOnlineS6 veri katmani acilamadi.");\n\t\t\t\t}\n\t\t\t\telse if(gDataStore.SchemaCheck(szSchemaReport,sizeof(szSchemaReport)) != 0)\n\t\t\t\t{\n\t\t\t\t\tLogAdd(LOG_RED,"[DataStore] SEMA EKSIGI - 14 tablonun bir kismi yok.");\n\t\t\t\t}'],
['\t\t\t\tgQueryManager.Disconnect();',
 '\t\t\t\tgQueryManager.Disconnect();\n\t\t\t\tgDataStore.Close();'],
]);

// 3) CB_AutoNapGame.cpp: DataNapGame.STT yok -> Checking ile degistir
edit('Source/2.DataServer/DataServer/CB_AutoNapGame.cpp',[
['gQueryManager.ExecQuery("Update DataNapGame set Status=\'2\' where Account=\'%s\' and Name=\'%s\'and STT=\'%d\' and Checking=\'%s\'",\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].STT,\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Checking);',
 '// DataNapGame tablosunda STT kolonu YOK (semada yalniz Account/Name/TienNap/Checking/Status).\n\t\t\t\t\t\t// "and STT=%d" kosulu calisma aninda Msg 207 veriyordu; satir artik\n\t\t\t\t\t\t// (Account,Name,Checking) uclusuyle tanimlanir.\n\t\t\t\t\t\tgQueryManager.ExecQuery("Update DataNapGame set Status=\'2\' where Account=\'%s\' and Name=\'%s\' and Checking=\'%s\'",\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\n\t\t\t\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Checking);'],
['gQueryManager.ExecQuery("Update DataNapGame set TienNap=\'%d\',Status=\'1\' where Account=\'%s\' and Name=\'%s\'and STT=\'%d\'",\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].TienNap,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].STT);',
 '// STT kolonu DataNapGame\'da yok -> Checking ile eslesiyor.\n\t\t\tgQueryManager.ExecQuery("Update DataNapGame set TienNap=\'%d\',Status=\'1\' where Account=\'%s\' and Name=\'%s\' and Checking=\'%s\'",\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].TienNap,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\n\t\t\t\tgCBAutoNapGame.mDataGETKQNapTien[n].Checking);'],
['\t\t\t\tinfo.STT = gQueryManager.GetAsInteger("STT");\n',''],
]);

// 4) DataServer.ini -> MuOnlineS6
edit('MuServer/2.DataServer/DataServer.ini',[['DataServerODBC = MuOnline','DataServerODBC = MuOnlineS6']]);
edit('ServerTools/MuServer_S6_2020/DataServer/DataServer.ini',[['DataServerODBC = MuOnline','DataServerODBC = MuOnlineS6']]);

console.log(fail===0?'### YAMALAR TAMAM':`### ${fail} ANCHOR HATASI`);
process.exit(fail===0?0:1);
