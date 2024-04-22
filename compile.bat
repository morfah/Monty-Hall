echo off
del "./*.exe"
del "./*.obj"
cl /O2 /EHsc /W4 /std:c++17 .\main.cpp
@if %ERRORLEVEL% == 0 (main.exe)
