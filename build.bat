@echo off

echo Building client...
clang++ common/*.cpp client/*.cpp -o bin/client.exe -I common

echo Building server...
clang++ server/server.cpp -o bin/server.exe -std=c++1z
echo Done.