const fs = require('fs');
const p = 'BuildLog/denetim/build_all.sh';
let s = fs.readFileSync(p, 'latin1');
function rep(a, b, label) {
  const c = s.split(a).length - 1;
  if (c !== 1) throw new Error(label + ' capa=' + c);
  s = s.split(a).join(b);
}
const argBlock = [
  'NOCLIENT=0',
  'for a in "$@"; do',
  '  case "$a" in',
  '    --no-client) NOCLIENT=1 ;;',
  "    --help|-h) sed -n '2,12p' \"$0\"; exit 0 ;;",
  '    *) echo "Bilinmeyen arguman: $a"; exit 1 ;;',
  '  esac',
  'done',
].join('\n');
rep('cd "$ROOT" || exit 1\nD="$ROOT/BuildLog/denetim"',
  'cd "$ROOT" || exit 1\n' + argBlock + '\nD="$ROOT/BuildLog/denetim"', 'yerlesim');
rep('[ -x "$MSB" ] || { echo "HATA: MSBuild bulunamadi: $MSB"; exit 1; }\n' + argBlock,
  '[ -x "$MSB" ] || { echo "HATA: MSBuild bulunamadi: $MSB"; exit 1; }', 'eski-blok');
fs.writeFileSync(p, s, 'latin1');
console.log('OK: arg ayristirma log kirpmasindan once');
