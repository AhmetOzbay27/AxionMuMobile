#!/bin/bash
# deploy_spk_package.sh - 2e.2 paket kurulum betigi (canliya DOKUNMAZ; test klasoru uretir)
#
# Canli SPK paket duzeni (Client and Tools\Client) ile bizim derleme cikimizi birlestirir:
#   Main.exe + wzAudio.dll(+ogg/vorbisfile) + SPK.ini + Data\SPK\* (bizim uretim) + Data\SPK\Config
# Ayrica Main.exe'nin bekledigi MUIG yollarini (Data\Local\*) SPK Config kopyalariyla esler
# (kaynak koddaki SPK-first cozumleme ileride bu eslemeyi gereksiz kilacak - bkz. docs/22).
#
# Kullanim: bash deploy_spk_package.sh [--with-assets]
set -u
SRC_REPO="/c/Axion Mu Source"
LIVE="/c/Axion Mu Mobile/Client and Tools/Client"
OUT="$SRC_REPO/BuildLog/2e2/deploy"
WITH_ASSETS="${1:-}"

echo "== 1) temizlik: $OUT"
rm -rf "$OUT"
mkdir -p "$OUT/Data/SPK/Config" "$OUT/Data/Local/Eng" "$OUT/Data/Custom"

echo "== 2) istemci binary + kok dosyalar (canli paket duzeni)"
cp -f "$SRC_REPO/ClientFile/Main.exe"    "$OUT/Main.exe"
cp -f "$LIVE/SPK.ini"                    "$OUT/SPK.ini"
cp -f "$LIVE/wzAudio.dll"                "$OUT/wzAudio.dll"     # Main.exe importu
cp -f "$LIVE/ogg.dll"                    "$OUT/ogg.dll"         # wzAudio bagimliligi
cp -f "$LIVE/vorbisfile.dll"             "$OUT/vorbisfile.dll"  # wzAudio bagimliligi (eksikse loader hard-error verir)

echo "== 3) SPK veri dosyalari (2d.2 uretimi)"
cp -f "$SRC_REPO/BuildLog/2d2/deploy/ConnectIP.bmd" "$OUT/Data/SPK/ConnectIP.bmd"
cp -f "$SRC_REPO/BuildLog/2d2/deploy/ServerData.bmd" "$OUT/Data/SPK/ServerData.bmd"

echo "== 4) SPK config agaci (canli)"
cp -rf "$LIVE/Data/SPK/Config/." "$OUT/Data/SPK/Config/"

echo "== 5) Data\\Local ek dosyalar (canli pakette olanlar; CBGetMain.bin/CBTextInfo.bin YOK)"
cp -f "$LIVE/Data/Enc1.dat" "$LIVE/Data/Dec2.dat" "$OUT/Data/"
cp -rf "$LIVE/Data/Local/." "$OUT/Data/Local/"

echo "== 6) MUIG -> SPK yol eslemesi (Main.exe'nin literal Data\\Local yollari)"
map() { # map <spk-kaynak> <paket-hedef>
  if [ -f "$1" ]; then cp -f "$1" "$2"; echo "   $(basename "$2")  <-  ${1#$LIVE/}"; else echo "   ATLA (yok): $1"; fi
}
C="$LIVE/Data/SPK/Config"
map "$C/Mix.bmd"                "$OUT/Data/Local/Mix.bmd"
map "$C/Filter.bmd"             "$OUT/Data/Local/Filter.bmd"
map "$C/FilterName.bmd"         "$OUT/Data/Local/FilterName.bmd"
map "$C/ItemAddOption.bmd"      "$OUT/Data/Local/ItemAddOption.bmd"
map "$C/Credit.bmd"             "$OUT/Data/Local/credit.bmd"
map "$C/Pet.bmd"                "$OUT/Data/Local/pet.bmd"
map "$C/MasterSkillTreeData.bmd" "$OUT/Data/Local/MasterSkillTreeData.bmd"
map "$C/MonsterSkill.bmd"       "$OUT/Data/Local/MonsterSkill.bmd"
map "$C/NPCDialogue.bmd"        "$OUT/Data/Local/NPCDialogue.bmd"
map "$C/QuestProgress.bmd"      "$OUT/Data/Local/QuestProgress.bmd"
map "$C/ServerList.bmd"         "$OUT/Data/Local/ServerList.bmd"
map "$C/Gate.bmd"               "$OUT/Data/Gate.bmd"
map "$C/Slide.bmd"              "$OUT/Data/Local/Eng/Slide_Eng.bmd"   # NewUISlideWindow: Data\Local\<Lang>\Slide_<Lang>.bmd

# 6b) genel esleme: Config\<Ad>.bmd ->  Data\Local\<Ad>.bmd  ve  Data\Local\Eng\<Ad>_Eng.bmd
# (Main.exe bazi tablolari "Data\Local\<Lang>\<Ad>_<Lang>.bmd" kalibinda ariyor)
for f in "$C"/*.bmd "$C"/*.txt; do
  [ -f "$f" ] || continue
  b=$(basename "$f"); n="${b%.*}"; e="${b##*.}"
  cp -f "$f" "$OUT/Data/Local/$b"
  cp -f "$f" "$OUT/Data/Local/Eng/${n}_Eng.$e"
done
echo "   (genel esleme: $(ls "$C" | wc -l) dosya -> Data\Local + Data\Local\Eng)"

if [ "$WITH_ASSETS" = "--with-assets" ]; then
  # NOT: Disk dolu (C: ~%100); baslik/login sahnesi icin gerekli asgari set kopyalanir.
  # Buyuk agaclar (Map 562M, Sound 210M, Music 120M, Custom 80M, Monster 87M, Player 56M,
  # Item 31M, NPC 24M) ileride junction ya da tam kopya ile eklenir (bkz. docs/22 § sinirlar).
  echo "== 7) gorsel varliklar (asgari set: Interface + Logo)"
  for d in Interface Logo Player; do
    if [ -d "$LIVE/Data/$d" ]; then mkdir -p "$OUT/Data/$d"; cp -rf "$LIVE/Data/$d/." "$OUT/Data/$d/"; echo "   $d"; fi
  done
  echo "== 7b) Data\Custom yapilandirma dosyalari (yalniz kok, agac degil)"
  if [ -d "$LIVE/Data/Custom" ]; then mkdir -p "$OUT/Data/Custom"; find "$LIVE/Data/Custom" -maxdepth 1 -type f -exec cp -f {} "$OUT/Data/Custom/" \; ; echo "   $(ls "$OUT/Data/Custom" | wc -l) dosya"; fi
fi

echo "== ozet"
du -sh "$OUT"
find "$OUT" -maxdepth 2 -type d | sed "s|$OUT|  deploy|" | sort
echo "== kok dosyalar"
ls -la "$OUT" | grep -vE "^total|^d" | awk '{print "  "$5"\t"$9}'
