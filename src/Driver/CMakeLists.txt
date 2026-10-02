add_library(PgDisabler MODULE
    Core/DriverEntry.cpp
    Core/DriverUnload.cpp
    Core/NtRangeInit.cpp
    Resolve/RangeContains.cpp
    Resolve/SystemExport.cpp
    Resolve/ResolveAll.cpp
    Hook/PhysWrite.cpp
    Hook/ProbeRange.cpp
    Hook/HookInstall.cpp
    Hook/HookRemove.cpp
    Hook/SwallowFind.cpp
    Hook/SwallowContinue.cpp
    Hook/SwallowTry.cpp
    Hook/HookPrepare.cpp
    Hook/HookFreeze.cpp
    Hook/SwallowAsm.asm
)

target_include_directories(PgDisabler PRIVATE ${CMAKE_SOURCE_DIR}/include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.28000.0/km" "C:/Program Files (x86)/Windows Kits/10/Include/10.0.28000.0/km/crt")

set_target_properties(PgDisabler PROPERTIES OUTPUT_NAME PgDisabler SUFFIX .sys MSVC_RUNTIME_LIBRARY MultiThreaded)

target_compile_definitions(PgDisabler PRIVATE _AMD64_ AMD64 _WIN64 WIN64 WINVER=0x0A00 _WIN32_WINNT=0x0A00 NTDDI_VERSION=0x0A00000C POOL_NX_OPTIN=1)

target_compile_options(PgDisabler PRIVATE /kernel /W3 /Gy /Zi /sdl- /GS-)

target_link_directories(PgDisabler PRIVATE "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.28000.0/km/x64")

target_link_libraries(PgDisabler PRIVATE ntoskrnl.lib hal.lib BufferOverflowK.lib wmilib.lib)

target_link_options(PgDisabler PRIVATE /DRIVER:WDM /SUBSYSTEM:NATIVE /ENTRY:DriverEntry /IGNORE:4078 /IGNORE:4221 /MANIFEST:NO /INCREMENTAL:NO /OPT:REF /OPT:ICF /DEBUG)

add_custom_command(TARGET PgDisabler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:PgDisabler> ${CMAKE_SOURCE_DIR}/build/PgDisabler.sys
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/SignDriver.ps1 -DriverPath ${CMAKE_SOURCE_DIR}/build/PgDisabler.sys
)
