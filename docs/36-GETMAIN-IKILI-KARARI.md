# 36 - Sahipsiz GetMain Ikili Karari (GetMainInfo.exe / GetMainInfo.pdb)

> Tarih: 2026-10-04 12:08
> Soru (kullanici): "Sahipsiz GetMain ikililerini (GetMainInfo.exe/pdb) incele:
> yeniden derlenmeli mi, hangi commit'e ait olmali, yoksa geri alinmali mi karar ver"
> **Karar: HEAD'e geri alindi.** Islevsel fark yok; fark tamamen derleme meta verisi.

## Baglam

`GetMain/` dagitim klasorunde yalniz iki dosya var: `GetMainInfo.exe` + `GetMainInfo.pdb`;
ikisi de 03.10 15:02 damgali (Main 603 derleme turu artigi). Git durumu:
`M GetMain/GetMainInfo.exe`, `M GetMain/GetMainInfo.pdb` seklindeydi; baska commit'siz
kaynak degisikligi YOK (bu turda dogrulandi).

## Bulgular (kanit)

1. Boyutlar HEAD ile ayni: exe 3.723.776 B, pdb 6.533.120 B.
2. md5 farkli: worktree exe `1991c037ead6b59129f77e51c0774ae7` / pdb
   `020c03fa3a9dcdb38aaf2eddb8a26be3`; HEAD exe `c480e0baf79cd1bf32c8bcfc79c1aed1` /
   pdb `c1e9fe738af6843daa3f35d9d14dd24f`.
3. `cmp -l` (exe): TAM 16 bayt fark ->
   - 3 bayt (offset 273-275, 1-tabanli): PE COFF `TimeDateStamp`
     (HEAD 1790938967 = 2026-10-02 11:02:47Z; yeni 1791028955 = 2026-10-03 12:02:35Z;
     yeni damga dosya mtime 03.10 15:02 ile uyumlu),
   - 12 bayt: 4 x debug-directory girdisi `TimeDateStamp` (28 B araliklarla:
     188261, 188289, 188317, 188345),
   - 1 bayt: RSDS `Age` 4 -> 5 (offset 190861) - PDB'ye 5. link.
4. PE bolum denetimi (`BuildLog/denetim/pe_cmp.js`): toplam fark 16; dagilim
   HEADER=3, `.rdata`=13; `.text` dahil DIGER TUM BOLUMLER ve dosya uzunlugu birebir
   ayni -> **kod baytlari ozdes**.
5. PDB farki (1.643.190 bayt) PDB ic yapisinin (GUID/zaman/akis duzeni) dogal sonucu;
   exe kodu degismedigi icin islevsel anlam yok.
6. Derleme izi (`BuildLog/Getmain/`): 03.10 15:02 tarihli ~30 obj + tlog +
   `GetMainInfo.exe.recipe`; recipe cikisi `C:\Axion Mu Source\GetMain\GetMainInfo.exe`.
   Link girdileri: 5.Main paylasilan objeleri (CCRC32, Custom*, ItemToolTip, Message,
   UIMapName, ...) + `Source/6.GetMainInfo/.../SPK/*` (main_spk, GetEngineConfig,
   ServerDataWriter, ConnectIPWriter, CrcFileReport, SpkUtil) + GetMainInfo.obj/res.
7. Git gecmisi: ikililer yalniz iki commit'te degisti -> `1339a2903` (ilk derlemeler) ve
   `32f5cdcb0` (GetMainInfo 2d.1; exe 3.693.568 -> 3.723.776 B, pdb ilk kez izlendi).
   HEAD (`5e1cb045c`) bu ikilileri zaten iceriyor.
8. `.lib` / `.exp` izlenmiyor (`.gitignore:18,20`). Link satiri `.lib` uretse de dagitim
   klasoru bilincli olarak yalniz exe+pdb (2 dosya).

## Karar ve uygulama

Geri alindi:

```
git checkout -- GetMain/GetMainInfo.exe GetMain/GetMainInfo.pdb
```

Sonuc: md5'ler HEAD bloguyla birebir (`c480e0ba...` / `c1e9fe73...`), `git diff` bos,
`GetMain/` temiz.

**Gerekce:**

- Islevsel fark yok (madde 3-5): yeni ikili ayni kaynaktan yeniden derleme; kod ozdes,
  fark yalniz damga/age. Kaynakta da commit'siz degisiklik yok -> yeni ikilinin tasidigi
  hicbir anlam yok.
- "Hangi commit'e ait olmali?": ikililerin ait oldugu soy `32f5cdcb0`; HEAD onun ikilisini
  zaten iceriyor. Geri alma = dogru soya donus.
- Meta-damga farkini commit etmek kalici ikili gurultusu olurdu; her yeniden derleme bu
  16 bayti zaten degistirir (RSDS age her link'te artar).
- "Yeniden derlenmeli mi?": gerek yok - ayni kaynaktan derleme koda dokunmuyor
  (kanitlandi); yeniden derleme ancak kaynak degistiginde anlamli.

## Korunan kanit

- `BuildLog/denetim/getmain_cmp_exe.txt` - `cmp -l` dokumu (16 satir).
- `BuildLog/denetim/pe_cmp.js` - PE bolum karsilastirma araci. Yeniden uretim:
  `git cat-file -p HEAD:GetMain/GetMainInfo.exe > BuildLog/getmain_head.exe && node BuildLog/denetim/pe_cmp.js`.
- `BuildLog/Getmain/GetMainInfo.rebuilt.exe` + `.pdb` - geri alinan yeniden derlemenin
  arsiv kopyasi (izlenmiyor; gerekirse silinebilir).

## Kalici kural

1. Kaynak degismeden ikili yeniden derleme COMMIT EDILMEZ; dagitim gerekiyorsa gecici
   kopyadan yapilir.
2. `6.GetMainInfo` veya bagladigi 5.Main objelerinin kaynagi degistiginde: yeniden derle,
   kod-bolumu farkini kanitla (`pe_cmp.js`), sonra yeni commit ile `GetMain/` ikililerini
   guncelle.

**Ilgili:** docs/08, docs/20, docs/35; commit `32f5cdcb0`, HEAD `5e1cb045c`.
