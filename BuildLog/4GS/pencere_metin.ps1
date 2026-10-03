param([int]$ProcId)
Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class W32 {
  public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr hWnd, EnumProc cb, IntPtr lParam);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr hWnd, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr hWnd, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint pid);
}
"@
$target = [uint32]$ProcId
$found = @()
$cb = [W32+EnumProc]{
  param($h, $l)
  $pid2 = [uint32]0
  [void][W32]::GetWindowThreadProcessId($h, [ref]$pid2)
  if ($pid2 -eq $target) {
    $t = New-Object System.Text.StringBuilder 512
    [void][W32]::GetWindowText($h, $t, 512)
    $c = New-Object System.Text.StringBuilder 256
    [void][W32]::GetClassName($h, $c, 256)
    $script:found += [pscustomobject]@{ Hwnd=$h; Class=$c.ToString(); Text=$t.ToString() }
  }
  return $true
}
[void][W32]::EnumWindows($cb, [IntPtr]::Zero)
foreach ($w in $found) {
  Write-Output ("[TOP ] cls={0} text={1}" -f $w.Class, $w.Text)
  $kids = @()
  $kb = [W32+EnumProc]{
    param($h2, $l2)
    $t2 = New-Object System.Text.StringBuilder 512
    [void][W32]::GetWindowText($h2, $t2, 512)
    $c2 = New-Object System.Text.StringBuilder 256
    [void][W32]::GetClassName($h2, $c2, 256)
    $script:kids += [pscustomobject]@{ Hwnd=$h2; Class=$c2.ToString(); Text=$t2.ToString() }
    return $true
  }
  [void][W32]::EnumChildWindows($w.Hwnd, $kb, [IntPtr]::Zero)
  foreach ($k in $kids) {
    if ($k.Text -ne "") { Write-Output ("  [CHLD] cls={0} text={1}" -f $k.Class, $k.Text) }
  }
}
