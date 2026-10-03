# shot_bbox_ascii.ps1 - parlak icerigin sinir kutusunu bulur ve ASCII olarak basar (LockBits ile hizli)
param(
  [Parameter(Mandatory=$true)][string]$Path,
  [int]$Threshold = 25,
  [int]$Cols = 130,
  [int]$Rows = 40,
  [int]$Pad = 6
)
Add-Type -AssemblyName System.Drawing
$bmp = [System.Drawing.Bitmap]::FromFile($Path)
$w = $bmp.Width; $h = $bmp.Height
$rect = New-Object System.Drawing.Rectangle 0, 0, $w, $h
$data = $bmp.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$stride = $data.Stride
$bytes = New-Object byte[] ($stride * $h)
[System.Runtime.InteropServices.Marshal]::Copy($data.Scan0, $bytes, 0, $bytes.Length)
$bmp.UnlockBits($data)
$bmp.Dispose()

function Lum([int]$x, [int]$y) {
  $off = $y * $stride + $x * 4
  $b = $bytes[$off]; $g = $bytes[$off + 1]; $r = $bytes[$off + 2]
  return (0.299 * $r + 0.587 * $g + 0.114 * $b)
}

$minX = $w; $maxX = -1; $minY = $h; $maxY = -1; $count = 0
for ($y = 0; $y -lt $h; $y += 1) {
  for ($x = 0; $x -lt $w; $x += 1) {
    if ((Lum $x $y) -ge $Threshold) {
      if ($x -lt $minX) { $minX = $x }; if ($x -gt $maxX) { $maxX = $x }
      if ($y -lt $minY) { $minY = $y }; if ($y -gt $maxY) { $maxY = $y }
      $count++
    }
  }
}
Write-Output ("IMG ${w}x${h} thr=$Threshold brightPixels=$count bbox=($minX,$minY)-($maxX,$maxY)")
if ($maxX -lt 0) { Write-Output "NO-CONTENT"; exit 0 }

$X0 = [Math]::Max(0, $minX - $Pad); $Y0 = [Math]::Max(0, $minY - $Pad)
$X1 = [Math]::Min($w, $maxX + 1 + $Pad); $Y1 = [Math]::Min($h, $maxY + 1 + $Pad)
Write-Output ("CROP=(" + $X0 + "," + $Y0 + ")-(" + $X1 + "," + $Y1 + ") size=" + ($X1 - $X0) + "x" + ($Y1 - $Y0))
$chars = ' .:-=+*#%@'
for ($ry = 0; $ry -lt $Rows; $ry++) {
  $line = ''
  for ($rx = 0; $rx -lt $Cols; $rx++) {
    $x0 = [int]($X0 + $rx * ($X1 - $X0) / $Cols); $x1 = [int]($X0 + ($rx + 1) * ($X1 - $X0) / $Cols) - 1
    $y0 = [int]($Y0 + $ry * ($Y1 - $Y0) / $Rows); $y1 = [int]($Y0 + ($ry + 1) * ($Y1 - $Y0) / $Rows) - 1
    if ($x1 -lt $x0) { $x1 = $x0 }; if ($y1 -lt $y0) { $y1 = $y0 }
    $mx = 0
    for ($y = $y0; $y -le $y1; $y++) {
      for ($x = $x0; $x -le $x1; $x++) {
        $l = Lum $x $y
        if ($l -gt $mx) { $mx = $l }
      }
    }
    if ($mx -lt $Threshold) { $line += ' '; continue }
    $idx = [int](($mx - $Threshold) * ($chars.Length - 1) / (255 - $Threshold))
    if ($idx -lt 1) { $idx = 1 }
    $line += $chars[$idx]
  }
  Write-Output ("|" + $line + "|")
}
