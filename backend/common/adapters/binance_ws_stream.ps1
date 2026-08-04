# Binance public trade WebSocket -> JSONL file (one trade JSON per line).
# Used by BinanceWebSocketClient.cpp so MinGW g++ does not need a C++ WebSocket library.
param(
    [string]$Symbol = "BTCUSDT",
    [int]$Seconds = 15,
    [string]$OutFile = "athena_binance_ws.jsonl"
)

$ErrorActionPreference = "Stop"

$streamSymbol = $Symbol.ToLower()
$uri = [Uri]("wss://stream.binance.com:9443/ws/{0}@trade" -f $streamSymbol)

if (Test-Path $OutFile) {
    Remove-Item $OutFile -Force -ErrorAction SilentlyContinue
}

# Share Read so the C++ poller can open the file while we append.
$fileStream = [System.IO.File]::Open(
    $OutFile,
    [System.IO.FileMode]::Create,
    [System.IO.FileAccess]::Write,
    [System.IO.FileShare]::ReadWrite
)
$writer = New-Object System.IO.StreamWriter($fileStream, [System.Text.UTF8Encoding]::new($false))
$writer.AutoFlush = $true

$ws = New-Object System.Net.WebSockets.ClientWebSocket
$cts = New-Object System.Threading.CancellationTokenSource
try {
    $ws.ConnectAsync($uri, $cts.Token).Wait()
    if ($ws.State -ne [System.Net.WebSockets.WebSocketState]::Open) {
        throw "WebSocket failed to open for $Symbol"
    }

    $buffer = New-Object byte[] 16384
    $segment = New-Object System.ArraySegment[byte] -ArgumentList @(, $buffer)
    $deadline = (Get-Date).AddSeconds($Seconds)

    while ((Get-Date) -lt $deadline -and $ws.State -eq [System.Net.WebSockets.WebSocketState]::Open) {
        $receiveTask = $ws.ReceiveAsync($segment, $cts.Token)
        if (-not $receiveTask.Wait(1000)) {
            continue
        }
        $result = $receiveTask.Result
        if ($result.MessageType -eq [System.Net.WebSockets.WebSocketMessageType]::Close) {
            break
        }
        if ($result.Count -le 0) {
            continue
        }
        $msg = [System.Text.Encoding]::UTF8.GetString($buffer, 0, $result.Count)
        $writer.WriteLine($msg)
    }
}
finally {
    if ($ws.State -eq [System.Net.WebSockets.WebSocketState]::Open) {
        try {
            $ws.CloseAsync(
                [System.Net.WebSockets.WebSocketCloseStatus]::NormalClosure,
                "done",
                $cts.Token
            ).Wait(2000) | Out-Null
        } catch {}
    }
    $ws.Dispose()
    if ($writer) { $writer.Dispose() }
    if ($fileStream) { $fileStream.Dispose() }
}
