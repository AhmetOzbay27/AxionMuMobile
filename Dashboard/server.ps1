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
$DataDir    = Join-Path $PSScriptRoot "data"
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
        elseif ($path -eq "/api/timeline") {
            # Zaman çizelgesi: git log'dan canlı okunur (hash + dd.MM.yyyy HH:mm + mesaj)
            $lines = & $GitExe -C $RepoRoot log --pretty=format:"%h|%ad|%s" --date=format:"%d.%m.%Y %H:%M" -40 2>$null
            $tl = @()
            foreach ($ln in $lines) {
                $p = $ln -split "\|", 2
                if ($p.Count -eq 2) {
                    $p2 = $p[1] -split "\|", 2
                    if ($p2.Count -eq 2) { $tl += @{ hash = $p[0]; date = $p2[0]; msg = $p2[1] } }
                }
            }
            Send-Response $ctx 200 "application/json; charset=utf-8" (ConvertTo-Json $tl -Depth 4)
        }
        elseif ($path -eq "/api/agent") {
            $o = Get-Content (Join-Path $DataDir "oneriler.json") -Raw -Encoding UTF8 | ConvertFrom-Json
            $k = Get-Content (Join-Path $DataDir "komut.json") -Raw -Encoding UTF8 | ConvertFrom-Json
            $s = Get-Content (Join-Path $DataDir "sonuc.json") -Raw -Encoding UTF8 | ConvertFrom-Json
            $so = Get-Content (Join-Path $DataDir "sohbet.json") -Raw -Encoding UTF8 | ConvertFrom-Json
            $obj = @{ suggestions = $o.suggestions; queue = $k.queue; history = $k.history; sonuc = $s; sohbet = $so.entries }
            Send-Response $ctx 200 "application/json; charset=utf-8" (ConvertTo-Json $obj -Depth 6)
        }
        elseif ($path -eq "/api/cmd" -and $ctx.Request.HttpMethod -eq "POST") {
            $body = (New-Object System.IO.StreamReader($ctx.Request.InputStream, [System.Text.Encoding]::UTF8)).ReadToEnd()
            $req = $body | ConvertFrom-Json
            $pinFile = Join-Path $DataDir "pin.txt"
            $pinOk = $false
            if (Test-Path $pinFile) {
                $pinOk = ("$($req.pin)" -eq (Get-Content $pinFile -Raw).Trim())
            }
            if (-not $pinOk) {
                Send-Response $ctx 403 "application/json; charset=utf-8" '{"ok":false,"error":"PIN yanlis"}'
            }
            elseif ([string]::IsNullOrWhiteSpace($req.text) -or $req.text.Length -gt 500) {
                Send-Response $ctx 400 "application/json; charset=utf-8" '{"ok":false,"error":"Komut bos veya 500 karakterden uzun"}'
            }
            else {
                $kf = Join-Path $DataDir "komut.json"
                $k = Get-Content $kf -Raw -Encoding UTF8 | ConvertFrom-Json
                if ($null -eq $k.queue) { $k | Add-Member -NotePropertyName queue -NotePropertyValue @() }
                $text = [string]$req.text
                if ($text.Length -gt 500) { $text = $text.Substring(0,500) }
                $entry = @{ text = $text; ts = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss"); id = [guid]::NewGuid().ToString("N").Substring(0,8) }
                $k.queue = @($k.queue) + $entry
                $k.updated = $entry.ts
                Set-Content -Path $kf -Value (ConvertTo-Json $k -Depth 6) -Encoding UTF8
                Send-Response $ctx 200 "application/json; charset=utf-8" '{"ok":true}'
            }
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
