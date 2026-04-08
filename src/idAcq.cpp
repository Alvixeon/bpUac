#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sddl.h>

#include "impToken.h"

namespace core {
    DWORD GetProcessID(const std::wstring& processName) {
        PROCESSENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (processName == entry.szExeFile) {
                    CloseHandle(snapshot);
                    printf("[*] Process ID found: %lu.\n", entry.th32ProcessID);
                    return entry.th32ProcessID;
                }
            }while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
        return 0;
    }
} // core