#pragma once

#include <ntddk.h>

namespace Pgc
{
    class CanaryTest
    {
    public:
        static bool CheckHooks(bool* HookedPrepare, bool* HookedFreeze);
        static void FireSynthetic();
        static void StartRealPatch();
    private:
        static void* SystemExport(const wchar_t* Name, const char* Tag);
        static bool InsideNt(void* Pointer);
        static void PatchThread(void* StartContext);
        static bool WritePointer(void** Slot, void* Value);
    };
}
