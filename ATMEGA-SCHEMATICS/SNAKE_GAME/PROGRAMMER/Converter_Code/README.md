1. Create a DEF file manually with FTDI functions
2. Convert from Visual Studio C ++ to  MinGW
    dlltool --input-def FTD2XX64.def --output-lib libftd2xx64.a --dllname FTD2XX64.dll


## To compile on Windows:
Example: gcc Simple.c -o Simple.exe -Iinclude -L. -lftd2xx64


