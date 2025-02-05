
[Setup]
;SignTool=signtool
Uninstallable=yes
AppId={{AC30DEEF-5373-4E75-B551-15125D71FB11}
AppName="IRLoader"
AppVersion="1.0"
AppPublisher="Bartolomej Pechacek"
OutputDir="C:\Users\knize\Documents\VST_CODE"
OutputBaseFilename="IRLoaderInstaller"
Compression=lzma
SolidCompression=yes
DefaultDirName="C:\Program Files\PechacekIRLoader"
DisableDirPage=true

[Files]
Source: "C:\Users\knize\Desktop\PechacekIRLoader\*"; DestDir: "C:\ProgramData\PechacekIRLoader"; Flags: recursesubdirs createallsubdirs 
Source: "C:\Users\knize\Documents\VST_CODE\BasicEQ\Builds\VisualStudio2022\x64\Release\Standalone Plugin\*"; DestDir: "C:\Program Files\PechacekIRLoader";   Flags: recursesubdirs createallsubdirs
Source: "C:\Users\knize\Documents\VST_CODE\BasicEQ\Builds\VisualStudio2022\x64\Release\VST3\*"; DestDir: "C:\Program Files\Common Files\VST3\PechacekIRLoader";   Flags: recursesubdirs createallsubdirs

[InstallDelete]
Type: filesandordirs; Name: "C:\ProgramData\PechacekIRLoader"
Type: filesandordirs; Name: "C:\Program Files\PechacekIRLoader"
Type: filesandordirs; Name: "C:\Program Files\Common Files\VST3\PechacekIRLoader"