Write-Output "Building common components..."
mkdir common\obj > $null
Set-Location common\obj
clang++ ../*.cpp -c -I ../ -std=c++1z -O3
Set-Location ../..

Write-Output "Building client..."
mkdir client\obj > $null
Set-Location client/obj
clang++ ../*.cpp -c -std=c++1z -I ../../common
Set-Location ../..
clang++ client/obj/*.o common/obj/*.o -o bin/client.exe

Write-Output "Building server..."
mkdir server\obj > $null
Set-Location server/obj
clang++ ../*cpp -c -std=c++1z -I ../../common -O3
Set-Location ../..
clang++ server/obj/*.o common/obj/*.o -o bin/server.exe

Write-Output "Cleaning up"
Remove-Item common\obj -r
Remove-Item client\obj -r
Remove-Item server\obj -r

Write-Output "Done."