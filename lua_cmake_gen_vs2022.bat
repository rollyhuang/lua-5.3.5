if not exist build md build
cmake -G "Visual Studio 17" -A x64 -H. -Bbuild
pause