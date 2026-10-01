@echo off
setlocal

set VCVARSALL="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
set OUT_DIR=%~dp0

echo Initializing MSVC environment...
call %VCVARSALL% x64

echo Building TestApp (Release, Static CRT, Size-optimized)...
cl.exe ^
    /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_HAS_EXCEPTIONS=0" ^
    /std:c++17 ^
    /EHs-c- ^
    /W3 ^
    /O1 ^
    /GL ^
    /MT ^
    "%OUT_DIR%rfc.cpp" ^
    "%OUT_DIR%main.cpp" ^
    /Fe:"%OUT_DIR%TestApp.exe" ^
    /link /LTCG /OPT:REF /OPT:ICF user32.lib gdi32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib shlwapi.lib

if %ERRORLEVEL% == 0 (
    echo.
    echo Build succeeded: TestApp.exe
) else (
    echo.
    echo Build FAILED.
)

endlocal
