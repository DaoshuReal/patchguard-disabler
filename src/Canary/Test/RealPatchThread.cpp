#include <ntddk.h>

#include <Canary/Test/Test.h>

namespace Pgc
{
    void CanaryTest::PatchThread(void* StartContext)
    {
        UNREFERENCED_PARAMETER(StartContext);

        LARGE_INTEGER Delay;

        Delay.QuadPart = -5LL * 10LL * 1000LL * 1000LL;

        KeDelayExecutionThread(KernelMode, FALSE, &Delay);

        void** Table = static_cast<void**>(SystemExport(L"HalDispatchTable", "HalDispatchTable"));

        if (Table == nullptr) {
            return;
        }

        if (!MmIsAddressValid(Table) || !MmIsAddressValid(Table + 1) || !MmIsAddressValid(Table + 2)) {
            return;
        }

        void** FirstSlot = Table + 1;
        void** SecondSlot = Table + 2;

        void* SavedFirst = *FirstSlot;
        void* SavedSecond = *SecondSlot;

        if (SavedFirst == nullptr || SavedSecond == nullptr) {
            return;
        }

        if (!WritePointer(FirstSlot, SavedSecond) || !WritePointer(SecondSlot, SavedFirst)) {
            return;
        }

        DbgPrint("[Pgc] Real patch armed\n");

        Delay.QuadPart = -480LL * 10LL * 1000LL * 1000LL;

        KeDelayExecutionThread(KernelMode, FALSE, &Delay);

        WritePointer(FirstSlot, SavedFirst);
        WritePointer(SecondSlot, SavedSecond);

        DbgPrint("[Pgc] Real patch restored\n");

        PsTerminateSystemThread(STATUS_SUCCESS);
    }
}
