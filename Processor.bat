@call parameters.bat Processor.cpp asm.cpp general.cpp strsort.cpp stack.cpp SoftCpu.cpp -o Processor.exe

@if not errorlevel 1 (
    Processor.exe %*
)