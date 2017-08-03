mkdir project\lib\

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.10.25017\bin\HostX64\x64\lib.exe" /OUT:project\lib\sc2apid.lib build\bin\sc2apid.lib build\bin\civetweb.lib build\bin\libprotobufd.lib build\bin\sc2libd.lib build\bin\sc2protocold.lib build\bin\sc2rendererd.lib build\bin\sc2utilsd.lib
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.10.25017\bin\HostX64\x64\lib.exe" /OUT:project\lib\sc2api.lib build\bin\sc2api.lib build\bin\civetweb.lib build\bin\libprotobuf.lib build\bin\sc2lib.lib build\bin\sc2protocol.lib build\bin\sc2renderer.lib build\bin\sc2utils.lib

xcopy include\sc2api\* project\include\sc2api\ /s /e
xcopy include\sc2lib\* project\include\sc2lib\ /s /e
xcopy include\sc2renderer\* project\include\sc2renderer\ /s /e
xcopy include\sc2utils\* project\include\sc2utils\ /s /e
xcopy build\generated\* project\include\sc2protocol\ /s /e
xcopy contrib\protobuf\src\google\* project\include\google\ /s /e
