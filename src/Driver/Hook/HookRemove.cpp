#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    void HalHook::Remove()
    {
        constexpr unsigned long PrepareOffset = 0x108;
        constexpr unsigned long FreezeOffset = 0x1A8;

        if (Table != nullptr && OriginalPrepare != nullptr && MmIsAddressValid(static_cast<unsigned char*>(Table) + PrepareOffset)) {
            PhysMem::WritePointer(reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + PrepareOffset), OriginalPrepare);
        }

        if (Table != nullptr && OriginalFreeze != nullptr && MmIsAddressValid(static_cast<unsigned char*>(Table) + FreezeOffset)) {
            PhysMem::WritePointer(reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + FreezeOffset), OriginalFreeze);
        }
    }
}
