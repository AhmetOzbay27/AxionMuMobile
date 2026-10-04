const fs = require('fs');
const f = 'BuildLog/denetim/build_all.sh';
let s = fs.readFileSync(f, 'latin1');
function rep(a, b, n, label) { const c = s.split(a).length - 1; if (c !== n) throw new Error(label + ' capa=' + c); s = s.split(a).join(b); }
rep('# Kullanim:  bash BuildLog/denetim/build_all.sh [--no-client] [--help]',
    '# Kullanim:  bash BuildLog/denetim/build_all.sh [--no-client] [--logdir DIR] [--help]',
    1, 'b5-usage');
rep('#            BuildLog/denetim/build_all_summary.txt    (ozet + dogrulama)\n',
    '#            BuildLog/denetim/build_all_summary.txt    (ozet + dogrulama)\n' +
    '#            --logdir DIR verilirse bu uc dosya DIR altina yazilir (varsayilan:\n' +
    '#            BuildLog/denetim); pre-push kancasi boyle cagirir (docs/39).\n',
    1, 'b5-logdir-notu');
const oldArg = 'NOCLIENT=0\n' +
'for a in "$@"; do\n' +
'  case "$a" in\n' +
'    --no-client) NOCLIENT=1 ;;\n' +
"    --help|-h) sed -n '2,12p' \"$0\"; exit 0 ;;\n" +
'    *) echo "Bilinmeyen arguman: $a"; exit 1 ;;\n' +
'  esac\n' +
'done\n' +
'D="$ROOT/BuildLog/denetim"\n' +
'OUT="$D/build_all"; TLOGS="$OUT/targets"\n' +
'mkdir -p "$TLOGS"\n' +
'MASTER="$D/build_all.log"; SUM="$D/build_all_summary.txt"\n';
const newArg = 'NOCLIENT=0\n' +
"LOGDIR=''\n" +
'while [ $# -gt 0 ]; do\n' +
'  case "$1" in\n' +
'    --no-client) NOCLIENT=1 ;;\n' +
'    --logdir)\n' +
'      shift\n' +
'      [ $# -gt 0 ] || { echo "HATA: --logdir bir dizin bekliyor"; exit 1; }\n' +
'      LOGDIR="$1" ;;\n' +
"    --help|-h) sed -n '2,14p' \"$0\"; exit 0 ;;\n" +
'    *) echo "Bilinmeyen arguman: $1"; exit 1 ;;\n' +
'  esac\n' +
'  shift\n' +
'done\n' +
'D="$ROOT/BuildLog/denetim"\n' +
'LD="${LOGDIR:-$D}"\n' +
'OUT="$LD/build_all"; TLOGS="$OUT/targets"\n' +
'mkdir -p "$TLOGS"\n' +
'MASTER="$LD/build_all.log"; SUM="$LD/build_all_summary.txt"\n';
rep(oldArg, newArg, 1, 'b5-argblok');
fs.writeFileSync(f, s, 'latin1');
console.log('build_all.sh guncellendi, satir=' + s.split('\n').length);
