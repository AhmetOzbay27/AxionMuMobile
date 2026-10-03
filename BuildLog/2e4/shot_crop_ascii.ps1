# shot_crop_ascii.ps1 - PNG'nin bir bolgesini yuksek cozunurlukte ASCII'ye cevirir (metin okumak icin)
param(
  [Parameter(Mandatory=$true)][string]$Path,
  [int]$X0 = 0, [int]$Y0 = 0, [int]$X1 = 0, [int]$Y1 = 0,
  [int]$Cols = 110, [int]$Rows = 34,
  [int]$Threshold = 40,
  [switch]$Binary
)
Add-Type -AssemblyName System.Drawing
$bmp = [System.Drawing.Bitmap]::FromFile($Path)
if ($X1 -le 0) { $X1 = $bmp.Width }
if ($Y1 -le 0) { $Y1 = $bmp.Height }
Write-Output ("IMG " + $bmp.Width + "x" + $bmp.Height + " crop=(" + $X0 + "," + $Y0 + ")-(" + $X1 + "," + $Y1 + ")")
$chars = ' .:-=+*#%@'
for ($ry = 0; $ry -lt $Rows; $ry++) {
  $line = ''
  for ($rx = 0; $rx -lt $Cols; $rx++) {
    $cxa = [int]($X0 + $rx * ($X1 - $X0) / $Cols); $cxb = [int]($X0 + ($rx + 1) * ($X1 - $X0) / $Cols) - 1
    $cya = [int]($Y0 + $ry * ($Y1 - $Y0) / $Rows); $cyb = [int]($Y0 + ($ry + 1) * ($Y1 - $Y0) / $Rows) - 1
    if ($cxb -lt $cxa) { $cxb = $cxa }; if ($cyb -lt $cya) { $cyb = $cya }
    $mx = 0
    for ($y = $cya; $y -le $cyb; $y++) {
      for ($x = $cxa; $x -le $cxb; $x++) {
        $px = $bmp.GetPixel($x, $y)
        $l = [int](0.299 * $px.R + 0.587 * $px.G + 0.114 * $px.B)
        if ($l -gt $mx) { $mx = $l }
        $c++
      }
    }
    if ($Binary) { $line += $(if ($mx -ge $Threshold) { '#' } else { ' ' }); continue }
    if ($mx -lt $Threshold) { $line += ' '; continue }
    $idx = [int](($mx - $Threshold) * ($chars.Length - 1) / (255 - $Threshold))
    if ($idx -lt 1) { $idx = 1 }
    $line += $chars[$idx]
  }
  Write-Output ("|" + $line + "|")
}
$bmp.Dispose()
