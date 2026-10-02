# dbgview.ps1 - DBWIN_BUFFER tabanli OutputDebugString yakalayici (Buffy, 2e.2 teshis)
# Kullanim: powershell -File dbgview.ps1 -Seconds 20 [-StartExe <path>]
# Not: bir seferde tek debug monitor calisabilir.
param(
  [int]$Seconds = 20,
  [string]$StartExe = '',
  [string]$WorkDir = ''
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class Dbwin {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr CreateFileMapping(IntPtr hFile, IntPtr lpAttr, uint flProtect, uint maxHigh, uint maxLow, string name);
  [DllImport("kernel32.dll")] public static extern IntPtr MapViewOfFile(IntPtr hMap, uint access, uint offHigh, uint offLow, IntPtr bytes);
  [DllImport("kernel32.dll")] public static extern IntPtr CreateEvent(IntPtr attr, bool manualReset, bool initialState, string name);
  [DllImport("kernel32.dll")] public static extern bool SetEvent(IntPtr h);
  [DllImport("kernel32.dll")] public static extern uint WaitForSingleObject(IntPtr h, uint ms);
  [DllImport("kernel32.dll")] public static extern bool UnmapViewOfFile(IntPtr p);
  public static IntPtr Map, Ready, Data;
  public static byte[] Buf = new byte[4096];
  public static string ReadLatest(out uint pid) {
    pid = (uint)Marshal.ReadInt32(Map);
    Marshal.Copy(Map, Buf, 0, 4096);
    int len = 0; while (len < 4090 && Buf[4+len] != 0) len++;
    return Encoding.Default.GetString(Buf, 4, len);
  }
  public static bool Init() {
    Map = CreateFileMapping((IntPtr)(-1), IntPtr.Zero, 4 /*PAGE_READWRITE*/, 0, 4096, "DBWIN_BUFFER");
    if (Map == IntPtr.Zero) return false;
    Map = MapViewOfFile(Map, 0xF001F /*FILE_MAP_ALL_ACCESS*/, 0, 0, (IntPtr)4096);
    if (Map == IntPtr.Zero) return false;
    Ready = CreateEvent(IntPtr.Zero, false, false, "DBWIN_BUFFER_READY");
    Data  = CreateEvent(IntPtr.Zero, false, false, "DBWIN_DATA_READY");
    return Ready != IntPtr.Zero && Data != IntPtr.Zero;
  }
}
"@

if (-not [Dbwin]::Init()) { Write-Output 'DBWIN init FAIL (baska bir monitor acik olabilir)'; exit 1 }
Write-Output 'DBWIN monitor aktif.'
if ($StartExe -ne '') {
  Write-Output ("baslatiliyor: " + $StartExe)
  $p = Start-Process -FilePath $StartExe -WorkingDirectory $WorkDir -PassThru
  Write-Output ("pid=" + $p.Id)
}
[void][Dbwin]::SetEvent([Dbwin]::Ready)
$deadline = (Get-Date).AddSeconds($Seconds)
$count = 0
while ((Get-Date) -lt $deadline) {
  $r = [Dbwin]::WaitForSingleObject([Dbwin]::Data, 500)
  if ($r -eq 0) {
    $pidOut = [uint32]0
    $msg = [Dbwin]::ReadLatest([ref]$pidOut)
    $count++
    Write-Output ("[" + $pidOut + "] " + $msg.TrimEnd())
    [void][Dbwin]::SetEvent([Dbwin]::Ready)
  }
}
Write-Output ("toplam " + $count + " satir")
