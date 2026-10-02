#include <ntddk.h>

#include <Driver/Hook/Hook.h>

namespace Pgd
{
    void HookHandler::Prepare(BOOLEAN FirstParameter)
    {
        if (Swallow::TrySwallow109()) {
            return;
        }

        auto Original = reinterpret_cast<void (*)(BOOLEAN)>(HalHook::OriginalPrepare);

        if (Original != nullptr) {
            Original(FirstParameter);
        }
    }
}
