#include "BinanceWebSocketClient.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <thread>
#endif

namespace {

void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(static_cast<DWORD>(ms));
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

std::string toLower(std::string value) {
    for (char& c : value) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return value;
}

#ifdef _WIN32
std::string getCwd() {
    char buf[MAX_PATH];
    const DWORD n = GetCurrentDirectoryA(MAX_PATH, buf);
    if (n == 0 || n >= MAX_PATH) {
        return ".";
    }
    return std::string(buf);
}

std::string joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) {
        return file;
    }
    const char last = dir.back();
    if (last == '\\' || last == '/') {
        return dir + file;
    }
    return dir + "\\" + file;
}
#endif

bool launchStreamScript(const std::string& scriptPath,
                        const std::string& symbol,
                        int durationSeconds,
                        const std::string& outFile) {
#ifdef _WIN32
    const std::string cwd = getCwd();
    const std::string absScript = joinPath(cwd, scriptPath);
    const std::string absOut = joinPath(cwd, outFile);
    const std::string absLog = joinPath(cwd, "athena_ws_launch.log");

    const std::string cmd =
        "powershell.exe -NoProfile -ExecutionPolicy Bypass -File \"" + absScript +
        "\" -Symbol \"" + symbol + "\" -Seconds " + std::to_string(durationSeconds) +
        " -OutFile \"" + absOut + "\" > \"" + absLog + "\" 2>&1";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string mutableCmd = "cmd.exe /C " + cmd;
    const BOOL ok = CreateProcessA(
        nullptr,
        mutableCmd.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        cwd.c_str(),
        &si,
        &pi);

    if (!ok) {
        std::cerr << "BinanceWebSocketClient: CreateProcess failed, GetLastError="
                  << GetLastError() << "\n";
        return false;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    std::cout << "BinanceWebSocketClient: launched WS bridge -> " << absOut << "\n";
    return true;
#else
    (void)scriptPath;
    (void)symbol;
    (void)durationSeconds;
    (void)outFile;
    std::cerr << "BinanceWebSocketClient: Windows PowerShell bridge only in v1\n";
    return false;
#endif
}

}  // namespace

int BinanceWebSocketClient::streamTrades(const std::string& symbol,
                                         int durationSeconds,
                                         int maxTrades,
                                         const MessageHandler& onMessage,
                                         const std::string& scriptPath) {
    if (symbol.empty() || durationSeconds <= 0 || maxTrades <= 0 || !onMessage) {
        return 0;
    }

    const std::string outFile = "athena_binance_ws_" + toLower(symbol) + ".jsonl";
    std::remove(outFile.c_str());

    if (!launchStreamScript(scriptPath, symbol, durationSeconds, outFile)) {
        return 0;
    }

    sleepMs(1500);

    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(durationSeconds + 3);

    int delivered = 0;
    std::uint64_t byteOffset = 0;

    while (std::chrono::steady_clock::now() < deadline && delivered < maxTrades) {
        // Open → copy new bytes → close immediately.
        // Holding the file open blocks PowerShell Add-Content on Windows.
        std::string data;
        {
            std::ifstream in(outFile, std::ios::binary);
            if (in) {
                in.seekg(static_cast<std::streamoff>(byteOffset));
                data.assign(std::istreambuf_iterator<char>(in),
                            std::istreambuf_iterator<char>());
            }
        }

        std::size_t start = 0;
        while (delivered < maxTrades) {
            const std::size_t nl = data.find('\n', start);
            if (nl == std::string::npos) {
                break;  // incomplete trailing line — wait for more data
            }

            std::string line = data.substr(start, nl - start);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (!line.empty()) {
                if (static_cast<unsigned char>(line[0]) == 0xEF && line.size() >= 3) {
                    line = line.substr(3);
                }
                onMessage(line);
                ++delivered;
                if (delivered == 1) {
                    std::cout << "BinanceWebSocketClient: first live trade received\n";
                }
            }

            start = nl + 1;
        }

        byteOffset += static_cast<std::uint64_t>(start);
        sleepMs(80);
    }

    if (delivered == 0) {
        std::cerr << "BinanceWebSocketClient: no lines in " << outFile
                  << " (see athena_ws_launch.log if present)\n";
    }

    return delivered;
}
