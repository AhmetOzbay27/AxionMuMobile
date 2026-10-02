# ps_stack.ps1 - donmus bir 32-bit (WOW64) surecin yiginini okuyup modul+sembole cozer.
# Kullanim: powershell -File ps_stack.ps1 -TargetPid 1234 [-Exe C:\...\ClientFile\Main.exe] [-SymDir C:\Axion Mu Source\ClientFile]
param(
  [Parameter(Mandatory=$true)][int]$TargetPid,
  [string]$Exe = 'C:\Axion Mu Source\ClientFile\Main.exe',
  [string]$SymDir = 'C:\Axion Mu Source\ClientFile',
  [int]$StackBytes = 262144
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
[StructLayout(LayoutKind.Sequential)]
public struct MODULEINFO { public IntPtr baseOfDll; public uint sizeOfImage; public IntPtr entryPoint; }
public class Dbg {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenThread(uint access, bool inherit, int tid);
  [DllImport("kernel32.dll")] public static extern uint SuspendThread(IntPtr h);
  [DllImport("kernel32.dll")] public static extern int ResumeThread(IntPtr h);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool Wow64GetThreadContext(IntPtr h, IntPtr ctx);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr p, IntPtr addr, byte[] buf, int size, out int read);
  [DllImport("psapi.dll")] public static extern bool EnumProcessModulesEx(IntPtr p, IntPtr[] mods, int cb, out int needed, uint filter);
  [DllImport("psapi.dll", CharSet=CharSet.Unicode)] public static extern uint GetModuleFileNameEx(IntPtr p, IntPtr mod, StringBuilder sb, int n);
  [DllImport("psapi.dll")] public static extern bool GetModuleInformation(IntPtr p, IntPtr mod, out MODULEINFO mi, int cb);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymInitialize(IntPtr p, string searchPath, bool invade);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern ulong SymLoadModuleEx(IntPtr p, IntPtr hFile, string image, string module, ulong baseAddr, uint size, IntPtr data, uint flags);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymFromAddr(IntPtr p, ulong addr, out ulong disp, IntPtr symInfo);
  [DllImport("dbghelp.dll")] public static extern uint SymSetOptions(uint o);
  public static string Sym(IntPtr p, ulong addr) {
    IntPtr buf = Marshal.AllocHGlobal(1024);
    try {
      Marshal.WriteInt32(buf, 0, 88);   // SizeOfStruct
      Marshal.WriteInt32(buf, 76, 0);   // NameLen (out)
      Marshal.WriteInt32(buf, 80, 512); // MaxNameLen
      ulong disp;
      if (SymFromAddr(p, addr, out disp, buf)) {
        int nameLen = Marshal.ReadInt32(buf, 76);  // NameLen
        string s = Marshal.PtrToStringAnsi((IntPtr)(buf.ToInt64() + 84), nameLen);
        return s + "+0x" + disp.ToString("x");
      }
      return null;
    } finally { Marshal.FreeHGlobal(buf); }
  }
}
"@

$hp = [Dbg]::OpenProcess(0x410, $false, $TargetPid)   # VM_READ | QUERY_INFORMATION
if ($hp -eq [IntPtr]::Zero) { Write-Output ('OpenProcess FAIL err=' + [Runtime.InteropServices.Marshal]::GetLastWin32Error()); exit 1 }

# modul listesi
$mods = New-Object 'IntPtr[]' 512
$needed = 0
[void][Dbg]::EnumProcessModulesEx($hp, $mods, 4096, [ref]$needed, 0x01)  # LIST_MODULES_32BIT
$ranges = @()
for ($i = 0; $i -lt [int]($needed / [IntPtr]::Size); $i++) {
  $mi = New-Object MODULEINFO
  [void][Dbg]::GetModuleInformation($hp, $mods[$i], [ref]$mi, [Runtime.InteropServices.Marshal]::SizeOf([type][MODULEINFO]))
  $sb = New-Object System.Text.StringBuilder 512
  [void][Dbg]::GetModuleFileNameEx($hp, $mods[$i], $sb, 512)
  $ranges += [pscustomobject]@{ base = [uint64]$mi.baseOfDll.ToInt64(); size = [uint64]$mi.sizeOfImage; name = (Split-Path $sb.ToString() -Leaf); path = $sb.ToString() }
}
$main = $ranges | Where-Object { $_.name -ieq (Split-Path $Exe -Leaf) } | Select-Object -First 1
Write-Output ('modul sayisi=' + $ranges.Count + ' ana modul=' + ($main | ConvertTo-Json -Compress))

