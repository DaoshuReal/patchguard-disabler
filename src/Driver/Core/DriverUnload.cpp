#include <ntddk.h>

#include <Driver/Core/Core.h>
#include <Driver/Hook/Hook.h>

namespace Pgd
{
    void DriverLifecycle::Unload(PDRIVER_OBJECT DriverObject)
    {
        UNREFERENCED_PARAMETER(DriverObject);

        HalHook::Remove();
    }
}
