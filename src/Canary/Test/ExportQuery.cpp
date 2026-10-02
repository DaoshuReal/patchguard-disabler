#include <ntddk.h>

#include <Canary/Core/Core.h>
#include <Canary/Test/Test.h>

namespace Pgc
{
    void* CanaryTest::SystemExport(const wchar_t* Name, const char* Tag)
    {
        UNREFERENCED_PARAMETER(Tag);

        UNICODE_STRING NameValue;

        RtlInitUnicodeString(&NameValue, Name);

        void* Export = MmGetSystemRoutineAddress(&NameValue);

        if (Export != nullptr && !MmIsAddressValid(Export)) {
            return nullptr;
        }

        return Export;
    }
}
