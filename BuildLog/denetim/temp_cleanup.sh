#!/bin/bash
# Gecici klasor temizligi: 1 gunden eski dosyalar (kullanimda olanlar silinemez).
export MSYS2_ARG_CONV_EXCL='*'
T='/c/Users/Administrator/AppData/Local/Temp'
F0=$(df -k /c | awk 'NR==2{print $4}')
echo "BASLANGIC bos_kb=$F0 $(date '+%T')"
N=0
while IFS= read -r -d '' f; do
  rm -f "$f" 2>/dev/null && N=$((N + 1))
done < <(find "$T" -type f -mtime +0 -print0 2>/dev/null)
find "$T" -mindepth 1 -type d -mtime +0 -empty -delete 2>/dev/null
F1=$(df -k /c | awk 'NR==2{print $4}')
echo "SILINEN_DOSYA=$N"
awk -v a="$F0" -v b="$F1" 'BEGIN{printf "KAZANC_MB=%.0f\n", (b-a)/1024}'
echo "BITIS bos_kb=$F1 $(date '+%T')"
echo TEMP_CLEAN_DONE
