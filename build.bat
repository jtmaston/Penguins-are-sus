@echo off
echo Building client...
clang++ client/client.cpp -o bin/client.exe -std=c++1z
echo Building server...
clang++ server/server.cpp -o bin/server.exe -std=c++1z
echo Done.