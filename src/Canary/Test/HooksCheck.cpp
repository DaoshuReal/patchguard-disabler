#include <ntddk.h>

#include <Canary/Test/Test.h>

namespace Pgc
{
    bool CanaryTest::CheckHooks(bool* HookedPrepare, bool* HookedFreeze)
    {
        constexpr unsigned long PrepareOffset = 0x108;
        constexpr unsigned long FreezeOffset = 0x1A8;

        if (HookedPrepare != nullptr) {
            *HookedPrepare = false;
        }

        if (HookedFreeze != nullptr) {
            *HookedFreeze = false;
        }

        void* Table = SystemExport(L"HalPrivateDispatchTable", "HalPrivateDispatchTable");

        if (Table == nullptr) {
            return false;
        }

        if (!MmIsAddressValid(Table)) {
            return false;
        }

        if (*static_cast<volatile ULONG*>(Table) < 6) {
            return false;
        }

        void** PrepareSlot = reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + PrepareOffset);
        void** FreezeSlot = reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + FreezeOffset);

        if (!MmIsAddressValid(PrepareSlot) || !MmIsAddressValid(FreezeSlot)) {
            return false;
        }

        void* Prepare = *PrepareSlot;
        void* Freeze = *FreezeSlot;

        bool PrepareHooked = Prepare != nullptr && !InsideNt(Prepare);
        bool FreezeHooked = Freeze != nullptr && !InsideNt(Freeze);

        if (HookedPrepare != nullptr) {
            *HookedPrepare = PrepareHooked;
        }

        if (HookedFreeze != nullptr) {
            *HookedFreeze = FreezeHooked;
        }

        return true;
    }
}
