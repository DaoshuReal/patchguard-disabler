#include <ntddk.h>

#include <Driver/Core/Core.h>
#include <Driver/Resolve/Resolve.h>

namespace Pgd
{
    HalTable Resolver::Table = {};

    BugcheckAnchors Resolver::Anchors = {};

    ProcessorLayout Resolver::Layout = {};

    bool Resolver::Ready = false;

    bool Resolver::ResolveAll()
    {
        Table = {};
        Anchors = {};
        Layout = {};
        Ready = false;

        if (NtRange::Base == nullptr || NtRange::Size == 0) {
            return false;
        }

        void* BugCheckEx = SystemExport(L"KeBugCheckEx");

        if (BugCheckEx == nullptr || !RangeContains(BugCheckEx, 0x200)) {
            return false;
        }

        bool FoundContext = false;
        bool FoundIrql = false;
        bool FoundHardwareTrigger = false;
        bool FoundBugActive = false;
        bool FoundIpiFrozen = false;
        bool FoundTable = false;

        unsigned long ContextOffset = 0;
        unsigned long DebugIrqlOffset = 0;
        unsigned long IpiFrozenOffset = 0;
        volatile LONG* BugActive = nullptr;
        volatile LONG* HardwareTrigger = nullptr;
        void* HalTableValue = nullptr;
        unsigned long HalVersion = 0;

        for (unsigned long Index = 1; Index + 8 < 0x100; Index++) {
            unsigned char* Candidate = static_cast<unsigned char*>(BugCheckEx) + Index;

            if (!RangeContains(Candidate, 8)) {
                break;
            }

            if ((Candidate[0] == 0x48 || Candidate[0] == 0x49) && Candidate[1] == 0x8B && Candidate[7] == 0xE8) {
                ContextOffset = *reinterpret_cast<unsigned long*>(Candidate + 3);
                FoundContext = true;

                break;
            }
        }

        for (unsigned long Index = 1; Index + 8 < 0x200; Index++) {
            unsigned char* Candidate = static_cast<unsigned char*>(BugCheckEx) + Index;

            if (!RangeContains(Candidate, 8)) {
                break;
            }

            if (Candidate[0] == 0x65 && Candidate[1] == 0x88 && Candidate[2] == 0x04) {
                DebugIrqlOffset = *reinterpret_cast<unsigned long*>(Candidate + 4);
                FoundIrql = true;

                break;
            }
        }

        for (unsigned long Index = 1; Index + 7 < 0x200; Index++) {
            unsigned char* Candidate = static_cast<unsigned char*>(BugCheckEx) + Index;

            if (!RangeContains(Candidate, 7)) {
                break;
            }

            if (Candidate[0] == 0xF0 && Candidate[1] == 0xFF && Candidate[2] == 0x05) {
                void* Target = Candidate + 3 + 4 + *reinterpret_cast<LONG*>(Candidate + 3);

                if (RangeContains(Target, sizeof(LONG))) {
                    HardwareTrigger = static_cast<volatile LONG*>(Target);
                    FoundHardwareTrigger = true;
                }

                break;
            }
        }

        void* BugCheck2 = nullptr;

        for (unsigned long Index = 1; Index + 5 < 0x200; Index++) {
            unsigned char* Candidate = static_cast<unsigned char*>(BugCheckEx) + Index;

            if (!RangeContains(Candidate, 5)) {
                break;
            }

            if (Candidate[0] != 0xE8) {
                continue;
            }

            void* Target = Candidate + 1 + 4 + *reinterpret_cast<LONG*>(Candidate + 1);

            if ((reinterpret_cast<ULONG_PTR>(Target) & 3) != 0 || !RangeContains(Target, 0x800)) {
                continue;
            }

            for (unsigned long Inner = 0; Inner + 8 < 0x800; Inner++) {
                unsigned char* InnerCandidate = static_cast<unsigned char*>(Target) + Inner;

                if (!RangeContains(InnerCandidate, 8)) {
                    break;
                }

                if (InnerCandidate[0] == 0xF0 && InnerCandidate[1] == 0x0F && InnerCandidate[2] == 0xB1) {
                    void* InnerTarget = InnerCandidate + 4 + 4 + *reinterpret_cast<LONG*>(InnerCandidate + 4);

                    if (RangeContains(InnerTarget, sizeof(LONG))) {
                        BugActive = static_cast<volatile LONG*>(InnerTarget);
                        FoundBugActive = true;
                        BugCheck2 = Target;
                    }

                    break;
                }
            }

            if (BugCheck2 != nullptr) {
                break;
            }
        }

        if (BugCheck2 == nullptr) {
            return false;
        }

        for (unsigned long Index = 1, Tried = 0; Index + 3 < 0x1000 && Tried < 16; Index++) {
            unsigned char* Candidate = static_cast<unsigned char*>(BugCheck2) + Index;

            if (!RangeContains(Candidate, 3)) {
                break;
            }

            if (Candidate[0] != 0xF3 || Candidate[1] != 0x90 || Candidate[2] != 0xEB) {
                continue;
            }

            unsigned char* Pause = Candidate;
            unsigned char* SpinBegin = static_cast<unsigned char*>(Pause + 2 + *reinterpret_cast<CHAR*>(Pause + 1));

            if (!RangeContains(SpinBegin, 0xC0)) {
                continue;
            }

            Tried++;

            unsigned char* Search = nullptr;

            if (SpinBegin[0] == 0x33 && SpinBegin[4] == 0xE8) {
                Search = SpinBegin + 4 + 5 + *reinterpret_cast<LONG*>(SpinBegin + 4 + 1);

                for (unsigned long Scan = 0; Scan < 0x500; Scan++, Search++) {
                    if (!RangeContains(Search, 8)) {
                        break;
                    }

                    if (*reinterpret_cast<USHORT*>(Search) == 0x394C) {
                        IpiFrozenOffset = static_cast<unsigned long>(*reinterpret_cast<LONG*>(Search - 4));
                        FoundIpiFrozen = true;

                        break;
                    }
                }

                if (FoundIpiFrozen) {
                    for (unsigned long Scan = 0; Scan < 0x500; Scan++, Search++) {
                        if (!RangeContains(Search, 5)) {
                            break;
                        }

                        if (Search[0] == 0xE8 && Search[4] == 0x00) {
                            Search = Search + 5 + *reinterpret_cast<LONG*>(Search + 1);

                            break;
                        }
                    }
                }
            } else if (SpinBegin[0] == 0x65 && RangeContains(SpinBegin, 0x20) && SpinBegin[9] == 0x8B && SpinBegin[0x18] == 0xE8) {
                IpiFrozenOffset = *reinterpret_cast<unsigned long*>(SpinBegin + 0xB);
                FoundIpiFrozen = true;
            }

            if (!FoundIpiFrozen) {
                for (unsigned long Scan = 0; Scan + 6 < 0xC0; Scan++) {
                    unsigned char* Probe = SpinBegin + Scan;

                    if (!RangeContains(Probe, 6)) {
                        break;
                    }

                    if (Probe[0] == 0x8B && (Probe[1] & 0xC7) == 0x80) {
                        unsigned long Displacement = *reinterpret_cast<unsigned long*>(Probe + 2);

                        if (Displacement < 0xC000) {
                            IpiFrozenOffset = Displacement;
                            FoundIpiFrozen = true;

                            break;
                        }
                    }
                }
            }

            if (FoundIpiFrozen) {
                break;
            }
        }

        void* TableValue = SystemExport(L"HalPrivateDispatchTable");

        if (TableValue != nullptr && RangeContains(TableValue, 0x250)) {
            unsigned long Version = *static_cast<volatile ULONG*>(TableValue);

            if (Version >= 6 && Version <= 0x100) {
                HalTableValue = TableValue;
                HalVersion = Version;
                FoundTable = true;
            }
        }

        if (!FoundTable || !FoundBugActive || !FoundHardwareTrigger || !FoundContext || !FoundIrql) {
            return false;
        }

        if (ContextOffset == 0 || ContextOffset >= 0x20000 || DebugIrqlOffset == 0 || DebugIrqlOffset >= 0x10000) {
            return false;
        }

        Table.Table = HalTableValue;
        Table.Version = HalVersion;
        Anchors.Active = BugActive;
        Anchors.HardwareTrigger = HardwareTrigger;
        Layout.ContextOffset = ContextOffset;
        Layout.DebugIrqlOffset = DebugIrqlOffset;
        Layout.DebugIrqlIsPcr = true;

        if (FoundIpiFrozen && IpiFrozenOffset < 0x20000) {
            Layout.IpiFrozenOffset = IpiFrozenOffset;
            Layout.IpiFrozenValid = true;
        }

        Ready = true;

        return true;
    }
}
