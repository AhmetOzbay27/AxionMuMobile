function hex2dec(s,   i,c,d,n) {
  n = 0
  s = tolower(s)
  gsub(/^0x/, "", s)
  for (i = 1; i <= length(s); i++) {
    c = substr(s, i, 1)
    d = index("0123456789abcdef", c) - 1
    if (d < 0) return n
    n = n * 16 + d
  }
  return n
}
FNR == NR {
  if ($0 ~ /^[0-9a-fA-F]+$/) targets[++nt] = $0
  next
}
/^[ \t]*[0-9]+:[0-9a-f]+[ \t]+/ {
  split($1, a, ":")
  if (a[1] != "0001") next
  addr = hex2dec(a[2])
  sym = $2
  if (sym == "") next
  naddr[++n] = addr
  nsym[n] = sym
}
END {
  for (t = 1; t <= nt; t++) {
    tgt = hex2dec(targets[t])
    best = -1
    bsym = "(none)"
    for (i = 1; i <= n; i++) {
      if (naddr[i] <= tgt && naddr[i] >= best) { best = naddr[i]; bsym = nsym[i] }
    }
    printf "%s (0x%x) -> %s +0x%x\n", targets[t], tgt, bsym, tgt - best
  }
}
