#include <ntddk.h>

#include <Canary/Core/Core.h>

namespace Pgc
{
    void DriverLifecycle::Unload(PDRIVER_OBJECT DriverObject)
    {
        UNREFERENCED_PARAMETER(DriverObject);
    }
}
