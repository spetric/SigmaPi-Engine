:: Generates single-threaded dynamic library for IA-32 architecture
:: Windows host, Windows target
cd C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\
call vcvars32.bat
set IPPROOT=C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\ipp\
if not exist "C:\intel_custom\ipp\ipp_custom\ia32" mkdir "C:\intel_custom\ipp\ipp_custom\ia32"

cl.exe /c /MT /I "%IPPROOT%\include" "C:\intel_custom\ipp\ipp_custom\main.c" /Fo"C:\intel_custom\ipp\ipp_custom\ia32\main.obj"

link.exe /DLL /MACHINE:X86 /VERBOSE:SAFESEH /DEF:"C:\intel_custom\ipp\ipp_custom\export.def" "C:\intel_custom\ipp\ipp_custom\ia32\main.obj" /OUT:"C:\intel_custom\ipp\ipp_custom\ia32\ipp_custom.dll" /IMPLIB:"C:\intel_custom\ipp\ipp_custom\ia32\ipp_custom.lib" "%IPPROOT%\lib\ia32_win\ippccmt.lib"  "%IPPROOT%\lib\ia32_win\ippcvmt.lib" "%IPPROOT%\lib\ia32_win\ippimt.lib" "%IPPROOT%\lib\ia32_win\ippsmt.lib" "%IPPROOT%\lib\ia32_win\ippvmmt.lib" "%IPPROOT%\lib\ia32_win\ippcoremt.lib"
c:
cd c:\intel_custom\ipp\ipp_custom\