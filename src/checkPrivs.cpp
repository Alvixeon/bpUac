#include "checkPrivs.h"

#include <cstdio>
#include <windows.h>

#include "idAcq.h"
#include <sddl.h>


/*
 *
 * This function is intended to check the current elevation level via token acquisition attempts.
 * program flow is as follows
 * 1. Attempt to open the current process with the desired access of TOKEN_QUERY
 * 2. If this fails then goto cleanup, no privs
 * 3. Attempt to create another handle to the token and check the current elevation
 * 4. If this fails, goto cleanup, no privs
 * 5. If everything succeeds, return true
 *
 * X. Cleanup closes the handle to the token and resets the value back to nullptr
 */
namespace core {
    bool IsSystem() {
        BOOL isSystem = FALSE;
        PSID systemSid = nullptr;
        HANDLE hToken = nullptr;

        if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken)) {
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
                return false;
        }

        if (ConvertStringSidToSidW(L"S-1-5-18", &systemSid)) {
            CheckTokenMembership(hToken, systemSid, &isSystem);
            LocalFree(systemSid);
        }

        if (hToken)
            CloseHandle(hToken);

        return isSystem == TRUE;
    }

    bool isElevated() {
        BOOL isElevated = FALSE;
        HANDLE hToken = nullptr;
        TOKEN_ELEVATION elevation;
        DWORD dSize;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            printf("\n [!] Failed to get process token: %lu", GetLastError());
            goto Cleanup;
        }
        if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dSize)) {
            printf("\n[!] Failed to get Token Information :%lu.", GetLastError());
            goto Cleanup; // if Failed, we treat as False
        }
        isElevated = elevation.TokenIsElevated;

    Cleanup:
        if (hToken) {
            CloseHandle(hToken);
            hToken = nullptr;
        }
        return isElevated;
    }
} // core
