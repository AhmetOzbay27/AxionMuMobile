# ps_code_scan.ps1 - donmus surecin yiginini Main.exe .text (kod) araligina gore tarar.
# Kullanim: powershell -File ps_code_scan.ps1 -TargetPid 1234 [-Exe ...] [-SymDir ...]
param(
  [Parameter(Mandatory=$true)][int]$TargetPid,
  [string]$Exe = 'C:\Axion Mu Source\ClientFile\Main.exe',
  [string]$SymDir = 'C:\Axion Mu Source\ClientFile',
  [uint32]$CodeLo = 0x401000,
  [uint32]$CodeHi = 0x4A525C,
  [int]$Back = 0x8000,
  [int]$Fwd = 0x20000
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class Sc {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenThread(uint access, bool inherit, int tid);
  [DllImport("kernel32.dll")] public static extern uint SuspendThread(IntPtr h);
  [DllImport("kernel32.dll")] public static extern int ResumeThread(IntPtr h);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool Wow64GetThreadContext(IntPtr h, IntPtr ctx);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr p, IntPtr addr, byte[] buf, int size, out int read);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymInitialize(IntPtr p, string searchPath, bool invade);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern ulong SymLoadModuleEx(IntPtr p, IntPtr hFile, string image, string module, ulong baseAddr, uint size, IntPtr data, uint flags);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymFromAddr(IntPtr p, ulong addr, out ulong disp, IntPtr symInfo);
  [DllImport("dbghelp.dll")] public static extern uint SymSetOptions(uint o);
  public static string Sym(IntPtr p, ulong addr) {
    IntPtr buf = Marshal.AllocHGlobal(1024);
    try {
      Marshal.WriteInt32(buf, 0, 88);
      Marshal.WriteInt32(buf, 76, 0);
      Marshal.WriteInt32(buf, 80, 512);
      ulong disp;
      if (SymFromAddr(p, addr, out disp, buf)) {
        int nameLen = Marshal.ReadInt32(buf, 76);
        string s = Marshal.PtrToStringAnsi((IntPtr)(buf.ToInt64() + 84), nameLen);
        return s + "+0x" + disp.ToString("x");
      }
      return null;
    } finally { Marshal.FreeHGlobal(buf); }
  }
}
"@
$hp = [Sc]::OpenProcess(0x410, $false, $TargetPid)
[void][Sc]::SymSetOptions(0x6)
[void][Sc]::SymInitialize($hp, $SymDir, $false)
[void][Sc]::SymLoadModuleEx($hp, [IntPtr]::Zero, $Exe, $null, 0x400000, 0x9EAD000, [IntPtr]::Zero, 0)
$threads = (Get-Process -Id $TargetPid).Threads | Sort-Object Id
foreach ($t in $threads) {
  $ht = [Sc]::OpenThread(0x0002 -bor 0x0008, $false, $t.Id)
  if ($ht -eq [IntPtr]::Zero) { continue }
  try {
    [void][Sc]::SuspendThread($ht)
    $ctx = [Runtime.InteropServices.Marshal]::AllocHGlobal(0x400)
    [Runtime.InteropServices.Marshal]::WriteInt32($ctx, 0, 0x10007)
    if ([Sc]::Wow64GetThreadContext($ht, $ctx)) {
      $eip = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB8)
      $esp = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xC4)
      Write-Output ('=== tid=' + $t.Id + ' eip=0x' + $eip.ToString('X8') + ' esp=0x' + $esp.ToString('X8'))
      $lo = [uint32]($esp - $Back)
      $len = $Back + $Fwd
      $buf = New-Object byte[] $len
      for ($c = 0; $c -lt $len; $c += 4096) {
        $tmp = New-Object byte[] 4096
        $got = 0
        if ([Sc]::ReadProcessMemory($hp, [IntPtr][int]($lo + $c), $tmp, 4096, [ref]$got)) { [Array]::Copy($tmp, 0, $buf, $c, 4096) }
      }
      $hits = @()
      for ($o = 0; $o -lt $len - 4; $o += 4) {
        $v = [uint32]([BitConverter]::ToUInt32($buf, $o))
        if ($v -ge $CodeLo -and $v -lt $CodeHi) { $hits += [pscustomobject]@{ off = [uint32]$lo + $o; addr = $v } }
      }
      Write-Output ('  kod araligindaki adres sayisi=' + $hits.Count)
      foreach ($h in $hits) {
        Write-Output ('    stk=0x' + ([uint64]$h.off).ToString('X8') + ' (esp' + (',' + ([int64]$h.off - [int64]$esp).ToString('+0;-0')) + ') 0x' + ([uint64]$h.addr).ToString('X8') + ' rva=0x' + (([uint64]$h.addr) - 0x400000).ToString('X') + '  ' + [Sc]::Sym($hp, [uint64]$h.addr))
      }
    }
    [Runtime.InteropServices.Marshal]::FreeHGlobal($ctx)
  } finally { [void][Sc]::ResumeThread($ht) }
}
