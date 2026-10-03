# run19_watch.ps1 - 2e.4/Faz3 E2E: yuksek frekansli TCP izleme (100 ms) + pencere ekran goruntusu
# Amac: run18'de gorunmeyen 127.0.0.1:63000 baglantisini kesin kanitlamak.
# Sunucu yigini (CS 63000 / DS 63002 / JS 63003 / GS 55901) ayakta olmali.
param(
  [string]$Label = 'run19-watch',
  [string]$ClientDir = 'C:\Axion Mu Source\BuildLog\2e2\deploy',
  [string]$Exe = 'Main.exe',
  [int]$ObserveSeconds = 80,
  [string]$OutDir = 'C:\Axion Mu Source\BuildLog\2e4\results',
  [string]$ShotDir = 'C:\Axion Mu Source\BuildLog\2e4\shots',
  [string]$ShotTimes = '20,30,45,60,75'
)

$ErrorActionPreference = 'Continue'
Add-Type -AssemblyName System.Drawing

$code = @"
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public class W19 {
  public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")] static extern bool EnumWindows(EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetClassName(IntPtr hWnd, StringBuilder sb, int max);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetWindowText(IntPtr hWnd, StringBuilder sb, int max);
  [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hWnd);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L; public int T; public int R; public int B; }
  [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
  public static string Rect(IntPtr h) { RECT r; if (!GetWindowRect(h, out r)) return ""; return r.L + "," + r.T + "," + r.R + "," + r.B; }
  public static bool PW(IntPtr h, IntPtr hdc, uint flags) { return PrintWindow(h, hdc, flags); }
  public static int MainHwnd(uint targetPid) {
    int best = 0;
    EnumWindows(delegate(IntPtr h, IntPtr l) {
      uint pid; GetWindowThreadProcessId(h, out pid);
      if (pid == targetPid) {
        var cls = new StringBuilder(256); GetClassName(h, cls, 256);
        var txt = new StringBuilder(512); GetWindowText(h, txt, 512);
        if (txt.Length > 0 && cls.ToString() != "IME" && cls.ToString() != "MSCTFIME UI") { best = (int)h.ToInt64(); return false; }
      }
      return true;
    }, IntPtr.Zero);
    return best;
  }
  public static string[] Windows(uint targetPid) {
    var list = new List<string>();
    EnumWindows(delegate(IntPtr h, IntPtr l) {
      uint pid; GetWindowThreadProcessId(h, out pid);
      if (pid != targetPid) return true;
      var cls = new StringBuilder(256); GetClassName(h, cls, 256);
      var txt = new StringBuilder(512); GetWindowText(h, txt, 512);
      list.Add(cls + "|" + txt + "|vis=" + (IsWindowVisible(h) ? 1 : 0) + "|0x" + h.ToInt64().ToString("X"));
      return true;
    }, IntPtr.Zero);
    return list.ToArray();
  }
}
"@
Add-Type -TypeDefinition $code -Language CSharp

function GrabHwnd([int]$hwndInt, [string]$path) {
  try {
    $h = [IntPtr]$hwndInt
    $rect = [W19]::Rect($h)
    if ($rect -eq '') { return $false }
    $p = $rect -split ','
    $w = [int]$p[2] - [int]$p[0]; $hh = [int]$p[3] - [int]$p[1]
    if ($w -le 0 -or $hh -le 0 -or $w -gt 8000 -or $hh -gt 8000) { return $false }
    $bmp = New-Object System.Drawing.Bitmap $w, $hh
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    $ok = [W19]::PW($h, $hdc, 2)
    $g.ReleaseHdc($hdc)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return $ok
  } catch { return $false }
}

$exe = Join-Path $ClientDir $Exe
$shots = New-Object System.Collections.ArrayList
$events = New-Object System.Collections.ArrayList
$windowLog = New-Object System.Collections.ArrayList
$pidConns = New-Object System.Collections.ArrayList
$shotSet = $ShotTimes -split ',' | ForEach-Object { [double]$_ }
$shotDone = @{}
$seen = @{}
$proc = $null
$t0 = Get-Date
try { $proc = Start-Process -FilePath $exe -WorkingDirectory $ClientDir -PassThru }
catch { Write-Output ("LAUNCH-ERROR " + $_.Exception.Message); exit 1 }
$mainPid = $proc.Id

