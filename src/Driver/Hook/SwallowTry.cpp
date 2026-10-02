#include <ntddk.h>
#include <intrin.h>

#include <Driver/Resolve/Resolve.h>
#include <Driver/Hook/Hook.h>

namespace Pgd
{
    bool Swallow::TrySwallow109Guarded()
    {
        constexpr unsigned long long PcrMajorOffset = 0x60;
        constexpr unsigned long BugCheckPatchGuard = 0x109;

        if (!Resolver::Ready) {
            return false;
        }

        if (Resolver::Anchors.Active == nullptr || Resolver::Anchors.HardwareTrigger == nullptr) {
            return false;
        }

        if (Resolver::Layout.ContextOffset == 0 || Resolver::Layout.DebugIrqlOffset == 0) {
            return false;
        }

        unsigned char* Prcb = reinterpret_cast<unsigned char*>(__readgsqword(0x20));

        if (Prcb == nullptr || !MmIsAddressValid(Prcb)) {
            return false;
        }

        void** ContextSlot = reinterpret_cast<void**>(Prcb + Resolver::Layout.ContextOffset);

        if (!MmIsAddressValid(ContextSlot)) {
            return false;
        }

        CONTEXT* BugContext = static_cast<CONTEXT*>(*ContextSlot);

        if (!Probe::RangeMapped(BugContext, sizeof(CONTEXT))) {
            return false;
        }

        unsigned long Code = static_cast<unsigned long>(BugContext->Rcx);

        if (Code != BugCheckPatchGuard) {
            return false;
        }

        unsigned long long TrapRsp = BugContext->Rsp;
        unsigned long TrapFlags = BugContext->EFlags;

        if (!MmIsAddressValid(static_cast<PVOID>(const_cast<LONG*>(Resolver::Anchors.Active))) ||
            !MmIsAddressValid(static_cast<PVOID>(const_cast<LONG*>(Resolver::Anchors.HardwareTrigger)))) {
            return false;
        }

        *Resolver::Anchors.Active = 0;
        *Resolver::Anchors.HardwareTrigger = 0;

        if (Resolver::Layout.IpiFrozenValid) {
            volatile ULONG* FrozenSlot = reinterpret_cast<volatile ULONG*>(Prcb + Resolver::Layout.IpiFrozenOffset);

            if (MmIsAddressValid(const_cast<ULONG*>(FrozenSlot))) {
                *FrozenSlot = 5;
            }
        }

        UCHAR PreviousMajor = __readgsbyte(PcrMajorOffset);

        PgWriteGsByte(PcrMajorOffset, 0x7A);

        KIRQL SavedIrql;

        if (Resolver::Layout.DebugIrqlIsPcr) {
            SavedIrql = __readgsbyte(Resolver::Layout.DebugIrqlOffset);
        } else {
            volatile UCHAR* IrqlSlot = reinterpret_cast<volatile UCHAR*>(Prcb + Resolver::Layout.DebugIrqlOffset);

            if (!MmIsAddressValid(const_cast<UCHAR*>(IrqlSlot))) {
                PgWriteGsByte(PcrMajorOffset, PreviousMajor);

                return false;
            }

            SavedIrql = *IrqlSlot;
        }

        if ((TrapFlags & 0x200) != 0) {
            PgWriteCr8(SavedIrql >= DISPATCH_LEVEL ? SavedIrql : DISPATCH_LEVEL);
            PgSti();
        } else {
            PgWriteCr8(HIGH_LEVEL);
        }

        CONTEXT* Target = Swallow::FindContext(TrapRsp);

        if (Target == nullptr) {
            PgWriteGsByte(PcrMajorOffset, PreviousMajor);

            return false;
        }

        PgWriteGsByte(PcrMajorOffset, PreviousMajor);

        Swallow::Continue(Target, SavedIrql);

        return true;
    }

    bool Swallow::TrySwallow109()
    {
        __try {
            return TrySwallow109Guarded();
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
}
