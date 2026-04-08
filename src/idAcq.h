#ifndef BPUAC_IDACQ_H
#define BPUAC_IDACQ_H
#include <string>
#include <windows.h>

namespace core {
    DWORD GetProcessID(const std::wstring& processName);

} // core

#endif //BPUAC_IDACQ_H