$netstatCount = 0
while (((Get-Date) - $t0).TotalSeconds -lt $ObserveSeconds) {
  Start-Sleep -Milliseconds 100
  $el = [math]::Round(((Get-Date) - $t0).TotalSeconds, 2)

  # 1) Global aktif TCP: hedef portlara dokunan tum baglantilar (bagimsiz PID)
  try { $tc = [System.Net.NetworkInformation.IPGlobalProperties]::GetIPGlobalProperties().GetActiveTcpConnections() }
  catch { $tc = @() }
  foreach ($c in $tc) {
    $lp = $c.LocalEndPoint.Port; $rp = $c.RemoteEndPoint.Port
    $tag = ''
    if ($rp -ge 63000 -and $rp -le 63003) { $tag = 'to-local-server' }
    elseif ($lp -ge 63000 -and $lp -le 63003) { $tag = 'from-local-server' }
    elseif ($rp -ge 55901 -and $rp -le 55919) { $tag = 'to-game-server' }
    elseif ($lp -ge 55901 -and $lp -le 55919) { $tag = 'from-game-server' }
    if ($tag -ne '') {
      $key = "$lp>$rp|" + $c.State.ToString()
      if (-not $seen.ContainsKey($key)) {
        $seen[$key] = $true
        [void]$events.Add([pscustomobject]@{ t = $el; tag = $tag; local = $c.LocalEndPoint.ToString(); remote = $c.RemoteEndPoint.ToString(); state = $c.State.ToString() })
      }
    }
  }

  # 2) Her ~1 sn: Main.exe PID'ine ait baglantilar (netstat -ano)
  $netstatCount++
  if ($netstatCount % 10 -eq 0) {
    try {
      $ns = netstat -ano | Select-String -Pattern 'TCP' | ForEach-Object { $_.ToString() }
      foreach ($line in $ns) {
        if ($line -match '\s+(' + $mainPid + ')\s*$') {
          [void]$pidConns.Add([pscustomobject]@{ t = $el; line = ($line -replace '\s+', ' ').Trim() })
        }
      }
    } catch {}
  }

  # 3) Pencere listesi degisimi
  if (-not $shotDone.ContainsKey('winlist')) {
    $wins = [W19]::Windows([uint32]$mainPid)
    if ($wins.Count -gt 0) {
      $shotDone['winlist'] = $true
      [void]$windowLog.Add([pscustomobject]@{ t = $el; windows = $wins })
    }
  }

  # 4) Zamanli ekran goruntuleri
  foreach ($st in $shotSet) {
    if ($el -ge $st -and -not $shotDone.ContainsKey("t$st")) {
      $shotDone["t$st"] = $true
      $hw = [W19]::MainHwnd([uint32]$mainPid)
      if ($hw -ne 0) {
        $pth = Join-Path $ShotDir ($Label + "-t" + ([int]$st) + "s.png")
        if (GrabHwnd $hw $pth) { [void]$shots.Add($pth) }
      }
    }
  }

  if ($proc.HasExited) { break }
}

try { Stop-Process -Id $mainPid -Force -ErrorAction SilentlyContinue } catch {}
Start-Sleep -Milliseconds 500

$result = [pscustomobject]@{
  label = $Label
  exe = $exe
  observeSeconds = $ObserveSeconds
  rootPid = $mainPid
  exitedEarly = $proc.HasExited
  tcpEvents = @($events)
  pidConnections = @($pidConns)
  windowLog = @($windowLog)
  screenshots = @($shots)
}
$outPath = Join-Path $OutDir ($Label + '.json')
$result | ConvertTo-Json -Depth 8 | Out-File -FilePath $outPath -Encoding UTF8
Write-Output ("DONE " + $Label + " events=" + $events.Count + " pidconns=" + $pidConns.Count + " shots=" + $shots.Count + " json=" + $outPath)
