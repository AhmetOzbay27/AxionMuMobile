# run22_e2e.ps1 - Faz3 E2E: istemciyi baslat, TCP izle, zamanli tiklama + ekran goruntusu
# Kullanim ornegi:
#   powershell -File run22_e2e.ps1 -Label run22 -ObserveSeconds 80 -ClickTimes "12,16" -ClickFrac "0.36,0.39"
param(
  [string]$Label = 'run22-e2e',
  [string]$ClientDir = 'C:\Axion Mu Source\BuildLog\2e2\deploy',
  [string]$Exe = 'Main.exe',
  [int]$ObserveSeconds = 80,
  [string]$OutDir = 'C:\Axion Mu Source\BuildLog\2e4\results',
  [string]$ShotDir = 'C:\Axion Mu Source\BuildLog\2e4\shots',
  [string]$ShotTimes = '4,8,12,16,20,25,30,40,55,70',
  [string]$ClickTimes = '12',
  [string]$ClickFrac = '0.36,0.39',
  [switch]$RealInput
)

$ErrorActionPreference = 'Continue'
Add-Type -AssemblyName System.Drawing

$code = @"
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public class W22 {
  public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")] static extern bool EnumWindows(EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetClassName(IntPtr hWnd, StringBuilder sb, int max);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] static extern int GetWindowText(IntPtr hWnd, StringBuilder sb, int max);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L; public int T; public int R; public int B; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X; public int Y; }
  [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
  [DllImport("user32.dll")] public static extern IntPtr PostMessage(IntPtr h, uint msg, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr after, int x, int y, int cx, int cy, uint flags);
  [DllImport("user32.dll")] public static extern IntPtr WindowFromPoint(POINT p);
  public static string WindowAt(int x, int y) {
    POINT p; p.X = x; p.Y = y;
    IntPtr h = WindowFromPoint(p);
    if (h == IntPtr.Zero) return "none";
    var cls = new StringBuilder(256); GetClassName(h, cls, 256);
    var txt = new StringBuilder(512); GetWindowText(h, txt, 512);
    return cls + "|" + txt + "|0x" + h.ToInt64().ToString("X");
  }
  public static bool IsForeground(IntPtr h) { return GetForegroundWindow() == h; }
  public static void TopMost(IntPtr h) { SetWindowPos(h, new IntPtr(-1), 0, 0, 0, 0, 0x0001 | 0x0002); }
  public static void Normal(IntPtr h) { SetWindowPos(h, new IntPtr(-2), 0, 0, 0, 0, 0x0001 | 0x0002); }
  [DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT p);
  [DllImport("user32.dll")] public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, IntPtr extra);
  [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, IntPtr dwExtraInfo);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  public static IntPtr ForceForeground(IntPtr h) {
    IntPtr prev = GetForegroundWindow();
    PostMessage(h, 0x0006, (IntPtr)1, h);              // WM_ACTIVATE / WA_ACTIVE
    PostMessage(h, 0x001C, (IntPtr)1, IntPtr.Zero);    // WM_ACTIVATEAPP TRUE
    PostMessage(h, 0x0007, h, IntPtr.Zero);            // WM_SETFOCUS
    keybd_event(0x12, 0, 0, IntPtr.Zero);              // ALT down (foreground kilidini acar)
    SetForegroundWindow(h);
    keybd_event(0x12, 0, 2, IntPtr.Zero);              // ALT up
    System.Threading.Thread.Sleep(400);
    return prev;
  }
  public static int[] Cursor() { POINT p; GetCursorPos(out p); return new int[] { p.X, p.Y }; }
  public static void Click(int x, int y) {
    SetCursorPos(x, y);
    System.Threading.Thread.Sleep(250);
    mouse_event(0x0002, 0, 0, 0, IntPtr.Zero); // LEFTDOWN
    System.Threading.Thread.Sleep(120);
    mouse_event(0x0004, 0, 0, 0, IntPtr.Zero); // LEFTUP
    System.Threading.Thread.Sleep(120);
  }
  public static string Rect(IntPtr h) { RECT r; if (!GetWindowRect(h, out r)) return ""; return r.L + "," + r.T + "," + r.R + "," + r.B; }
  public static bool PW(IntPtr h, IntPtr hdc, uint flags) { return PrintWindow(h, hdc, flags); }
  public static int[] ScreenPoint(IntPtr h, int cx, int cy) {
    RECT w; GetWindowRect(h, out w);
    POINT p; p.X = 0; p.Y = 0; ClientToScreen(h, ref p);
    return new int[] { p.X + cx, p.Y + cy };
  }
  public static int[] ClientInfo(IntPtr h) {
    RECT w; GetWindowRect(h, out w);
    POINT p; p.X = 0; p.Y = 0; ClientToScreen(h, ref p);
    RECT c; GetClientRect(h, out c);
    return new int[] { p.X - w.L, p.Y - w.T, c.R, c.B };
  }
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
      RECT r; GetWindowRect(h, out r);
      list.Add(cls + "|" + txt + "|0x" + h.ToInt64().ToString("X") + "|" + r.L + "," + r.T + "," + r.R + "," + r.B);
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
    $rect = [W22]::Rect($h)
    if ($rect -eq '') { return $false }
    $p = $rect -split ','
    $w = [int]$p[2] - [int]$p[0]; $hh = [int]$p[3] - [int]$p[1]
    if ($w -le 0 -or $hh -le 0 -or $w -gt 8000 -or $hh -gt 8000) { return $false }
    $bmp = New-Object System.Drawing.Bitmap $w, $hh
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    $ok = [W22]::PW($h, $hdc, 2)
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
$clicks = New-Object System.Collections.ArrayList
$shotSet = $ShotTimes -split ',' | ForEach-Object { [double]$_ }
$clickSet = $ClickTimes -split ',' | ForEach-Object { [double]$_ }
$fracList = @()
foreach ($f in ($ClickFrac -split ';')) {
  $pp = $f -split ','
  $fracList += ,@([double]$pp[0], [double]$pp[1])
}
$done = @{}
$seen = @{}
$t0 = Get-Date
$proc = Start-Process -FilePath $exe -WorkingDirectory $ClientDir -PassThru
$mainPid = $proc.Id
$winLogged = $false
$netstatCount = 0

while (((Get-Date) - $t0).TotalSeconds -lt $ObserveSeconds) {
  Start-Sleep -Milliseconds 100
  $el = [math]::Round(((Get-Date) - $t0).TotalSeconds, 2)

  try { $tc = [System.Net.NetworkInformation.IPGlobalProperties]::GetIPGlobalProperties().GetActiveTcpConnections() }
  catch { $tc = @() }
  foreach ($c in $tc) {
    $lp = $c.LocalEndPoint.Port; $rp = $c.RemoteEndPoint.Port
    $tag = ''
    if ($rp -ge 63000 -and $rp -le 63003) { $tag = 'to-local-server' }
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

  $netstatCount++
  $hwnd = [W22]::MainHwnd([uint32]$mainPid)

  if (-not $winLogged -and $hwnd -ne 0) {
    $wins = [W22]::Windows([uint32]$mainPid)
    $ci = [W22]::ClientInfo([IntPtr]$hwnd)
    [void]$windowLog.Add([pscustomobject]@{ t = $el; windows = $wins; clientOffset = ($ci[0].ToString() + ',' + $ci[1]); clientSize = ($ci[2].ToString() + 'x' + $ci[3]) })
    $winLogged = $true
  }

  foreach ($ct in $clickSet) {
    if ($el -ge $ct -and -not $done.ContainsKey("c$ct")) {
      $done["c$ct"] = $true
      if ($hwnd -ne 0) {
        $idx = [array]::IndexOf($clickSet, $ct)
        if ($idx -lt 0) { $idx = 0 }
        if ($idx -ge $fracList.Count) { $idx = $fracList.Count - 1 }
        $fx = $fracList[$idx][0]; $fy = $fracList[$idx][1]
        $ci = [W22]::ClientInfo([IntPtr]$hwnd)
        $cx = [int]($ci[2] * $fx); $cy = [int]($ci[3] * $fy)
        if ($RealInput) {
          [W22]::TopMost([IntPtr]$hwnd)
          Start-Sleep -Milliseconds 150
          $prevFg = [W22]::ForceForeground([IntPtr]$hwnd)
          $fgOk = [W22]::IsForeground([IntPtr]$hwnd)
          $orig = [W22]::Cursor()
          $sp = [W22]::ScreenPoint([IntPtr]$hwnd, $cx, $cy)
          $sx = $sp[0]; $sy = $sp[1]
          $atPoint = [W22]::WindowAt($sx, $sy)
          [W22]::Click($sx, $sy)
          $atAfter = [W22]::WindowAt([W22]::Cursor()[0], [W22]::Cursor()[1])
          [void]$clicks.Add([pscustomobject]@{ t = $el; clientX = $cx; clientY = $cy; screenX = $sx; screenY = $sy; frac = $ClickFrac; mode = 'realInput'; foreground = $fgOk; windowAtPoint = $atPoint; cursorAfter = $atAfter })
          Start-Sleep -Milliseconds 200
          [void][W22]::SetCursorPos($orig[0], $orig[1])
          if ($prevFg -ne [IntPtr]::Zero) { [void][W22]::SetForegroundWindow($prevFg) }
          [W22]::Normal([IntPtr]$hwnd)
        } else {
          [void][W22]::SetForegroundWindow([IntPtr]$hwnd)
          Start-Sleep -Milliseconds 300
          $lparam = [IntPtr](($cy -shl 16) -bor ($cx -band 0xFFFF))
          [void][W22]::PostMessage([IntPtr]$hwnd, 0x0201, [IntPtr]1, $lparam)  # WM_LBUTTONDOWN
          Start-Sleep -Milliseconds 120
          [void][W22]::PostMessage([IntPtr]$hwnd, 0x0202, [IntPtr]0, $lparam)  # WM_LBUTTONUP
          [void]$clicks.Add([pscustomobject]@{ t = $el; clientX = $cx; clientY = $cy; frac = $ClickFrac; mode = 'postMessage' })
        }
      }
    }
  }

  foreach ($st in $shotSet) {
    if ($el -ge $st -and -not $done.ContainsKey("t$st")) {
      $done["t$st"] = $true
      if ($hwnd -ne 0) {
        $pth = Join-Path $ShotDir ($Label + "-t" + ([int]$st) + "s.png")
        if (GrabHwnd $hwnd $pth) { [void]$shots.Add($pth) }
      }
    }
  }

  if ($proc.HasExited) { break }
}

$exitedEarly = $proc.HasExited
try { Stop-Process -Id $mainPid -Force -ErrorAction SilentlyContinue } catch {}
Start-Sleep -Milliseconds 500

$result = [pscustomobject]@{
  label = $Label
  exe = $exe
  observeSeconds = $ObserveSeconds
  rootPid = $mainPid
  exitedEarly = $exitedEarly
  clickConfig = $ClickFrac
  clicks = @($clicks)
  tcpEvents = @($events)
  windowLog = @($windowLog)
  screenshots = @($shots)
}
$outPath = Join-Path $OutDir ($Label + '.json')
$result | ConvertTo-Json -Depth 8 | Out-File -FilePath $outPath -Encoding UTF8
Write-Output ("DONE " + $Label + " events=" + $events.Count + " clicks=" + $clicks.Count + " shots=" + $shots.Count + " json=" + $outPath)
