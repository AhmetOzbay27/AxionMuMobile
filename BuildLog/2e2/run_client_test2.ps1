# run_client_test2.ps1 - 2e.2 SPK paket yerleşimi test koşucusu (Buffy)
# - 2d.2 harness türevi: -Exe ile hangi istemcinin koşacağı seçilir (Engine.exe/Main.exe)
# - launches <Exe> (args) in given dir
# - watches windows of process tree, PrintWindow captures (no desktop needed)
# - records TCP connections (ConnectIP consumption evidence)
# - kills all tree processes by PID
param(
  [Parameter(Mandatory=$true)][string]$Label,
  [string]$Arguments = '',
  [int]$ObserveSeconds = 22,
  [string]$ClientDir = 'C:\Axion Mu Source\BuildLog\2e2\deploy',
  [string]$Exe = 'Main.exe',
  [string]$OutDir = 'C:\Axion Mu Source\BuildLog\2e2\results',
  [string]$ShotDir = 'C:\Axion Mu Source\BuildLog\2e2\shots'
)

$ErrorActionPreference = 'Continue'
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms

$code = @"
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public class WinEnum {
  public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")] static extern bool EnumWindows(EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr hWnd, EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetClassName(IntPtr hWnd, StringBuilder sb, int max);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetWindowText(IntPtr hWnd, StringBuilder sb, int max);
  [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hWnd);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L; public int T; public int R; public int B; }
  [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
  public static string Rect(IntPtr h) { RECT r; if (!GetWindowRect(h, out r)) return ""; return r.L + "," + r.T + "," + r.R + "," + r.B; }
  public static bool PW(IntPtr h, IntPtr hdc, uint flags) { return PrintWindow(h, hdc, flags); }
  public static List<string> Snapshot(uint targetPid) {
    var list = new List<string>();
    EnumWindows(delegate(IntPtr h, IntPtr l) {
      uint pid; GetWindowThreadProcessId(h, out pid);
      if (pid != targetPid) return true;
      var cls = new StringBuilder(256); GetClassName(h, cls, 256);
      var txt = new StringBuilder(1024); GetWindowText(h, txt, 1024);
      list.Add("TOP|pid=" + pid + "|" + cls + "|" + txt + "|visible=" + (IsWindowVisible(h) ? "1" : "0") + "|hwnd=0x" + h.ToInt64().ToString("X"));
      EnumChildWindows(h, delegate(IntPtr c, IntPtr l2) {
        var cls2 = new StringBuilder(256); GetClassName(c, cls2, 256);
        var txt2 = new StringBuilder(2048); GetWindowText(c, txt2, 2048);
        if (txt2.Length > 0) list.Add("CHILD|" + cls2 + "|" + txt2);
        return true;
      }, IntPtr.Zero);
      return true;
    }, IntPtr.Zero);
    return list;
  }
}
"@
Add-Type -TypeDefinition $code -Language CSharp

$errors = New-Object System.Collections.ArrayList
function Note([string]$m) { if ($errors.Count -lt 12) { [void]$errors.Add($m) } }

function GrabWindow([string]$hwndHex, [string]$path) {
  try {
    $h = [IntPtr]([Convert]::ToInt64($hwndHex, 16))
    $rect = [WinEnum]::Rect($h)
    if ($rect -eq '') { return $false }
    $p = $rect -split ','
    $w = [int]$p[2] - [int]$p[0]; $hh = [int]$p[3] - [int]$p[1]
    if ($w -le 0 -or $hh -le 0 -or $w -gt 8000 -or $hh -gt 8000) { return $false }
    $bmp = New-Object System.Drawing.Bitmap $w, $hh
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    $ok = [WinEnum]::PW($h, $hdc, 2)
    $g.ReleaseHdc($hdc)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return $ok
  } catch { Note ("shot: " + $_.Exception.Message); return $false }
}

function Get-Tree([int]$rootPid) {
  try { $all = Get-CimInstance Win32_Process | Select-Object ProcessId, ParentProcessId, Name } catch { return @() }
  $seen = @{}; $queue = New-Object System.Collections.ArrayList
  [void]$queue.Add($rootPid)
  $result = New-Object System.Collections.ArrayList
  while ($queue.Count -gt 0) {
    $cur = [int]$queue[0]; $queue.RemoveAt(0)
    if ($seen.ContainsKey($cur)) { continue }
    $seen[$cur] = $true
    $nm = 'unknown'
    foreach ($p in $all) { if ([int]$p.ProcessId -eq $cur) { $nm = $p.Name; break } }
    [void]$result.Add([pscustomobject]@{ pid = $cur; name = $nm })
    foreach ($p in $all) { if ([int]$p.ParentProcessId -eq $cur) { [void]$queue.Add([int]$p.ProcessId) } }
  }
  return $result
}

$exe = Join-Path $ClientDir $Exe
$startTime = Get-Date
$shots = New-Object System.Collections.ArrayList
$history = New-Object System.Collections.ArrayList
$conns = New-Object System.Collections.ArrayList
$proc = $null
try {
  if ($Arguments -ne '') { $proc = Start-Process -FilePath $exe -ArgumentList $Arguments -WorkingDirectory $ClientDir -PassThru }
  else { $proc = Start-Process -FilePath $exe -WorkingDirectory $ClientDir -PassThru }
} catch { Note ("launch: " + $_.Exception.Message) }

$dialogSeen = $false
$errorTextSeen = $false
$killAt = $null
$finalTree = @()
$captured = @{}

