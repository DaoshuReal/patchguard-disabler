#include <ntddk.h>

#include <Driver/Resolve/Resolve.h>

namespace Pgd
{
    void* Resolver::SystemExport(const wchar_t* Name)
    {
        UNICODE_STRING NameValue;

        RtlInitUnicodeString(&NameValue, Name);

        return MmGetSystemRoutineAddress(&NameValue);
    }
}
