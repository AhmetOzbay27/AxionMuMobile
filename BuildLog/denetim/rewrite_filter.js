#!/usr/bin/env node
// rewrite_filter.js - `git fast-export --no-data` akisindan verilen yol oneklerini
// (dizin/agac) TUM tarihten cikarir; cikti `git fast-import` icin uygundur (docs/43).
// Kullanim:
//   git fast-export --no-data <refs...> | node BuildLog/denetim/rewrite_filter.js \
//       Source/5.Main/boost_1_80_0 ClientBuild_192.168.99.200 | git fast-import --force
// Notlar:
//  - Yalnizca "M <mode> <ref> <path>" ve "D <path>" satirlari filtrelenir; diger
//    satirlar (commit/reset/from/mark/tag/data bloklari) BIREBIR kopyalanir.
//  - "data <n>" bloklari bayt bazinda (n bayt) dokunulmadan gecirilir.
//  - Yollarin basindaki .git C-tirnaklama bicimi (orn. "\303\237") cozulur; cozulemezse
//    tirnakli ham metin uzerinden onek karsilastirmasi yapilir.
'use strict';

const prefixes = process.argv.slice(2).map((p) => p.replace(/\/+$/, '').replace(/\\/g, '/'));
if (prefixes.length === 0) {
  console.error('HATA: en az bir yol oneki gerekli (or. Source/5.Main/boost_1_80_0)');
  process.exit(2);
}

function unquoteGit(s) {
  // s: '"...."' (git C-tirnaklama) -> utf8 metin
  const inner = s.slice(1, -1);
  const bytes = [];
  for (let k = 0; k < inner.length; k++) {
    const ch = inner[k];
    if (ch === '\\') {
      const nx = inner[++k];
      if (nx >= '0' && nx <= '7') {
        let oct = nx;
        while (oct.length < 3 && inner[k + 1] >= '0' && inner[k + 1] <= '7') oct += inner[++k];
        bytes.push(parseInt(oct, 8));
      } else if (nx === 'n') bytes.push(10);
      else if (nx === 't') bytes.push(9);
      else if (nx === 'r') bytes.push(13);
      else if (nx === 'b') bytes.push(8);
      else if (nx === 'f') bytes.push(12);
      else if (nx === 'v') bytes.push(11);
      else bytes.push(nx.charCodeAt(0));
    } else {
      bytes.push(ch.charCodeAt(0));
    }
  }
  return Buffer.from(bytes).toString('utf8');
}

function isRemoved(p) {
  const clean = p.startsWith('"') ? p.slice(1).replace(/"$/, '') : p;
  return prefixes.some((x) => clean === x || clean.startsWith(x + '/'));
}

function pathOf(line, kind) {
  if (kind === 'M') {
    const m = /^M (\d+) (\S+) (.*)$/.exec(line);
    return m ? m[3] : null;
  }
  return line.slice(2);
}

const chunks = [];
process.stdin.on('data', (c) => chunks.push(c));
process.stdin.on('end', () => {
  const buf = Buffer.concat(chunks);
  const out = [];
  const n = buf.length;
  let i = 0;
  let dropped = 0;
  let keptM = 0;
  let dataBlocks = 0;

  while (i < n) {
    let nl = buf.indexOf(10, i);
    if (nl === -1) nl = n;
    const line = buf.subarray(i, nl).toString('utf8');
    const afterLine = nl < n ? nl + 1 : nl;

    const dm = /^data (\d+)$/.exec(line);
    if (dm) {
      dataBlocks++;
      const len = parseInt(dm[1], 10);
      const end = Math.min(afterLine + len, n);
      out.push(buf.subarray(i, end));
      i = end;
      continue;
    }

    if ((line.startsWith('M ') || line.startsWith('D ')) && line.length > 2) {
      const raw = pathOf(line, line[0]);
      if (raw !== null && isRemoved(raw)) {
        dropped++;
        i = afterLine;
        continue;
      }
      if (line.startsWith('M ')) keptM++;
    }

    out.push(buf.subarray(i, afterLine));
    i = afterLine;
  }

  process.stdout.write(Buffer.concat(out), () => {
    console.error(
      'rewrite_filter: atilan_satir=' + dropped + ' korunan_M=' + keptM +
      ' data_blok=' + dataBlocks + ' girdi_bayt=' + n + ' cikti_bayt=' + out.reduce((a, b) => a + b.length, 0)
    );
  });
});
