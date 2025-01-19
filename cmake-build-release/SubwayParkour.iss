; 脚本示例
[Setup]
AppName=Jump
AppVersion=1.0
DefaultDirName={pf}\Jump_simple
OutputDir=Output
OutputBaseFilename=JumpInstaller
Compression=lzma2/ultra
SolidCompression=yes
; 其他配置项根据需要添加

[Files]
Source: "jump.exe"; DestDir: "{app}"; DestName: "Jump.exe"
Source: "font_0.ttf"; DestDir: "{app}"
Source: "font_1.ttf"; DestDir: "{app}"
Source: "C:\enviorment\SDL2-2.26.1\SDL2-2.26.1\x86_64-w64-mingw32\bin\SDL2_ttf.dll"; DestDir: "{app}"
Source: "C:\enviorment\SDL2-2.26.1\SDL2-2.26.1\x86_64-w64-mingw32\bin\SDL2.dll"; DestDir: "{app}"
Source: "C:\enviorment\SDL2-2.26.1\SDL2-2.26.1\x86_64-w64-mingw32\bin\SDL2_image.dll"; DestDir: "{app}"
Source: "C:\enviorment\SDL2-2.26.1\SDL2-2.26.1\x86_64-w64-mingw32\bin\sdl2-config"; DestDir: "{app}"

[Icons]
Name: "{group}\Jump"; Filename: "{app}\Jump.exe"
; 其他快捷方式等

[Run]
Filename: "{app}\Jump.exe"; Description: "{cm:LaunchProgram,Jump}"; Flags: nowait postinstall
