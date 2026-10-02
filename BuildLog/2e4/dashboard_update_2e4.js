// dashboard_update_2e4.js - 2e.4 kaydını Dashboard/data/*.json'a işler
// Kullanım: node BuildLog/2e4/dashboard_update_2e4.js
'use strict';
const fs = require('fs');
const path = require('path');
const D = path.join(__dirname, '..', '..', 'Dashboard', 'data');

const TS = '2026-10-02 22:35';

const sonucEntry = '[22:35] 2e.4 TAMAM — SPK-FIRST VARLIK ÇÖZÜMLEYİCİ + HARİTA/NESNE YOL PARİTESİ: yeni SPKAsset.cpp (SPK_ResolveAssetPath/SPK_AssetExists) canlı düzeni çözüyor (Data\\Local\\X.bmd -> Data\\SPK\\Config\\X.bmd; Data\\Local\\<Lang>\\<Ad>_<Lang>.<ext> -> Config\\<Ad>.<ext>; NpcName, Gate, GlobalText, QuestScript, MonsterSkill, MasterSkillTree/Tooltip vb.); 20 istemci kaynağındaki çağrı noktaları bağlandı (ZzzOpenData, ServerListManager, QuestMng, CreditWin, ItemAddOptioninfo, w_PetProcess, NewUISlideWindow, UIJewelHarmony, w_BuffScriptLoader, CSItemOption). H-011: CSprite::Create boş doku -> log + devam (AV yok). H-014: VIPChar Rank MessageBox -> log + return. H-015: harita yolu "World%d" -> "Map\\World%d" (MapManager/GMBattleCastle/GMCrywolf1st/GM_Kanturu_3rd); LoadData AccessModel "Data\\Object*" -> "Data\\Map\\Object*"; ZzzTexture LoadBitmap aynı; Minimap önce Data\\SPK\\Minimap\\Minimap_<Ad>_<Lang>.bmd, yoksa eski yol. Derleme: Main.exe md5 b386bd65c4fbdc1aec840adfcfb8af8a (12.029.952 B), 0 hata (yalnız LNK4099/MSB8004). Sahada run14: diyalog yok, çökme yok, kalıntı süreç yok; pencere t=0,9 s (Axion Mu), t=5,2 s 45.87.120.29:44405 SYN_SENT; KEN.txt yeni yol kanıtı (LoadBitmap Data\\Map\\Object8\\drop01.jpg). Rapor docs/24 (run11-14 tablosu); docs/02/03(B-07,B-08)/04/22 + CHANGELOG güncellendi. Kalan: içerik/şema eşlemesi (B-08: itemtooltip*, World74 minimap yok), tam Data kopyası (disk), SPK_CRCFILE SPK_MEXE, E2E login (Faz 3.1).';

const sonuc = JSON.parse(fs.readFileSync(path.join(D, 'sonuc.json'), 'utf8'));
sonuc.updated = TS + ':00';
sonuc.status = 'ok';
sonuc.text = sonucEntry + ' || ' + (sonuc.text || '');
fs.writeFileSync(path.join(D, 'sonuc.json'), JSON.stringify(sonuc, null, 2), 'utf8');
console.log('sonuc.json guncellendi; text uzunlugu', sonuc.text.length);

const ch = JSON.parse(fs.readFileSync(path.join(D, 'sohbet.json'), 'utf8'));
const arr = ch.entries;
if (!Array.isArray(arr)) { console.error('entries dizisi yok!'); process.exit(1); }

// Sira duzeltmesi: onceki turda sona push edilmis "asistan" kayitlarini basa tasi
// (panel j.sohbet.slice(0,15) ile dizi basindan okur) + kim'i panele uygun "ajan" yap.
const misplaced = arr.filter(e => e.kim === 'asistan').sort((a, b) => b.ts.localeCompare(a.ts));
const rest = arr.filter(e => e.kim !== 'asistan');
misplaced.forEach(e => { e.kim = 'ajan'; });

const yeni = [
  {
    ts: TS,
    kim: 'kullanıcı',
    text: 'Geri kalan adımlarıda uygulamaya başla ve change log güncelleyip gereksiz dosya temizliği yap.'
  },
  {
    ts: TS,
    kim: 'ajan',
    text: '2e.4 TAMAM: canlı SPK-first varlık çözümleyici (SPKAsset.cpp: Local/Config eşlemesi) 20 çağrı noktasına ve harita/nesne yol paritesine (World%d -> Map\\World%d; Object -> Map\\Object; Minimap SPK\\Minimap öncelik) uygulandı; H-011/H-014/H-015 kapandı. Derleme: b386bd65c4fbdc1aec840adfcfb8af8a (12.029.952 B), 0 hata. Sahada run14: diyalogsuz/çökmesiz, 45.87.120.29:44405 SYN_SENT; kanıt BuildLog/2e4/{results,shots,evidence} + docs/24. Bu turda ayrıca gereksiz BuildLog kopyaları (2e2/deploy 324 MB, 5Main obj 161 MB, 2e3/deploy 25 MB) silindi. Kalan: içerik/şema eşlemesi (B-08), tam Data kopyası, SPK_CRCFILE, E2E login. Commit: 2e.4 (hash takibi sonraki commit\'te).'
  }
];

ch.updated = TS + ':00';
ch.entries = yeni.concat(misplaced, rest);
fs.writeFileSync(path.join(D, 'sohbet.json'), JSON.stringify(ch, null, 2), 'utf8');
console.log('sohbet.json guncellendi; kayit sayisi', ch.entries.length, '| basa tasinan:', misplaced.map(e => e.ts).join(', '));
