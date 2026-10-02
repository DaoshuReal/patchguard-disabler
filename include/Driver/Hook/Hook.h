#pragma once

#include <ntddk.h>

extern "C" void PgCli();
extern "C" void PgSti();
extern "C" void PgWriteCr8(unsigned __int64 Value);
extern "C" void PgWriteGsByte(unsigned long Offset, unsigned char Value);

namespace Pgd
{
    class HalHook;
    class Swallow;

    class PhysMem
    {
    public:
        static bool WritePointer(void** Slot, void* Value);
    };

    class Probe
    {
    public:
        static bool RangeMapped(void* Pointer, unsigned long Size);
    };

    class HookHandler
    {
    public:
        static void Prepare(BOOLEAN FirstParameter);
        static void NotifyFreeze(BOOLEAN FirstParameter, BOOLEAN SecondParameter);
    };

    class HalHook
    {
    public:
        static bool Install(void* Table);
        static void Remove();
    private:
        static void* Table;
        static void* OriginalPrepare;
        static void* OriginalFreeze;
        friend class HookHandler;
        friend class Swallow;
    };

    class Swallow
    {
    public:
        static bool TrySwallow109();
    private:
        static bool TrySwallow109Guarded();
        static CONTEXT* FindContext(unsigned long long Rsp);
        static void Continue(CONTEXT* Context, KIRQL Irql);
    };
}
