#ifndef BPUAC_IMPTOKEN_H
#define BPUAC_IMPTOKEN_H
#include <windows.h>
#include <iostream>

namespace core {
    bool getSystemPrivs(const std::wstring& targetProcess);
    bool getSEDebug();
    bool SetPrivilege(const HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

} // core

#endif //BPUAC_IMPTOKEN_H
