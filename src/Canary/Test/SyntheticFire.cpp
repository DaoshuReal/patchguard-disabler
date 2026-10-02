#include <ntddk.h>

#include <Canary/Test/Test.h>

extern "C" NTSYSAPI VOID NTAPI RtlCaptureContext(PCONTEXT ContextRecord);

namespace Pgc
{
    static volatile LONG SyntheticStage = 0;

    void CanaryTest::FireSynthetic()
    {
        CONTEXT Plant;

        RtlZeroMemory(&Plant, sizeof(Plant));
        RtlCaptureContext(&Plant);

        Plant.ContextFlags = 0x10005F;
        Plant.SegCs = 0x10;
        Plant.SegDs = 0x2B;
        Plant.SegEs = 0x2B;
        Plant.SegGs = 0x2B;
        Plant.SegFs = 0x53;

        if (SyntheticStage == 0) {
            SyntheticStage = 1;

            DbgPrint("[Pgc] Firing synthetic 0x109\n");

            KeBugCheckEx(0x109, 0xA0A0A0A0, 0xBB, 0xCC, 0xDD);
        } else {
            DbgPrint("[Pgc] Survived synthetic 0x109\n");
        }
    }
}
