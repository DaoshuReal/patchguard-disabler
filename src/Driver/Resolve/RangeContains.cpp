#include <ntddk.h>

#include <Driver/Core/Core.h>
#include <Driver/Resolve/Resolve.h>

namespace Pgd
{
    bool Resolver::RangeContains(void* Pointer, unsigned long Need)
    {
        if (Pointer == nullptr || Need == 0 || NtRange::Base == nullptr || NtRange::Size == 0) {
            return false;
        }

        ULONG_PTR Base = reinterpret_cast<ULONG_PTR>(NtRange::Base);
        ULONG_PTR Value = reinterpret_cast<ULONG_PTR>(Pointer);

        return Value >= Base && Need <= NtRange::Size && Value + Need >= Value && Value + Need <= Base + NtRange::Size;
    }
}
