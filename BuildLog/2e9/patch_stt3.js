const fs=require('fs');
const P='C:/Axion Mu Source/Source/2.DataServer/DataServer/CB_AutoNapGame.cpp';
let s=fs.readFileSync(P,'latin1');
let fail=0;

// (1) STT kolonunu okuyan satir - yalniz DataNapGame SELECT'inde (satir 367).
// Satir 348 CardPhone SELECT'idir ve kucuk 'stt' kullanir; buraya dokunulmaz.
const oldRead='info.STT = gQueryManager.GetAsInteger("STT");';
if(s.split(oldRead).length-1!==1){console.log('  ANCHOR SAYISI != 1: '+oldRead);fail++;}
else{
  s=s.replace(oldRead,
    '// STT yalniz CardPhone tablosunda vardir; DataNapGame semasinda YOK.\r\n'+
    '\t\t\t\t// Eski satir calisma aninda Msg 207 (Invalid column name) uretiyordu.');
  console.log('  ok: STT okuma satiri');
}

// (2) DataNapGame Status=2 UPDATE - tam ifadeyi girinti koruyarak yeniden yaz
const reStatus=/(\t*)gQueryManager\.ExecQuery\("Update DataNapGame set Status='2'[\s\S]*?mDataGETKQNapTien\[n\]\.Checking\);/;
if(!reStatus.test(s)){console.log('  ANCHOR YOK: Status=2 UPDATE');fail++;}
else{
  s=s.replace(reStatus,(m,ind)=>
    ind+'// DataNapGame semasinda STT kolonu YOK; "and STT=%d" kosulu Msg 207 veriyordu.\r\n'+
    ind+'// Satir artik (Account,Name,Checking) uclusuyle tanimlanir.\r\n'+
    ind+'gQueryManager.ExecQuery("Update DataNapGame set Status=\'2\' where Account=\'%s\' and Name=\'%s\' and Checking=\'%s\'",\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Checking);');
  console.log('  ok: Status=2 UPDATE');
}

// (3) DataNapGame TienNap UPDATE
const reNap=/(\t*)gQueryManager\.ExecQuery\("Update DataNapGame set TienNap='%d',Status='1'[\s\S]*?mDataGETKQNapTien\[n\]\.STT\);/;
if(!reNap.test(s)){console.log('  ANCHOR YOK: TienNap UPDATE');fail++;}
else{
  s=s.replace(reNap,(m,ind)=>
    ind+'// DataNapGame semasinda STT kolonu YOK -> Checking ile eslesiyor.\r\n'+
    ind+'gQueryManager.ExecQuery("Update DataNapGame set TienNap=\'%d\',Status=\'1\' where Account=\'%s\' and Name=\'%s\' and Checking=\'%s\'",\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].TienNap,\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Account,\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Name,\r\n'+
    ind+'\tgCBAutoNapGame.mDataGETKQNapTien[n].Checking);');
  console.log('  ok: TienNap UPDATE');
}

if(fail===0){fs.writeFileSync(P,s,'latin1');console.log('### STT YAMASI UYGULANDI');}
else console.log('### '+fail+' ANCHOR HATASI - dosya YAZILMADI');
process.exit(fail===0?0:1);
