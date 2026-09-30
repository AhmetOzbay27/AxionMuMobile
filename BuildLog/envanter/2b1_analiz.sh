#!/bin/bash
# 2b.1 — 213 ortak dosyanin bizim<->donor karsilastirmasi
# Cikti: 2b1_diff_matrisi.csv (noktali virgullu)
B="/c/Axion Mu Source/Source/4.GameServer/GameServer"
D="/c/Axion Mu Mobile/New Source Code/Source/Source/GameServer/GameServer"
cd "/c/Axion Mu Source/BuildLog/envanter"

echo "dosya;bizim_satir;donor_satir;birebir;norm_benzer;ortak_metot;bizim_metot;donor_metot;ham_diff_eklenen;ham_diff_silinen" > 2b1_diff_matrisi.csv

norm() { sed 's/^[[:space:]]*//; s/[[:space:]]*$//' "$1" 2>/dev/null | sed 's/[[:space:]]\+/ /g' | grep -v "^$" | grep -v "^//" | grep -v "^/\*" | grep -v "^ \*" | sort -u; }
meth() { grep -hoE "[A-Za-z_][A-Za-z0-9_]*::[A-Za-z_][A-Za-z0-9_]*" "$1" 2>/dev/null | sort -u; }

while read f; do
  b="$B/$f"; d="$D/$f"
  bl=$(wc -l < "$b"); dl=$(wc -l < "$d")
  if cmp -s "$b" "$d"; then identik=1; else identik=0; fi
  a=$(norm "$b"); dd=$(norm "$d")
  c=$(comm -12 <(echo "$a") <(echo "$dd") | wc -l)
  u=$(cat <(echo "$a") <(echo "$dd") | sort -u | wc -l)
  if [ "$u" -gt 0 ]; then ben=$(( c * 100 / u )); else ben=100; fi
  ma=$(meth "$b"); md=$(meth "$d")
  ca=$(echo "$ma" | grep -c .); cdn=$(echo "$md" | grep -c .)
  co=$(comm -12 <(echo "$ma") <(echo "$md") | wc -l)
  de=$(diff "$b" "$d" 2>/dev/null | grep -c "^>")
  ds=$(diff "$b" "$d" 2>/dev/null | grep -c "^<")
  echo "$f;$bl;$dl;$identik;$ben;$co;$ca;$cdn;$de;$ds" >> 2b1_diff_matrisi.csv
done < ortak_2b1.txt
echo "bitti: $(wc -l < 2b1_diff_matrisi.csv) satir"
