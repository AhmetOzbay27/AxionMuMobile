# ps_stackwalk.ps1 - StackWalk64 tabanli cagri zinciri dokumu (32-bit WOW64 hedef).
# Kullanim: powershell -File ps_stackwalk.ps1 -TargetPid 1234 [-Exe ...\ClientFile\Main.exe] [-SymDir ...\ClientFile]
param(
  [Parameter(Mandatory=$true)][int]$TargetPid,
  [string]$Exe = 'C:\Axion Mu Source\ClientFile\Main.exe',
  [string]$SymDir = 'C:\Axion Mu Source\ClientFile',
  [int]$MaxFrames = 32
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class SW {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenThread(uint access, bool inherit, int tid);
  [DllImport("kernel32.dll")] public static extern uint SuspendThread(IntPtr h);
  [DllImport("kernel32.dll")] public static extern int ResumeThread(IntPtr h);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool Wow64GetThreadContext(IntPtr h, IntPtr ctx);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr p, IntPtr addr, IntPtr buf, IntPtr size, out IntPtr read);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymInitialize(IntPtr p, string searchPath, bool invade);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern ulong SymLoadModuleEx(IntPtr p, IntPtr hFile, string image, string module, ulong baseAddr, uint size, IntPtr data, uint flags);
  [DllImport("dbghelp.dll", SetLastError=true)] public static extern bool SymFromAddr(IntPtr p, ulong addr, out ulong disp, IntPtr symInfo);
  [DllImport("dbghelp.dll")] public static extern uint SymSetOptions(uint o);
  [DllImport("dbghelp.dll")] public static extern ulong SymGetModuleBase64(IntPtr p, ulong addr);
  [DllImport("dbghelp.dll")] public static extern IntPtr SymFunctionTableAccess64(IntPtr p, ulong addr);

  public delegate bool ReadProc(IntPtr hProcess, ulong addr, IntPtr buf, uint size, out uint read);
  public delegate IntPtr FuncTableProc(IntPtr hProcess, ulong addr);
  public delegate ulong GetModuleBaseProc(IntPtr hProcess, ulong addr);
  public delegate bool TranslateProc(IntPtr hProcess, IntPtr hThread, out ulong addr);

  [DllImport("dbghelp.dll", SetLastError=true)]
  public static extern bool StackWalk64(uint machine, IntPtr hProcess, IntPtr hThread, IntPtr frame, IntPtr ctx,
    ReadProc readFn, FuncTableProc ftaFn, GetModuleBaseProc gmbFn, TranslateProc transFn);

  public static ReadProc ReadFn = new ReadProc(ReadImpl);
  public static FuncTableProc FtaFn = new FuncTableProc(FtaImpl);
  public static GetModuleBaseProc GmbFn = new GetModuleBaseProc(GmbImpl);
  public static bool ReadImpl(IntPtr hp, ulong addr, IntPtr buf, uint size, out uint read) {
    IntPtr got;
    bool ok = ReadProcessMemory(hp, (IntPtr)(long)addr, buf, (IntPtr)size, out got);
    read = (uint)got.ToInt64();
    return ok;
  }
  public static IntPtr FtaImpl(IntPtr hp, ulong addr) { return SymFunctionTableAccess64(hp, addr); }
  public static ulong GmbImpl(IntPtr hp, ulong addr) { return SymGetModuleBase64(hp, addr); }
  public static string Sym(IntPtr p, ulong addr) {
    IntPtr buf = Marshal.AllocHGlobal(1024);
    try {
      Marshal.WriteInt32(buf, 0, 88);
      Marshal.WriteInt32(buf, 76, 0);
      Marshal.WriteInt32(buf, 80, 512);
      ulong disp;
      if (SymFromAddr(p, addr, out disp, buf)) {
        int nameLen = Marshal.ReadInt32(buf, 76);
        return Marshal.PtrToStringAnsi((IntPtr)(buf.ToInt64() + 84), nameLen) + "+0x" + disp.ToString("x");
      }
      return null;
    } finally { Marshal.FreeHGlobal(buf); }
  }
  // STACKFRAME64 (64-bit layout) yardimcilari
  public static void SetAddr(IntPtr frame, int off, ulong pc, ushort seg, uint mode) {
    Marshal.WriteInt64(frame, off, (long)pc);
    Marshal.WriteInt16(frame, off + 8, (short)seg);
    Marshal.WriteInt32(frame, off + 12, (int)mode);
  }
  public static ulong GetAddr(IntPtr frame, int off) { return (ulong)Marshal.ReadInt64(frame, off); }
}
"@

$hp = [SW]::OpenProcess(0x410, $false, $TargetPid)
if ($hp -eq [IntPtr]::Zero) { Write-Output 'OpenProcess FAIL'; exit 1 }
[void][SW]::SymSetOptions(0x2)
[void][SW]::SymInitialize($hp, $SymDir, $false)
[void][SW]::SymLoadModuleEx($hp, [IntPtr]::Zero, $Exe, $null, 0x400000, 0x9EAD000, [IntPtr]::Zero, 0)

$threads = (Get-Process -Id $TargetPid).Threads | Sort-Object Id
foreach ($t in $threads) {
  $ht = [SW]::OpenThread(0x0002 -bor 0x0008, $false, $t.Id)
  if ($ht -eq [IntPtr]::Zero) { continue }
  try {
    [void][SW]::SuspendThread($ht)
    $ctx = [Runtime.InteropServices.Marshal]::AllocHGlobal(0x400)
    [Runtime.InteropServices.Marshal]::WriteInt32($ctx, 0, 0x10007)
    if ([SW]::Wow64GetThreadContext($ht, $ctx)) {
      $eip = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB8)
      $esp = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xC4)
      $ebp = [uint32][Runtime.InteropServices.Marshal]::ReadInt32($ctx, 0xB4)
      Write-Output ('=== tid=' + $t.Id + ' eip=0x' + $eip.ToString('X8') + ' esp=0x' + $esp.ToString('X8') + ' ebp=0x' + $ebp.ToString('X8') + ' ===')
      $frame = [Runtime.InteropServices.Marshal]::AllocHGlobal(0x800)
      for ($i = 0; $i -lt 0x800; $i += 8) { [Runtime.InteropServices.Marshal]::WriteInt64($frame, $i, 0) }
      [SW]::SetAddr($frame, 0x00, ([uint64]$eip), 0, 3)   # AddrPC
      [SW]::SetAddr($frame, 0x20, ([uint64]$ebp), 0, 3)   # AddrFrame
      [SW]::SetAddr($frame, 0x30, ([uint64]$esp), 0, 3)   # AddrStack
      for ($f = 0; $f -lt $MaxFrames; $f++) {
        $ok = [SW]::StackWalk64(0x14c, $hp, $ht, $frame, $ctx, [SW]::ReadFn, [SW]::FtaFn, [SW]::GmbFn, $null)
        if (-not $ok) { break }
        $pc = [SW]::GetAddr($frame, 0x00)
        if ($pc -eq 0) { break }
        $ret = [SW]::GetAddr($frame, 0x10)
        Write-Output ('  [' + $f + '] pc=0x' + $pc.ToString('X8') + ' ret=0x' + $ret.ToString('X8') + '  ' + [SW]::Sym($hp, $pc))
        if ($f -gt 0 -and $pc -eq 0) { break }
      }
      [Runtime.InteropServices.Marshal]::FreeHGlobal($frame)
    }
    [Runtime.InteropServices.Marshal]::FreeHGlobal($ctx)
  } finally { [void][SW]::ResumeThread($ht) }
}
