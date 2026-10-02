#include <ntddk.h>

#include <Driver/Core/Core.h>
#include <Driver/Nt/NtTypes.h>

namespace Pgd
{
    void* NtRange::Base = nullptr;

    unsigned long NtRange::Size = 0;

    void NtRange::Initialize()
    {
        if (Base != nullptr) {
            return;
        }

        if (PsLoadedModuleList == nullptr) {
            return;
        }

        UNICODE_STRING CompareName;

        RtlInitUnicodeString(&CompareName, L"ntoskrnl.exe");

        for (PLIST_ENTRY Entry = PsLoadedModuleList->Flink; Entry != PsLoadedModuleList; Entry = Entry->Flink) {
            PKLDR_DATA_TABLE_ENTRY LdrEntry = CONTAINING_RECORD(Entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

            if (!MmIsAddressValid(LdrEntry) || !MmIsAddressValid(LdrEntry->DllBase)) {
                continue;
            }

            if (!MmIsAddressValid(&LdrEntry->BaseDllName) || LdrEntry->BaseDllName.Buffer == nullptr ||
                !MmIsAddressValid(LdrEntry->BaseDllName.Buffer)) {
                continue;
            }

            if (RtlCompareUnicodeString(&CompareName, &LdrEntry->BaseDllName, TRUE) == 0) {
                Base = LdrEntry->DllBase;
                Size = LdrEntry->SizeOfImage;

                break;
            }
        }
    }
}
