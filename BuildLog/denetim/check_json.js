const fs=require('fs');
for (const f of ['Dashboard/data/sohbet.json','Dashboard/data/sonuc.json','Dashboard/data/komut.json','Dashboard/data/oneriler.json']) {
  const b=fs.readFileSync(f);
  const s=b.toString('utf8');
  let ok='OK'; let n=-1;
  try { const j=JSON.parse(s); n = j.entries? j.entries.length : -1; } catch(e){ ok='HATA: '+e.message; }
  console.log(f, 'CR-bayt='+(b.filter(x=>x===13).length), 'LF-bayt='+(b.filter(x=>x===10).length), 'BOM='+(b[0]===0xEF), 'json='+ok, 'entries='+n);
}
