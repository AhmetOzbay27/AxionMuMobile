# downscale.ps1 - PNG'yi kucultup kaydeder (goruntuyu incelemek icin)
param([Parameter(Mandatory=$true)][string]$In, [Parameter(Mandatory=$true)][string]$Out, [int]$MaxW = 1000, [int]$MaxH = 700)
Add-Type -AssemblyName System.Drawing
$src = [System.Drawing.Bitmap]::FromFile($In)
$scale = [Math]::Min($MaxW / $src.Width, $MaxH / $src.Height)
if ($scale -gt 1) { $scale = 1 }
$w = [int]($src.Width * $scale); $h = [int]($src.Height * $scale)
$dst = New-Object System.Drawing.Bitmap $w, $h
$g = [System.Drawing.Graphics]::FromImage($dst)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.DrawImage($src, 0, 0, $w, $h)
$dst.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $dst.Dispose(); $src.Dispose()
Write-Output ("saved $Out ${w}x${h}")
