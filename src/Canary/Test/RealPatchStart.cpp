#include <ntddk.h>

#include <Canary/Test/Test.h>

namespace Pgc
{
    void CanaryTest::StartRealPatch()
    {
        HANDLE ThreadHandle = nullptr;

        NTSTATUS Status = PsCreateSystemThread(&ThreadHandle, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, PatchThread, nullptr);

        if (NT_SUCCESS(Status)) {
            DbgPrint("[Pgc] Real patch thread launched\n");
        }

        if (ThreadHandle != nullptr) {
            ZwClose(ThreadHandle);
        }
    }
}
