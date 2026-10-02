#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    void* HalHook::Table = nullptr;

    void* HalHook::OriginalPrepare = nullptr;

    void* HalHook::OriginalFreeze = nullptr;

    bool HalHook::Install(void* TableValue)
    {
        constexpr unsigned long PrepareOffset = 0x108;
        constexpr unsigned long FreezeOffset = 0x1A8;

        Table = TableValue;

        void** PrepareSlot = reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + PrepareOffset);
        void** FreezeSlot = reinterpret_cast<void**>(static_cast<unsigned char*>(Table) + FreezeOffset);

        if (!MmIsAddressValid(PrepareSlot) || !MmIsAddressValid(FreezeSlot)) {
            return false;
        }

        void* SavedPrepare = *PrepareSlot;
        void* SavedFreeze = *FreezeSlot;

        if (!MmIsAddressValid(SavedPrepare) || !MmIsAddressValid(SavedFreeze)) {
            return false;
        }

        OriginalPrepare = SavedPrepare;
        OriginalFreeze = SavedFreeze;

        if (SavedPrepare != static_cast<void*>(HookHandler::Prepare) && !PhysMem::WritePointer(PrepareSlot, static_cast<void*>(HookHandler::Prepare))) {
            return false;
        }

        if (SavedFreeze != static_cast<void*>(HookHandler::NotifyFreeze) && !PhysMem::WritePointer(FreezeSlot, static_cast<void*>(HookHandler::NotifyFreeze))) {
            PhysMem::WritePointer(PrepareSlot, SavedPrepare);

            return false;
        }

        void* ReadPrepare = *PrepareSlot;
        void* ReadFreeze = *FreezeSlot;

        if (ReadPrepare != static_cast<void*>(HookHandler::Prepare) || ReadFreeze != static_cast<void*>(HookHandler::NotifyFreeze)) {
            Remove();

            return false;
        }

        return true;
    }
}
