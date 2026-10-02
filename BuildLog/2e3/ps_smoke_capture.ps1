param(
  [string]$OutDir = "$PSScriptRoot\results",
  [string]$Label = 'smoke1'
)
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$names = 'ConnectServer','DataServer','JoinServer','GameServer'
$procs = @()
foreach ($n in $names) {
  foreach ($p in (Get-CimInstance Win32_Process -Filter "Name='$n.exe'")) {
    $procs += [pscustomobject]@{
      name = $n
      pid = $p.ProcessId
      path = $p.ExecutablePath
      started = "$($p.CreationDate)"
    }
  }
}
$ports = (netstat -ano | Select-String '6300[0-3]') | ForEach-Object { $_.ToString().Trim() }

$obj = [pscustomobject]@{
  label = $Label
  time = (Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
  processes = $procs
  ports = $ports
}
$obj | ConvertTo-Json -Depth 5 | Set-Content -Encoding UTF8 (Join-Path $OutDir "$Label.json")
Write-Host "yazildi: $OutDir\$Label.json ; surec sayisi: $($procs.Count)"
$procs | Format-Table name, pid -AutoSize | Out-String | Write-Host
