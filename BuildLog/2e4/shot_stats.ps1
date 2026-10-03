# shot_stats.ps1 - PNG ekran goruntusu metin analizi (ASCII onizleme + renk istatistigi)
param([string[]]$Paths)
Add-Type -AssemblyName System.Drawing
foreach ($p in $Paths) {
  if (-not (Test-Path $p)) { Write-Output ("MISSING " + $p); continue }
  $bmp = [System.Drawing.Bitmap]::FromFile($p)
  $w = $bmp.Width; $h = $bmp.Height
  $cols = 64; $rows = 24
  $chars = ' .:-=+*#%@'
  $sb = New-Object System.Text.StringBuilder
  $sumR = 0; $sumG = 0; $sumB = 0; $n = 0
  $distinct = @{}
  for ($ry = 0; $ry -lt $rows; $ry++) {
    $line = ''
    for ($rx = 0; $rx -lt $cols; $rx++) {
      $x0 = [int]($rx * $w / $cols); $x1 = [int](($rx + 1) * $w / $cols) - 1
      $y0 = [int]($ry * $h / $rows); $y1 = [int](($ry + 1) * $h / $rows) - 1
      if ($x1 -lt $x0) { $x1 = $x0 }; if ($y1 -lt $y0) { $y1 = $y0 }
      $r = 0; $g = 0; $b = 0; $c = 0
      for ($y = $y0; $y -le $y1; $y += 2) {
        for ($x = $x0; $x -le $x1; $x += 2) {
          $px = $bmp.GetPixel($x, $y)
          $r += $px.R; $g += $px.G; $b += $px.B; $c++
        }
      }
      if ($c -eq 0) { $c = 1 }
      $r = [int]($r / $c); $g = [int]($g / $c); $b = [int]($b / $c)
      $sumR += $r; $sumG += $g; $sumB += $b; $n++
      $key = "$r,$g,$b"
      if (-not $distinct.ContainsKey($key)) { $distinct[$key] = 0 }
      $distinct[$key]++
      $lum = [int](0.299 * $r + 0.587 * $g + 0.114 * $b)
      $idx = [int]($lum * ($chars.Length - 1) / 255)
      $line += $chars[$idx]
    }
    [void]$sb.AppendLine($line)
  }
  $bmp.Dispose()
  Write-Output ("=== " + (Split-Path $p -Leaf) + " ${w}x${h} avgRGB=(" + [int]($sumR / $n) + "," + [int]($sumG / $n) + "," + [int]($sumB / $n) + ") distinctGridColors=" + $distinct.Count)
  Write-Output $sb.ToString()
}
