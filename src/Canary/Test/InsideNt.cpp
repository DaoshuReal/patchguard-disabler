#include <ntddk.h>

#include <Canary/Core/Core.h>
#include <Canary/Test/Test.h>

namespace Pgc
{
    bool CanaryTest::InsideNt(void* Pointer)
    {
        return Pointer != nullptr && NtRange::Base != nullptr && NtRange::Size != 0 && Pointer >= NtRange::Base &&
            Pointer < static_cast<unsigned char*>(NtRange::Base) + NtRange::Size;
    }
}
