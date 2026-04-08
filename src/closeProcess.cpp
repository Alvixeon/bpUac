#include "closeProcess.h"
#include <iostream>
#include <windows.h>
#include "idAcq.h"

namespace core {
    void closeProcess(const std::wstring& processName) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, core::GetProcessID((processName)));
        if (hProcess != NULL) {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
        }else {
            std::cout << "[!] Failed to open process\n";
        }
    }

} // core