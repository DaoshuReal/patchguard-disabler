add_library(PgCanary MODULE
    Core/DriverEntry.cpp
    Core/DriverUnload.cpp
    Core/NtRangeInit.cpp
    Test/ExportQuery.cpp
    Test/InsideNt.cpp
    Test/HooksCheck.cpp
    Test/SyntheticFire.cpp
    Test/PhysWrite.cpp
    Test/RealPatchThread.cpp
    Test/RealPatchStart.cpp
)

target_include_directories(PgCanary PRIVATE ${CMAKE_SOURCE_DIR}/include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.28000.0/km" "C:/Program Files (x86)/Windows Kits/10/Include/10.0.28000.0/km/crt")

set_target_properties(PgCanary PROPERTIES OUTPUT_NAME PgCanary SUFFIX .sys MSVC_RUNTIME_LIBRARY MultiThreaded)

target_compile_definitions(PgCanary PRIVATE _AMD64_ AMD64 _WIN64 WIN64 WINVER=0x0A00 _WIN32_WINNT=0x0A00 NTDDI_VERSION=0x0A00000C POOL_NX_OPTIN=1)

target_compile_options(PgCanary PRIVATE /kernel /W3 /Gy /Zi /sdl- /GS-)

target_link_directories(PgCanary PRIVATE "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.28000.0/km/x64")

target_link_libraries(PgCanary PRIVATE ntoskrnl.lib hal.lib BufferOverflowK.lib wmilib.lib)

target_link_options(PgCanary PRIVATE /DRIVER:WDM /SUBSYSTEM:NATIVE /ENTRY:DriverEntry /IGNORE:4078 /IGNORE:4221 /MANIFEST:NO /INCREMENTAL:NO /OPT:REF /OPT:ICF /DEBUG)

add_custom_command(TARGET PgCanary POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:PgCanary> ${CMAKE_SOURCE_DIR}/build/PgCanary.sys
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tools/SignDriver.ps1 -DriverPath ${CMAKE_SOURCE_DIR}/build/PgCanary.sys
)
