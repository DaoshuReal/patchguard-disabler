#pragma once

#include <ntddk.h>

extern "C" PLIST_ENTRY PsLoadedModuleList;

namespace Pgc
{
    class NtRange
    {
    public:
        static void Initialize();
        static void* Base;
        static unsigned long Size;
    };

    class DriverLifecycle
    {
    public:
        static void Unload(PDRIVER_OBJECT DriverObject);
    };
}
