#include <ntddk.h>

#include <Driver/Core/Core.h>
#include <Driver/Resolve/Resolve.h>
#include <Driver/Hook/Hook.h>

namespace Pgd
{
    extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
    {
        UNREFERENCED_PARAMETER(RegistryPath);

        NtRange::Initialize();

        if (NtRange::Base == nullptr) {
            return STATUS_UNSUCCESSFUL;
        }

        if (DriverObject) {
            if (DriverObject) {
            DriverObject->DriverUnload = DriverLifecycle::Unload;
        }
        }

        if (!Resolver::ResolveAll() || !Resolver::Ready) {
            return STATUS_UNSUCCESSFUL;
        }

        if (Resolver::Table.Version < 6 || Resolver::Table.Version > 0x100) {
            return STATUS_UNSUCCESSFUL;
        }

        if (!HalHook::Install(Resolver::Table.Table)) {
            return STATUS_UNSUCCESSFUL;
        }

        DbgPrint("[Pgd] Armed\n");

        return STATUS_SUCCESS;
    }
}
