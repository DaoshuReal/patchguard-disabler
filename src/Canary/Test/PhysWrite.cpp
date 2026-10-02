#include <ntddk.h>

#include <Canary/Test/Test.h>

namespace Pgc
{
    bool CanaryTest::WritePointer(void** Slot, void* Value)
    {
        PHYSICAL_ADDRESS Physical = MmGetPhysicalAddress(Slot);

        if (Physical.QuadPart == 0) {
            return false;
        }

        void** Mapped = static_cast<void**>(MmMapIoSpaceEx(Physical, sizeof(void*), PAGE_READWRITE));

        if (Mapped == nullptr) {
            return false;
        }

        *Mapped = Value;

        MmUnmapIoSpace(Mapped, sizeof(void*));

        return true;
    }
}
