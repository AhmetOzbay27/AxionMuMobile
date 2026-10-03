// apply_open_data_patch.js — ZzzOpenData.cpp'yi HEAD baytlariyla yeniden kurar ve
// B-08 (SPK ToolTipText.txt) yamasini yalnizca ASCII blok uzerinde uygular.
// Amac: edit aracinin EUC-KR baytlarini U+FFFD'ye cevirmesini tamamen onlemek.
'use strict';
const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const rel = 'Source/5.Main/source/ZzzOpenData.cpp';
const root = process.cwd();
const head = execSync(`git show HEAD:${rel}`, { cwd: root, maxBuffer: 64 * 1024 * 1024 }).toString('latin1');
const headFile = 'BuildLog/2e4/ZzzOpenData.head.cpp';
fs.writeFileSync(headFile, Buffer.from(head, 'latin1'));
console.log('HEAD surumu yazildi: ' + headFile + ' (' + Buffer.byteLength(head, 'latin1') + ' bayt)');

let s = head;

const startMarker = 'sprintf(Text, "Data\\\\Local\\\\%s\\\\itemtooltiptext_%s.bmd", g_strSelectedML.c_str(), g_strSelectedML.c_str());';
const endMarker = 'set_item_tooltip();';

const startIdx = s.indexOf(startMarker);
const endIdx = s.indexOf(endMarker, startIdx);
if (startIdx < 0 || endIdx < 0) { console.error('HEDEF BLOK BULUNAMADI'); process.exit(1); }

const nl = s.slice(startIdx, endIdx).includes('\r\n') ? '\r\n' : '\n';
console.log('blok satir sonu: ' + (nl === '\r\n' ? 'CRLF' : 'LF'));

// ucuncu blogun (itemtooltiptext) son satirindan sonra ekleme yap
const marker = 'g_ErrorReport.Write(szSkip);';
const mIdx = s.lastIndexOf(marker, endIdx);
if (mIdx < 0) { console.error('ARKA UC BULUNAMADI'); process.exit(1); }
const afterMarker = mIdx + marker.length;
const eolIdx = s.indexOf('\n', afterMarker);
if (eolIdx < 0) { console.error('SATIR SONU BULUNAMADI'); process.exit(1); }
const insertAt = eolIdx + 1;

const added = [
  '',
  '\t\t\t// 2e.5 (B-08): SPK paketi ayni tabloyu duz metin olarak tasir',
  '\t\t\t// (Data\\SPK\\Config\\ToolTipText.txt). Resolver uzerinden cozulur.',
  '\t\t\tsprintf(Text, "Data\\\\Local\\\\%s\\\\ToolTipText_%s.txt", g_strSelectedML.c_str(), g_strSelectedML.c_str());',
  '\t\t\tconst char* pTooltipText = SPK_ResolveAssetPath(Text);',
  '\t\t\tif (SPK_AssetExists(pTooltipText))',
  '\t\t\t\tload_item_tooltip_text_spk(pTooltipText);'
].join(nl) + nl;

s = s.slice(0, insertAt) + added + s.slice(insertAt);

fs.writeFileSync(rel, Buffer.from(s, 'latin1'));
console.log('yama uygulandi: ' + rel);
