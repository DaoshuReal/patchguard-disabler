#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    bool Probe::RangeMapped(void* Pointer, unsigned long Size)
    {
        if (Pointer == nullptr || Size == 0) {
            return false;
        }

        unsigned char* Begin = static_cast<unsigned char*>(Pointer);
        unsigned char* End = Begin + (Size - 1);

        if (End < Begin) {
            return false;
        }

        return MmIsAddressValid(Begin) && MmIsAddressValid(End);
    }
}