# dbghelp
[void][Dbg]::SymSetOptions(0x6)  # UNDNAME | DEFERRED_LOADS
[void][Dbg]::SymInitialize($hp, $SymDir, $false)
if ($main) { [void][Dbg]::SymLoadModuleEx($hp, [IntPtr]::Zero, $Exe, $null, $main.base, [uint32]$main.size, [IntPtr]::Zero, 0) }

$threads = (Get-Process -Id $TargetPid).Threads | Sort-Object Id
foreach ($t in $threads) {
  $ht = [Dbg]::OpenThread(0x0002 -bor 0x0008, $false, $t.Id)   # SUSPEND_RESUME | GET_CONTEXT
  if ($ht -eq [IntPtr]::Zero) { Write-Output ('tid=' + $t.Id + ' OpenThread FAIL'); continue }
  try {
    [void][Dbg]::SuspendThread($ht)
    $ctx = [Runtime.InteropServices.Marshal]::AllocHGlobal(0x400)
    [Runtime.InteropServices.Marshal]::WriteInt32($ctx, 0, 0x10007)   # CONTEXT_FULL
    if (-not [Dbg]::Wow64GetThreadContext($ht, $ctx)) {
      Write-Output ('tid=' + $t.Id + ' GetThreadContext FAIL err=' + [Runtime.InteropServices.Marshal]::GetLastWin32Error())
    } else {
      $eip = [Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB8)
      $esp = [Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xC4)
      $ebp = [Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB4)
      Write-Output ('--- tid=' + $t.Id + ' eip=0x' + $eip.ToString('X8') + ' esp=0x' + $esp.ToString('X8') + ' ebp=0x' + $ebp.ToString('X8') + ' ---')
      $start = [uint32]($esp - 0x2000)
      $buf = New-Object byte[] $StackBytes
      $okBytes = 0
      for ($c = 0; $c -lt $StackBytes; $c += 4096) {
        $tmp = New-Object byte[] 4096
        $got = 0
        if ([Dbg]::ReadProcessMemory($hp, [IntPtr][int]($start + $c), $tmp, 4096, [ref]$got)) {
          [Array]::Copy($tmp, 0, $buf, $c, 4096); $okBytes += 4096
        }
      }
      Write-Output ('  okunabilen yigin bayti=' + $okBytes)
      if ($okBytes -gt 0) {
        $read = $StackBytes
        $found = @()
        for ($o = 0; $o -lt $read - 4; $o += 4) {
          $v = [uint32]([BitConverter]::ToUInt32($buf, $o))
          if ($v -lt 0x10000) { continue }
          foreach ($r in $ranges) {
            if ($v -ge $r.base -and $v -lt ($r.base + $r.size)) {
              $found += [pscustomobject]@{ off = ($start + $o); addr = $v; mod = $r.name; rva = ($v - $r.base) }
              break
            }
          }
          if ($found.Count -ge 200) { break }
        }
        $i = 0
        foreach ($f in $found) {
          $sym = ''
          if ($f.mod -ieq (Split-Path $Exe -Leaf)) { $sym = [Dbg]::Sym($hp, [uint64]$f.addr) }
          Write-Output ('  [' + $i + '] off=0x' + ([uint64]$f.off).ToString('X8') + ' addr=0x' + ([uint64]$f.addr).ToString('X8') + ' ' + $f.mod + '+0x' + ([uint64]$f.rva).ToString('X') + '  ' + $sym)
          $i++
          if ($i -ge 40) { Write-Output ('  ... toplam ' + $found.Count + ' adres'); break }
        }
      } else { Write-Output '  yigin okunamadi' }
    }
    [Runtime.InteropServices.Marshal]::FreeHGlobal($ctx)
  } finally {
    [void][Dbg]::ResumeThread($ht)
  }
}
