#include "impToken.h"
#include <windows.h>
#include <iostream>


/*
 *
 * Responsible for elevating the current process tokens given admin rights
 *
 */


namespace core {
    // Helper function to set privileges
    BOOL SetPrivilege(const HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
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

    void it() {
        HANDLE hToken;
        // Open current process token
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            // Enable SeDebugPrivilege
            if (SetPrivilege(hToken, SE_DEBUG_NAME, TRUE)) {
                std::cout << "SeDebugPrivilege enabled successfully." << std::endl;
            }
            CloseHandle(hToken);
        }
    }
} // core
