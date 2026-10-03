# grid_lum.ps1 - ekran goruntusunun izgara bazli ortalama parlaklik haritasi (sayisal)
param([Parameter(Mandatory=$true)][string]$Path, [int]$Cols = 64, [int]$Rows = 24)
Add-Type -AssemblyName System.Drawing
$bmp = [System.Drawing.Bitmap]::FromFile($Path)
$w = $bmp.Width; $h = $bmp.Height
Write-Output ("IMG ${w}x${h} avg=$( ($bmp.GetPixel(0,0).R) )")
for ($ry = 0; $ry -lt $Rows; $ry++) {
  $line = ''
  for ($rx = 0; $rx -lt $Cols; $rx++) {
    $x0 = [int]($rx * $w / $Cols); $x1 = [int](($rx + 1) * $w / $Cols) - 1
    $y0 = [int]($ry * $h / $Rows); $y1 = [int](($ry + 1) * $h / $Rows) - 1
    if ($x1 -lt $x0) { $x1 = $x0 }; if ($y1 -lt $y0) { $y1 = $y0 }
    $sum = 0; $c = 0
    for ($y = $y0; $y -le $y1; $y += 2) {
      for ($x = $x0; $x -le $x1; $x += 2) {
        $px = $bmp.GetPixel($x, $y)
        $sum += [int](0.299 * $px.R + 0.587 * $px.G + 0.114 * $px.B); $c++
      }
    }
    if ($c -eq 0) { $c = 1 }
    $v = [int]($sum / $c)
    $line += ('{0,4}' -f $v)
  }
  Write-Output ("r" + $ry.ToString('00') + ":" + $line)
}
$bmp.Dispose()