if ($proc) {
  $deadline = (Get-Date).AddSeconds($ObserveSeconds)
  $lastKey = ''
  $t0 = Get-Date
  while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 500
    $tree = Get-Tree $proc.Id
    $treePids = @($tree | ForEach-Object { $_.pid })
    $treeNames = ($tree | ForEach-Object { $_.name + ':' + $_.pid }) -join ','
    $snap = @()
    foreach ($tp in $treePids) {
      $snap += [WinEnum]::Snapshot([uint32]$tp)
      if ($snap.Count -gt 400) { break }
    }
    $key = $treeNames + '||' + ($snap -join ';')
    if ($key -ne $lastKey) {
      $lastKey = $key
      [void]$history.Add(@{ t = [math]::Round(((Get-Date) - $t0).TotalSeconds, 1); tree = $treeNames; windows = @($snap) })
      foreach ($line in $snap) {
        if ($line -match 'inconsistent|0x000FF|Please verify|Invalid file|Please update|incorrect') { $errorTextSeen = $true }
      }
      $hasDialog = $false
      foreach ($line in $snap) {
        if ($line -match 'TOP\|[^|]*\|#32770\|') { $hasDialog = $true }
        if ($line -match '^TOP\|[^|]*\|[^|]*\|[^|]*\|visible=1\|hwnd=0x([0-9A-F]+)$') {
          $hx = $Matches[1]
          if (-not $captured.ContainsKey($hx) -and $captured.Count -lt 5) {
            $oc = @($line -split '\|')[3]
            if ($oc -ne '' -and $oc -ne 'Default IME' -and $oc -ne 'MSCTFIME UI' -and $oc -ne 'GDI+ Hook Window Class') {
              $captured[$hx] = $true
              $pth = Join-Path $ShotDir ($Label + "-win-0x" + $hx + ".png")
              if (GrabWindow $hx $pth) { [void]$shots.Add($pth) }
            }
          }
        }
      }
      if ($hasDialog -and -not $dialogSeen) {
        $dialogSeen = $true
        foreach ($line in $snap) {
          if ($line -match 'TOP\|[^|]*\|#32770\|.*hwnd=0x([0-9A-F]+)$') {
            $pth = Join-Path $ShotDir ($Label + "-dialog-0x" + $Matches[1] + ".png")
            if (GrabWindow $Matches[1] $pth) { [void]$shots.Add($pth) }
          }
        }
        $killAt = (Get-Date).AddSeconds(3)
      }
    }
    $el = ((Get-Date) - $t0).TotalSeconds
    if (($el -ge 5 -and $conns.Count -eq 0) -or ($el -ge 15 -and $conns.Count -lt 2)) {
      foreach ($tp in $treePids) {
        try {
          $tc = Get-NetTCPConnection -OwningProcess $tp -ErrorAction SilentlyContinue
          foreach ($c in $tc) {
            if ($c.RemoteAddress -ne '0.0.0.0' -and $c.RemoteAddress -ne '::') {
              [void]$conns.Add(@{ t = [math]::Round($el, 1); pid = $tp; remote = $c.RemoteAddress + ':' + $c.RemotePort; state = $c.State.ToString() })
            }
          }
        } catch {}
      }
    }
    if ($proc.HasExited -and $treePids.Count -le 1) { break }
    if ($killAt -and (Get-Date) -ge $killAt) { break }
  }
  $finalTree = Get-Tree $proc.Id
  foreach ($tp in @($finalTree | Sort-Object { $_.pid } -Descending)) {
    try { Stop-Process -Id $tp.pid -Force -ErrorAction SilentlyContinue } catch {}
  }
  Start-Sleep -Milliseconds 700
}

$md5 = @{}
foreach ($f in @('Data\SPK\ConnectIP.bmd', 'Data\SPK\ServerData.bmd', 'SPK.ini', $Exe)) {
  $fp = Join-Path $ClientDir $f
  if (Test-Path $fp) { $md5[$f] = (Get-FileHash $fp -Algorithm MD5).Hash.ToLower() }
}

$leftover = @()
Get-Process | Where-Object { $_.ProcessName -match 'Engine|Launcher|Update|Main' } | ForEach-Object { $leftover += ($_.ProcessName + ':' + $_.Id) }

$result = @{
  label             = $Label
  arguments         = $Arguments
  exe               = $Exe
  clientDir         = $ClientDir
  startTime         = $startTime.ToString('yyyy-MM-dd HH:mm:ss')
  endTime           = (Get-Date).ToString('yyyy-MM-dd HH:mm:ss')
  observeSeconds    = $ObserveSeconds
  dialogSeen        = $dialogSeen
  errorTextSeen     = $errorTextSeen
  rootPid           = if ($proc) { $proc.Id } else { $null }
  processTree       = @($finalTree | ForEach-Object { $_.name + ':' + $_.pid })
  leftoverProcesses = @($leftover)
  tcpConnections    = @($conns)
  windowsHistory    = @($history)
  screenshots       = @($shots)
  md5               = $md5
  errors            = @($errors)
}
$outPath = Join-Path $OutDir ($Label + '.json')
$result | ConvertTo-Json -Depth 8 | Out-File -FilePath $outPath -Encoding UTF8
Write-Output ("DONE " + $Label + " dialog=" + $dialogSeen + " errorText=" + $errorTextSeen + " leftover=" + ($leftover -join ',') + " json=" + $outPath)
