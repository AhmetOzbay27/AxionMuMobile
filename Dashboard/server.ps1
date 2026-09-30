# ============================================================
# AXION MU - ILERLEME PANOSU SUNUCUSU (bagimliliksz, PS 5.1+)
# Kullanim: powershell -ExecutionPolicy Bypass -File server.ps1 [-Published 1]
# Port 8096.  -Published 1 -> http://+:8096/ (dis erisim; yonetici gerekir)
#             yoksa        -> http://localhost:8096/ (yalniz yerel)
# ============================================================
param([int]$Published = 0)

$ErrorActionPreference = "Stop"

$RepoRoot   = "C:\Axion Mu Source"
$DocsDir    = Join-Path $RepoRoot "docs"
$WwwDir     = Join-Path $PSScriptRoot "www"
$GitExe     = "C:\Program Files\Git\cmd\git.exe"
if (-not (Test-Path $GitExe)) { $GitExe = "git" }

# --- Dinlenecek adresler ---
$listener = New-Object System.Net.HttpListener
if ($Published -eq 1) {
    $listener.Prefixes.Add("http://+:8096/")        # dis erisim (admin/ACL gerekir)
    Write-Host "[AXION-PANO] DIS erisim: http://45.87.120.29:8096/"
} else {
    $listener.Prefixes.Add("http://localhost:8096/") # yalniz yerel (ACL gerekmez)
}
Write-Host "[AXION-PANO] Yerel: http://localhost:8096/  - Ctrl+C ile durdur"

# --- Yardimcilar ---
function Get-BuildInfo {
    $paths = @{
        "gs_bizim"      = "$RepoRoot\MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe"
        "gs_canli"      = "C:\Axion Mu Mobile\4.MuServer\Sub-1\GameServer\GameServer.exe"
        "main_bizim"    = "$RepoRoot\ClientFile\Main.exe"
        "getmaininfo"   = "$RepoRoot\GetMain\GetMainInfo.exe"
    }
    $out = @{}
    foreach ($k in $paths.Keys) {
        $p = $paths[$k]
        if (Test-Path $p) {
            $f = Get-Item $p
            $out[$k] = @{ size = $f.Length; mtime = $f.LastWriteTime.ToString("yyyy-MM-dd HH:mm") }
        } else { $out[$k] = $null }
    }
    return $out
}

function Get-GitInfo {
    $log  = & $GitExe -C $RepoRoot log --pretty=format:"%h|%ad|%s" --date=format:"%d.%m %H:%M" -15 2>$null
    $commits = @()
    foreach ($l in $log) { $p = $l -split "\|", 2; if ($p.Count -eq 2) { $commits += @{ hash = $p[0]; rest = $p[1] } } }
    $st = & $GitExe -C $RepoRoot status --porcelain 2>$null
    $dirty = @($st | Where-Object { $_ -notmatch "^\?\?" }).Count
    $untracked = @($st | Where-Object { $_ -match "^\?\?" }).Count
    $branch = (& $GitExe -C $RepoRoot rev-parse --abbrev-ref HEAD 2>$null)
    return @{ branch = $branch; commits = $commits; dirty = $dirty; untracked = $untracked }
}

function Get-DiskFree {
    $d = Get-PSDrive C
    return [math]::Round($d.Free / 1GB, 1)
}

function Get-StatusJson {
    $docs = @()
    Get-ChildItem $DocsDir -Filter *.md | Sort-Object Name | ForEach-Object {
        $docs += @{ name = $_.Name; size = $_.Length; mtime = $_.LastWriteTime.ToString("yyyy-MM-dd HH:mm") }
    }
    $obj = @{
        host    = $env:COMPUTERNAME
        time    = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
        git     = Get-GitInfo
        builds  = Get-BuildInfo
        diskGB  = Get-DiskFree
        docs    = $docs
    }
    return ConvertTo-Json $obj -Depth 5
}

function Send-Response($ctx, $status, $ctype, $body) {
    $ctx.Response.StatusCode = $status
    $ctx.Response.ContentType = $ctype
    $ctx.Response.Headers.Add("Cache-Control", "no-store")
    $buf = [System.Text.Encoding]::UTF8.GetBytes($body)
    $ctx.Response.ContentLength64 = $buf.Length
    $ctx.Response.OutputStream.Write($buf, 0, $buf.Length)
    $ctx.Response.OutputStream.Close()
}

# --- Ana dongu ---
$listener.Start()
while ($listener.IsListening) {
    $ctx = $listener.GetContext()
    try {
        $path = $ctx.Request.Url.AbsolutePath
        Write-Host "[ISTEK] $path"

        if ($path -eq "/" -or $path -eq "/index.html") {
            $html = [System.IO.File]::ReadAllText((Join-Path $WwwDir "index.html"))
            Send-Response $ctx 200 "text/html; charset=utf-8" $html
        }
        elseif ($path -eq "/api/status") {
            Send-Response $ctx 200 "application/json; charset=utf-8" (Get-StatusJson)
        }
        elseif ($path -like "/api/doc/*") {
            $name = [System.Uri]::UnescapeDataString($path.Substring(9))
            # Guvenlik: yalniz docs klasorundeki .md dosyalari, yol gecisi yasak
            # Not: Turkce buyuk I (U+0130 + U+0307 bilesen) oldugu icin regex yerine
            # kombine kontrol: .md uzanti + yol gecisi/.. yok + Test-Path dogrulamasi
            $safe = ($name -like '*.md') -and ($name -notmatch '[/\\]') -and ($name -notlike '*..*') -and (Test-Path (Join-Path $DocsDir $name)) -and ((Get-Item (Join-Path $DocsDir $name)).DirectoryName -eq $DocsDir)
            if ($safe) {
                $md = [System.IO.File]::ReadAllText((Join-Path $DocsDir $name))
                Send-Response $ctx 200 "text/plain; charset=utf-8" $md
            } else {
                Send-Response $ctx 404 "text/plain" "doc bulunamadi: $name"
            }
        }
        elseif ($path -like "/api/log/*") {
            $n = 20
            if ($path -match "/api/log/(\d+)") { $n = [math]::Min([int]$Matches[1], 100) }
            $lines = & $GitExe -C $RepoRoot log --pretty=format:"%h %ad %s" --date=format:"%d.%m.%Y %H:%M" -$n 2>$null
            Send-Response $ctx 200 "text/plain; charset=utf-8" ($lines -join "`n")
        }
        elseif ($path -eq "/api/ping") {
            Send-Response $ctx 200 "text/plain" "ok"
        }
        else {
            Send-Response $ctx 404 "text/plain" "404"
        }
    } catch {
        Write-Host "[HATA] $_"
        try { Send-Response $ctx 500 "text/plain" "sunucu hatasi: $_" } catch {}
    }
}
