@call parameters.bat MakeAsm.cpp asm.cpp general.cpp strsort.cpp stack.cpp -o MakeAsm.exe

@if not errorlevel 1 (
    MakeAsm.exe %*
)