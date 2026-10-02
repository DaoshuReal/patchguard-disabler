#pragma once

#include <ntddk.h>

namespace Pgd
{
    struct HalTable
    {
        void* Table;
        unsigned long Version;
    };

    struct BugcheckAnchors
    {
        volatile LONG* Active;
        volatile LONG* HardwareTrigger;
    };

    struct ProcessorLayout
    {
        unsigned long ContextOffset;
        unsigned long IpiFrozenOffset;
        bool IpiFrozenValid;
        unsigned long DebugIrqlOffset;
        bool DebugIrqlIsPcr;
    };

    class Resolver
    {
    public:
        static bool ResolveAll();
        static HalTable Table;
        static BugcheckAnchors Anchors;
        static ProcessorLayout Layout;
        static bool Ready;
    private:
        static bool RangeContains(void* Pointer, unsigned long Need);
        static void* SystemExport(const wchar_t* Name);
    };
}
