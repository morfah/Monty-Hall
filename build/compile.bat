@echo off
del "./main.exe"
del "./main.obj"
cl /O2 /EHsc /W4 /std:c++17 ..\src\main.cpp
