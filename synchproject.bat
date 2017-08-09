mkdir project\lib\

xcopy build\bin\civetweb.lib project\lib\ /s /e
xcopy build\bin\libprotobuf.lib project\lib\ /s /e
xcopy build\bin\libprotobufd.lib project\lib\ /s /e
xcopy build\bin\sc2lib.lib project\lib\ /s /e
xcopy build\bin\sc2libd.lib project\lib\ /s /e
xcopy build\bin\sc2api.lib project\lib\ /s /e
xcopy build\bin\sc2apid.lib project\lib\ /s /e
xcopy build\bin\sc2protocol.lib project\lib\ /s /e
xcopy build\bin\sc2protocold.lib project\lib\ /s /e
xcopy build\bin\sc2renderer.lib project\lib\ /s /e
xcopy build\bin\sc2rendererd.lib project\lib\ /s /e
xcopy build\bin\sc2utils.lib project\lib\ /s /e
xcopy build\bin\sc2utilsd.lib project\lib\ /s /e

xcopy maps\* project\project\maps\ /s /e
xcopy include\sc2api\* project\include\sc2api\ /s /e
xcopy include\sc2lib\* project\include\sc2lib\ /s /e
xcopy include\sc2renderer\* project\include\sc2renderer\ /s /e
xcopy include\sc2utils\* project\include\sc2utils\ /s /e
xcopy build\generated\* project\include\ /s /e
xcopy contrib\protobuf\src\google\* project\include\google\ /s /e
