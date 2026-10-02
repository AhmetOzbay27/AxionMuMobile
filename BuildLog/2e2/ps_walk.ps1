# ps_walk.ps1 - EBP zinciri yürüyüşü + dbghelp sembol çözümü (32-bit WOW64 hedef).
# Kullanim: powershell -File ps_walk.ps1 -TargetPid 1234 [-Exe ...\ClientFile\Main.exe] [-SymDir ...\ClientFile]
param(
  [Parameter(Mandatory=$true)][int]$TargetPid,
  [string]$Exe = 'C:\Axion Mu Source\ClientFile\Main.exe',
  [string]$SymDir = 'C:\Axion Mu Source\ClientFile',
  [int]$MaxFrames = 40
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
[StructLayout(LayoutKind.Sequential)]
public struct MEMORY_BASIC_INFORMATION {
  public IntPtr BaseAddress; public IntPtr AllocationBase; public uint AllocationProtect;
  public IntPtr RegionSize; public uint State; public uint Protect; public uint Type;
}
[StructLayout(LayoutKind.Sequential)]
public struct MODULEINFO { public IntPtr baseOfDll; public uint sizeOfImage; public IntPtr entryPoint; }
public class Wk {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenThread(uint access, bool inherit, int tid);
  [DllImport("kernel32.dll")] public static extern uint SuspendThread(IntPtr h);
  [DllImport("kernel32.dll")] public static extern int ResumeThread(IntPtr h);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool Wow64GetThreadContext(IntPtr h, IntPtr ctx);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr p, IntPtr addr, byte[] buf, int size, out int read);
  [DllImport("kernel32.dll")] public static extern IntPtr VirtualQueryEx(IntPtr p, IntPtr addr, out MEMORY_BASIC_INFORMATION mbi, IntPtr len);
  [DllImport("psapi.dll")] public static extern bool EnumProcessModulesEx(IntPtr p, IntPtr[] mods, int cb, out int needed, uint filter);
  [DllImport("psapi.dll", CharSet=CharSet.Unicode)] public static extern uint GetModuleFileNameEx(IntPtr p, IntPtr mod, StringBuilder sb, int n);
  [DllImport("psapi.dll")] public static extern bool GetModuleInformation(IntPtr p, IntPtr mod, out MODULEINFO mi, int cb);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymInitialize(IntPtr p, string searchPath, bool invade);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern ulong SymLoadModuleEx(IntPtr p, IntPtr hFile, string image, string module, ulong baseAddr, uint size, IntPtr data, uint flags);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymFromAddr(IntPtr p, ulong addr, out ulong disp, IntPtr symInfo);
  [DllImport("dbghelp.dll")] public static extern uint SymSetOptions(uint o);
  public static uint ReadU32(IntPtr p, uint addr) {
    byte[] b = new byte[4]; int got = 0;
    if (!ReadProcessMemory(p, (IntPtr)(int)addr, b, 4, out got) || got != 4) return 0;
    return BitConverter.ToUInt32(b, 0);
  }
  public static bool Readable(IntPtr p, uint addr) {
    byte[] b = new byte[1]; int got = 0;
    return ReadProcessMemory(p, (IntPtr)(int)addr, b, 1, out got) && got == 1;
  }
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

$hp = [Wk]::OpenProcess(0x410, $false, $TargetPid)
if ($hp -eq [IntPtr]::Zero) { Write-Output 'OpenProcess FAIL'; exit 1 }
$mods = New-Object 'IntPtr[]' 512
$needed = 0
[void][Wk]::EnumProcessModulesEx($hp, $mods, 4096, [ref]$needed, 0x01)
$ranges = @()
for ($i = 0; $i -lt [int]($needed / [IntPtr]::Size); $i++) {
  $mi = New-Object MODULEINFO
  [void][Wk]::GetModuleInformation($hp, $mods[$i], [ref]$mi, [Runtime.InteropServices.Marshal]::SizeOf([type][MODULEINFO]))
  $sb = New-Object System.Text.StringBuilder 512
  [void][Wk]::GetModuleFileNameEx($hp, $mods[$i], $sb, 512)
  $ranges += [pscustomobject]@{ base = [uint64]$mi.baseOfDll.ToInt64(); size = [uint64]$mi.sizeOfImage; name = (Split-Path $sb.ToString() -Leaf) }
}
function ModOf([uint64]$addr) {
  foreach ($r in $ranges) { if ($addr -ge $r.base -and $addr -lt ($r.base + $r.size)) { return ($r.name + '+0x' + ($addr - $r.base).ToString('X')) } }
  return '?'
}
[void][Wk]::SymSetOptions(0x6)
[void][Wk]::SymInitialize($hp, $SymDir, $false)
$mainMod = $ranges | Where-Object { $_.base -eq 0x400000 } | Select-Object -First 1
if ($mainMod) { [void][Wk]::SymLoadModuleEx($hp, [IntPtr]::Zero, $Exe, $null, $mainMod.base, [uint32]([uint64]$mainMod.size -band 0xFFFFFFFF), [IntPtr]::Zero, 0) }
Write-Output ('modul=' + ($ranges | Where-Object { $_.base -eq 0x400000 } | ForEach-Object { $_.name + ' base=0x' + $_.base.ToString('X') + ' size=0x' + $_.size.ToString('X') }))

$threads = (Get-Process -Id $TargetPid).Threads | Sort-Object Id
foreach ($t in $threads) {
  $ht = [Wk]::OpenThread(0x0002 -bor 0x0008, $false, $t.Id)
  if ($ht -eq [IntPtr]::Zero) { continue }
  try {
    [void][Wk]::SuspendThread($ht)
    $ctx = [Runtime.InteropServices.Marshal]::AllocHGlobal(0x400)
    [Runtime.InteropServices.Marshal]::WriteInt32($ctx, 0, 0x10007)
    if ([Wk]::Wow64GetThreadContext($ht, $ctx)) {
      $eip = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB8)
      $esp = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xC4)
      $ebp = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB4)
      Write-Output ('=== tid=' + $t.Id + ' eip=0x' + $eip.ToString('X8') + ' [' + (ModOf $eip) + '] ' + [Wk]::Sym($hp, $eip) + ' esp=0x' + $esp.ToString('X8') + ' ebp=0x' + $ebp.ToString('X8'))
      # 1) esp civari ham dword'ler (ilk frame icin ipucu)
      $start = [uint32]($esp)
      for ($o = 0; $o -lt 64; $o += 4) {
        $v = [Wk]::ReadU32($hp, $start + $o)
        if ($v -ge 0x400000 -and $v -lt 0x10000000) {
          $r = ModOf $v
          if ($r -ne '?') { Write-Output ('   esp+0x' + $o.ToString('X2') + ' -> 0x' + $v.ToString('X8') + ' ' + $r + ' ' + [Wk]::Sym($hp, $v)) }
        }
      }
      # 2) EBP zinciri
      $e = $ebp
      for ($f = 0; $f -lt $MaxFrames; $f++) {
        $next = [Wk]::ReadU32($hp, $e)
        $ret = [Wk]::ReadU32($hp, $e + 4)
        if ($ret -lt 0x10000) { break }
        $r = ModOf $ret
        Write-Output ('   frame[' + $f + '] ebp=0x' + $e.ToString('X8') + ' ret=0x' + $ret.ToString('X8') + ' ' + $r + '  ' + [Wk]::Sym($hp, $ret))
        if ($next -le $e -or ($next -band 3) -ne 0 -or ($next - $e) -gt 0x40000) { break }
        if (-not [Wk]::Readable($hp, $next + 4)) { break }
        $e = $next
      }
    }
    [Runtime.InteropServices.Marshal]::FreeHGlobal($ctx)
  } finally { [void][Wk]::ResumeThread($ht) }
}
