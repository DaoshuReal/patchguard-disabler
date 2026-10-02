#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    void HookHandler::NotifyFreeze(BOOLEAN FirstParameter, BOOLEAN SecondParameter)
    {
        auto Original = reinterpret_cast<void (*)(BOOLEAN, BOOLEAN)>(HalHook::OriginalFreeze);

        if (Original != nullptr) {
            Original(FirstParameter, SecondParameter);
        }
    }
}
