#include <ntddk.h>

#include <Canary/Core/Core.h>
#include <Canary/Test/Test.h>

namespace Pgc
{
    extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
    {
        UNREFERENCED_PARAMETER(RegistryPath);

        if (DriverObject) {
            DriverObject->DriverUnload = DriverLifecycle::Unload;
        }

        NtRange::Initialize();

        if (NtRange::Base == nullptr) {
            return STATUS_UNSUCCESSFUL;
        }

        bool HookedPrepare = false;
        bool HookedFreeze = false;

        if (!CanaryTest::CheckHooks(&HookedPrepare, &HookedFreeze)) {
            return STATUS_UNSUCCESSFUL;
        }

        if (HookedPrepare && HookedFreeze) {
            DbgPrint("[Pgc] Hooks resident\n");
        } else {
            DbgPrint("[Pgc] Hooks absent\n");
        }

        CanaryTest::StartRealPatch();

        if (!HookedPrepare) {
            return STATUS_SUCCESS;
        }

        CanaryTest::FireSynthetic();

        return STATUS_SUCCESS;
    }
}
