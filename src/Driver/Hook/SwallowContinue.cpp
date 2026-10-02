#include <ntddk.h>
#include <intrin.h>

#include <Driver/Hook/Hook.h>

extern "C" NTKERNELAPI void NTAPI RtlRestoreContext(PCONTEXT ContextRecord, PEXCEPTION_RECORD ExceptionRecord);

namespace Pgd
{
    void Swallow::Continue(CONTEXT* Context, KIRQL Irql)
    {
        if (!Probe::RangeMapped(Context, sizeof(CONTEXT))) {
            __fastfail(0);
        }

        PgCli();
        PgWriteCr8(Irql);

        Context->ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS | CONTEXT_FLOATING_POINT;

        RtlRestoreContext(Context, nullptr);

        __fastfail(0);
    }
}
