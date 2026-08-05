# ATHENA Binance REST demo - PowerShell
$ErrorActionPreference = "Stop"

Write-Host "=== ATHENA Binance REST demo ===" -ForegroundColor Cyan

$Adapters = $PSScriptRoot
$Common = (Resolve-Path (Join-Path $Adapters "..")).Path
$Backend = (Resolve-Path (Join-Path $Common "..")).Path
$Exchange = Join-Path $Backend "exchange"
$Orderflow = Join-Path $Backend "orderflow"

function Find-Gpp {
    if ($env:ATHENA_GXX -and (Test-Path $env:ATHENA_GXX)) {
        return $env:ATHENA_GXX
    }
    if (Test-Path "C:\MinGW\bin\g++.exe") {
        return "C:\MinGW\bin\g++.exe"
    }
    $cmd = Get-Command g++ -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw "g++ not found."
}

$Gpp = Find-Gpp
Write-Host "Using g++: $Gpp"

$Includes = @(
    "-I$Adapters"
    "-I$(Join-Path $Common 'event')"
    "-I$(Join-Path $Common 'engine')"
    "-I$(Join-Path $Common 'types')"
    "-I$(Join-Path $Exchange 'include/models')"
    "-I$(Join-Path $Exchange 'include/enums')"
    "-I$(Join-Path $Exchange 'include/utility')"
    "-I$(Join-Path $Orderflow 'include')"
)

$Sources = @(
    (Join-Path $Adapters "demo/binance_main.cpp")
    (Join-Path $Adapters "MarketDataAdapter.cpp")
    (Join-Path $Adapters "BinanceAdapter.cpp")
    (Join-Path $Adapters "BinanceJsonParser.cpp")
    (Join-Path $Adapters "BinanceHttpClient.cpp")
    (Join-Path $Adapters "BinanceWebSocketClient.cpp")
    (Join-Path $Common "event/EventDispatcher.cpp")
    (Join-Path $Common "event/TradeEvent.cpp")
    (Join-Path $Exchange "src/Trade.cpp")
    (Join-Path $Orderflow "src/DeltaEngine.cpp")
    (Join-Path $Orderflow "src/FootPrintLevel.cpp")
    (Join-Path $Orderflow "src/FootPrintEngine.cpp")
)

$Out = Join-Path $Adapters "binance_demo.exe"
if (Test-Path $Out) {
    Remove-Item $Out -Force -ErrorAction SilentlyContinue
}

Write-Host "Compiling -> $Out"
& $Gpp -std=c++17 @Includes @Sources -o $Out
if ($LASTEXITCODE -ne 0) { throw "g++ compile failed (exit $LASTEXITCODE)." }

Write-Host "Compile OK. Fetching REST trades..." -ForegroundColor Yellow
& $Out
if ($LASTEXITCODE -ne 0) { throw "Demo failed (exit $LASTEXITCODE)." }

Write-Host "=== Done (binance REST demo) ===" -ForegroundColor Green
