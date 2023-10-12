@echo off
del /q *.obj > nul

REM /GR-: Disable Run-Time Type Information
REM /W3:  Warning level 3
REM /WX:  Warnings as errors.
REM /0x:  Enable Most Speed Optimizations
REM /GF:  Eliminate Duplicate String
REM /MD:  Multi-threaded, release runtime library
REM /GY-: Enable Function-Level Linking
REM /Ob2: Inline Function Expansion
REM /Oi:  Generate Intrinsic Functions
REM /Ot:  Favor Fast Code
REM /Oy:  Frame Pointer Omission
REM /GT   Fiber-safe thread-local storage
cl.exe /D_HAS_EXCEPTIONS=0 /D"NDEBUG" /nologo /GR- /W3 /WX /Ox /GF /MD /Gy- /Ob2 /Oi /Ot /Oy /GT main.cpp binary_search.cpp hash_search.cpp linear_search.cpp runner.cpp
if '%errorlevel%' neq '0' exit /b 1

echo Running...
%~dp0main.exe > results.csv
