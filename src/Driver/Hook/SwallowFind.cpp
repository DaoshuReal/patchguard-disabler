#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    CONTEXT* Swallow::FindContext(unsigned long long Rsp)
    {
        constexpr unsigned long long Align = 16;
        constexpr unsigned long ProbeSize = 0x48;

        Rsp = (Rsp + Align - 1) & ~(Align - 1);
        Rsp -= Align;

        for (int Index = 0; Index < 4096; Index++) {
            Rsp += Align;

            CONTEXT* Context = reinterpret_cast<CONTEXT*>(Rsp);

            if (!Probe::RangeMapped(Context, ProbeSize)) {
                return nullptr;
            }

            __try {
                if (Context->ContextFlags != 0x10005F && Context->ContextFlags != 0x10001F) {
                    continue;
                }

                if (Context->SegCs != 0x10) {
                    continue;
                }

                if (Context->SegDs == 0x2B && Context->SegEs == 0x2B && Context->SegFs == 0x53 && Context->SegGs == 0x2B) {
                    return Context;
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                return nullptr;
            }
        }

        return nullptr;
    }
}
