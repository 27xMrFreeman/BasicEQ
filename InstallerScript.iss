
[Setup]
;SignTool=signtool
Uninstallable=yes
UninstallFilesDir="C:\Program Files\PechacekIRLoader"
AppId={{AC30DEEF-5373-4E75-B551-15125D71FB11}
AppName="AmpSimulator"
AppVersion="1.0"
AppPublisher="Bartolomej Pechacek"
OutputDir="C:\Users\knize\Documents\VST_CODE"
OutputBaseFilename="AmpSimulatorInstaller"
Compression=lzma
SolidCompression=yes
DefaultDirName="C:\Program Files\PechacekIRLoader"
DisableDirPage=true

[Files]
Source: "C:\Users\knize\Desktop\PechacekIRLoader\*"; DestDir: "C:\ProgramData\PechacekIRLoader"; Flags: recursesubdirs createallsubdirs 
Source: "C:\Users\knize\Documents\VST_CODE\BasicEQ\Builds\VisualStudio2022\x64\Release\Standalone Plugin\Amp Simulator.exe"; DestDir: "C:\Program Files\PechacekIRLoader";   Flags: 
Source: "C:\Users\knize\Documents\VST_CODE\BasicEQ\Amp Simulator.vst3"; DestDir: "C:\Program Files\Common Files\VST3\PechacekIRLoader";   Flags: 

[InstallDelete]
Type: filesandordirs; Name: "C:\ProgramData\PechacekIRLoader"
Type: filesandordirs; Name: "C:\Program Files\PechacekIRLoader"
Type: filesandordirs; Name: "C:\Program Files\Common Files\VST3\PechacekIRLoader"