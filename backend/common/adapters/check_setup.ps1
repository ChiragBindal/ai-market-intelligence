# Quick health check - run this first if demos show no output
$ErrorActionPreference = "Continue"

Write-Host "=== ATHENA setup check ===" -ForegroundColor Cyan
Write-Host "Folder: $PSScriptRoot"
Write-Host ""

function Find-CurlExe {
    $cmd = Get-Command curl.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    return $null
}

function Check($name) {
    $cmd = Get-Command $name -ErrorAction SilentlyContinue
    if ($cmd) {
        Write-Host "[OK]  $name -> $($cmd.Source)" -ForegroundColor Green
        return $true
    }
    Write-Host "[FAIL] $name not in PATH" -ForegroundColor Red
    return $false
}

$gpp = Check "g++"
$curlExe = Find-CurlExe
if ($curlExe) {
    Write-Host "[OK]  curl.exe -> $curlExe" -ForegroundColor Green
} else {
    Write-Host "[FAIL] curl.exe not in PATH" -ForegroundColor Red
    Write-Host "Tip: PowerShell alias curl is NOT real curl. Use curl.exe" -ForegroundColor Yellow
}
Check "bash"

Write-Host ""
if ($null -ne $curlExe) {
    Write-Host "Testing Binance API (curl.exe)..." -ForegroundColor Yellow
    $tmp = Join-Path $PSScriptRoot "api_probe.json"
    $url = "https://api.binance.com/api/v3/trades?symbol=BTCUSDT&limit=1"
    & $curlExe -s $url -o $tmp
    $ok = ($LASTEXITCODE -eq 0) -and (Test-Path $tmp) -and ((Get-Item $tmp).Length -gt 2)
    if ($ok) {
        Write-Host "[OK]  Binance API returned data:" -ForegroundColor Green
        Get-Content $tmp
        Remove-Item $tmp -ErrorAction SilentlyContinue
    } else {
        Write-Host "[FAIL] Binance API - no data" -ForegroundColor Red
    }
}

Write-Host ""
if ($gpp) {
    $exe = Join-Path $PSScriptRoot "adapter_tests.exe"
    if (Test-Path $exe) {
        Write-Host "Found $exe - running..." -ForegroundColor Yellow
        & $exe
    } else {
        Write-Host "No adapter_tests.exe yet. Run: .\run_tests.ps1" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "=== End check ===" -ForegroundColor Cyan
