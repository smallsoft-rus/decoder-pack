; SmallSoft Decoder Pack 
; Copyright (c) 2023,  MSDN.WhiteKnight (https://github.com/smallsoft-rus/decoder-pack) 
; License: BSD 2.0

[Setup]
AppId={{D0C3BDCD-78DA-480A-B687-1BD744178591}
AppName=SmallSoft Decoder Pack
AppVersion=1.1
AppPublisher=SmallSoft, Inc.
AppPublisherURL=https://smallsoftru.wordpress.com/
AppSupportURL=https://smallsoftru.wordpress.com/
AppUpdatesURL=https://smallsoftru.wordpress.com/
DefaultDirName={autopf}\SmallSoft_Decoder_Pack
DefaultGroupName=SmallSoft Decoder Pack
InfoBeforeFile=package\package_info.txt
OutputDir=Release
OutputBaseFilename=decoderpack
SetupIconFile=config\images\smallsoft.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "Ярлык настройки на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}";
;-----SMALL MEDIA PLAYER -----
Name: "smplayer_desktopicon"; Description: "Ярлык Small Media Player на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components:program\smplayer;
;----MMCONTROL-----
Name: "mmcontrol_desktopicon"; Description: "Ярлык Multimedia Control Panel на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components:program\mmcontrol;

[Files]
Source: "C:\Distr\SmallSoft_Decoder_Pack\config.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: main;
Source: "package\package_info.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: main;
Source: "package\readme.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: main;
Source: "C:\Distr\SmallSoft_Decoder_Pack\codecs.reg"; DestDir: "{app}"; Flags: ignoreversion; Components: main;
Source: "C:\Distr\SmallSoft_Decoder_Pack\smplayer.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: program\smplayer;
Source: "C:\Distr\SmallSoft_Decoder_Pack\smp.ico"; DestDir: "{app}"; Flags: ignoreversion; Components: program\smplayer;
Source: "C:\Distr\SmallSoft_Decoder_Pack\mmcontrol.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: program\mmcontrol;
Source: "C:\Distr\SmallSoft_Decoder_Pack\readme_mmcontrol.txt"; DestDir: "{app}"; Flags: ignoreversion; Components: program\mmcontrol;
Source: "C:\Distr\SmallSoft_Decoder_Pack\AAC\*"; DestDir: "{app}\AAC"; Flags: ignoreversion recursesubdirs createallsubdirs;Components: audio\aac;
Source: "C:\Distr\SmallSoft_Decoder_Pack\DC-Bass_Source\*"; DestDir: "{app}\DC-Bass_Source"; Flags: ignoreversion recursesubdirs createallsubdirs;Components: audio\bass;
Source: "C:\Distr\SmallSoft_Decoder_Pack\dscaler\MpegAudio.dll"; DestDir: "{app}\dscaler"; Flags: ignoreversion;Components: audio\ds_audio;
Source: "C:\Distr\SmallSoft_Decoder_Pack\divx\DivXDec.ax"; DestDir: "{app}\divx"; Flags: ignoreversion;Components: video\divx;
Source: "C:\Distr\SmallSoft_Decoder_Pack\divx\DivXDecH264.ax"; DestDir: "{app}\divx"; Flags: ignoreversion;Components: video\divx_h264;
Source: "C:\Distr\SmallSoft_Decoder_Pack\divx\divx_h264_info.txt"; DestDir: "{app}\divx"; Flags: ignoreversion;Components: video\divx_h264;
Source: "C:\Distr\SmallSoft_Decoder_Pack\dscaler\MpegVideo.dll"; DestDir: "{app}\dscaler"; Flags: ignoreversion;Components: video\ds_video;
Source: "C:\Distr\SmallSoft_Decoder_Pack\splitter\MatroskaSplitter.ax"; DestDir: "{app}\splitter"; Flags: ignoreversion;Components: video\mkv;
Source: "C:\Distr\SmallSoft_Decoder_Pack\splitter\MpegSplitter.ax"; DestDir: "{app}\splitter"; Flags: ignoreversion;Components: video\mpeg;

Source: "C:\Distr\SmallSoft_Decoder_Pack\divx\divx.ico"; DestDir: "{app}\divx"; Flags: ignoreversion;Components: video\divx or video\divx_h264;
Source: "C:\Distr\SmallSoft_Decoder_Pack\dscaler\DScaler5.chm"; DestDir: "{app}\dscaler"; Flags: ignoreversion recursesubdirs createallsubdirs;Components: audio\ds_audio or video\ds_video;
Source: "C:\Distr\SmallSoft_Decoder_Pack\dscaler\GenDMOProp.dll"; DestDir: "{app}\dscaler"; Flags: ignoreversion recursesubdirs createallsubdirs;Components: audio\ds_audio or video\ds_video;
Source: "C:\Distr\SmallSoft_Decoder_Pack\dscaler\info.txt"; DestDir: "{app}\dscaler"; Flags: ignoreversion recursesubdirs createallsubdirs;Components: audio\ds_audio or video\ds_video;

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Components]
Name: "main"; Description: "Core"; Types: full compact custom; Flags: fixed
Name: "audio"; Description: "Audio"; Types: full compact custom
Name: "audio\aac"; Description: "CoreAAC Audio Decoder"; Types: full compact custom
Name: "audio\bass"; Description: "DC - Bass Source"; Types: full compact custom
Name: "audio\ds_audio"; Description: "DScaler Audio Decoder"; Types: full compact custom
Name: "video"; Description: "Video / Splitter"; Types: full 
Name: "video\divx"; Description: "DivX ASP Decoder"; Types: full
Name: "video\divx_h264"; Description: "DivX H.264/AVC Decoder"; Types: full
Name: "video\ds_video"; Description: "DScaler MPEG-2 Video Decoder"; Types: full
Name: "video\mpeg"; Description: "Gabest MPEG Splitter"; Types: full
Name: "video\mkv"; Description: "Gabest MKV Splitter"; Types: full
Name: "program"; Description: "Programs"; Types: full compact custom
Name: "program\smplayer"; Description: "Small Media Player"; Types: full compact custom
Name: "program\mmcontrol"; Description: "SmallSoft Multimedia Control Panel"; Types: full compact custom

[Icons]
Name: "{group}\Настройка SmallSoft Decoder Pack"; Filename: "{app}\config.exe"
Name: "{group}\{cm:ProgramOnTheWeb,SmallSoft Decoder Pack}"; Filename: "https://smallsoftru.wordpress.com/"
Name: "{group}\Исходный код"; Filename: "https://github.com/smallsoft-rus/decoder-pack";
Name: "{group}\{cm:UninstallProgram,SmallSoft Decoder Pack}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Настройка SmallSoft Decoder Pack"; Filename: "{app}\config.exe"; Tasks: desktopicon

;FAVOURITES
Name: "{userfavorites}\SmallSoft - Бесплатные программы"; IconFilename:"{app}\config.exe"; Filename: "https://smallsoftru.wordpress.com/"; MinVersion: 4.1,4;
;-------------------------------------------------------------------------------------------------------------------------
; DESKTOP PROGRAM ICONS
Name: "{commondesktop}\Small Media Player"; IconFilename:"{app}\smp.ico"; Filename: "{app}\smplayer.exe"; Tasks: smplayer_desktopicon;
Name: "{commondesktop}\SmallSoft Multimedia Control Panel"; Filename: "{app}\mmcontrol.exe"; Tasks: mmcontrol_desktopicon;

;CONFIG ICONS PROGRAM GROUP
Name: "{group}\Аудио кодеки\Свойства DC-Bass Source"; Filename: "{app}\config.exe"; Parameters:"/p ""DC-Bass Source"""; IconFileName:"{app}\DC-Bass_Source\dc.ico";Components:audio\bass;
Name: "{group}\Аудио кодеки\Свойства Core AAC Audio Decoder"; Filename: "{app}\config.exe"; Parameters:"/p ""CoreAAC Audio Decoder"""; IconFileName:"{app}\AAC\aac.ico";Components:audio\aac;
Name: "{group}\Видео кодеки\Свойства DivX MPEG-4 ASP Decoder"; Filename: "{app}\config.exe"; Parameters:"/p ""DivX Decoder Filter"""; IconFileName:"{app}\divx\divx.ico";Components:video\divx;
Name: "{group}\Видео кодеки\Настроить DivX H.264 Decoder"; Filename: "{app}\config.exe"; Parameters:"/p ""DivX H.264 Decoder"""; IconFileName:"{app}\divx\divx.ico";Components:video\divx_h264;
;PROGRAM ICONS IN GROUP
Name: "{group}\Программы\Small Media Player"; IconFilename:"{app}\smp.ico"; Filename: "{app}\smplayer.exe"; Components:program\smplayer;
Name: "{group}\Программы\SmallSoft Multimedia Control Panel"; Filename: "{app}\mmcontrol.exe"; Components:program\mmcontrol;


[Run]
Filename: "{app}\config.exe"; Description: "Настроить кодеки"; Flags: nowait postinstall skipifsilent;
Filename: "{app}\mmcontrol.exe"; Description: "Запустить утилиту SmallSoft Multimedia Control Panel"; Flags: nowait postinstall skipifsilent; Components: program\mmcontrol;
Filename: "{app}\smplayer.exe"; Description: "Запустить Small Media Player"; Flags: nowait postinstall skipifsilent; Components: program\smplayer;
;----REGISTER CODECS------
Filename: "regsvr32.exe"; Parameters:"/s ""{app}\aac\CoreAAC.ax""";  StatusMsg: "Регистрация CoreAAC";  Flags:runhidden; Components:audio\aac;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\DC-Bass_Source\DCBassSource.ax""";  StatusMsg: "Регистрация DC-Bass Source";  Flags:runhidden; Components:audio\bass;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\dscaler\MpegAudio.dll""";  StatusMsg: "Регистрация DScaler Audio Decoder";  Flags:runhidden; Components:audio\ds_audio;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\divx\DivXDec.ax""";  StatusMsg: "Регистрация DivX Decoder";  Flags:runhidden; Components:video\divx;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\divx\DivXDecH264.ax""";  StatusMsg: "Регистрация DivX h264 Decoder";  Flags:runhidden; Components:video\divx_h264;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\dscaler\MpegVideo.dll""";  StatusMsg: "Регистрация DScaler Mpeg2 Video Decoder";  Flags:runhidden; Components:video\ds_video;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\splitter\MatroskaSplitter.ax""";  StatusMsg: "Регистрация Gabest MKV Splitter";  Flags:runhidden; Components:video\mkv;
Filename: "regsvr32.exe"; Parameters:" /s ""{app}\splitter\MpegSplitter.ax""";  StatusMsg: "Регистрация Gabest MPEG Splitter";  Flags:runhidden; Components:video\mpeg;

[UninstallRun]
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\aac\CoreAAC.ax""";   Flags:runhidden; Components:audio\aac; RunOnceId: DelCoreAAC;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\DC-Bass_Source\DCBassSource.ax""";  Flags:runhidden; Components:audio\bass; RunOnceId: DelBass;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\dscaler\MpegAudio.dll"""; Flags:runhidden; Components:audio\ds_audio; RunOnceId: DelMpegAudio;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\divx\DivXDec.ax""";  Flags:runhidden; Components:video\divx; RunOnceId: DelDivx;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\divx\DivXDecH264.ax"""; Flags:runhidden; Components:video\divx_h264; RunOnceId: DelH264;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\dscaler\MpegVideo.dll"""; Flags:runhidden; Components:video\ds_video; RunOnceId: DelMpegVideo;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\splitter\MatroskaSplitter.ax"""; Flags:runhidden; Components:video\mkv; RunOnceId: DelMkv;
Filename: "regsvr32.exe"; Parameters:" /u /s ""{app}\splitter\MpegSplitter.ax"""; Flags:runhidden; Components:video\mpeg; RunOnceId: DelMpegSplitter;


[Registry]
Root: HKLM; Subkey: "Software\SmallSoft\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror;
;------------------AAC-----------------------------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:string; ValueName:"";ValueData:"CoreAAC Audio Decoder";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:string; ValueName:"file";ValueData:"{app}\aac\CoreAAC.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:binary; ValueName:"HasProperties";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:string; ValueName:"guid";ValueData:"{{6AC7C19E-8CA0-4E3D-9A9F-2881DE29E0AC}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\aac\"; ValueType:string; ValueName:"formats";ValueData:"AAC";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\aac;
;--------------------BASS-------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:string; ValueName:"";ValueData:"DC-Bass Source";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:string; ValueName:"file";ValueData:"{app}\DC-Bass_Source\DCBassSource.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:binary; ValueName:"HasProperties";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:string; ValueName:"guid";ValueData:"{{ABE7B1D9-4B3E-4ACD-A0D1-92611D3A4492}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\bass\"; ValueType:string; ValueName:"formats";ValueData:"WAV MP1 MP2 MP3 OGG AAC FLAC WV ALAC APE";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\bass;
;--------------------DS_AUDIO------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:string; ValueName:"";ValueData:"DScaler Audio Decoder";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:string; ValueName:"file";ValueData:"{app}\dscaler\MpegAudio.dll";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:binary; ValueName:"HasProperties";ValueData:"00";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:string; ValueName:"guid";ValueData:"{{D2CA75C2-05A1-4915-88A8-D433F876D186}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_audio\"; ValueType:string; ValueName:"formats";ValueData:"MP1 MP2 MP3 AC3 DTS";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:audio\ds_audio;
;--------------------DIVX------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:string; ValueName:"";ValueData:"DivX Decoder Filter";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:string; ValueName:"file";ValueData:"{app}\divx\DivXDec.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:binary; ValueName:"HasProperties";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:string; ValueName:"guid";ValueData:"{{78766964-0000-0010-8000-00AA00389B71}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx\"; ValueType:string; ValueName:"formats";ValueData:"DIVX DX50 DIV1 DIV2 DIV3 XVID 3IVX ...";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx;
;--------------------DIVX_H264------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:string; ValueName:"";ValueData:"DivX H.264 Decoder";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:string; ValueName:"file";ValueData:"{app}\divx\DivXDecH264.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:binary; ValueName:"HasProperties";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:string; ValueName:"guid";ValueData:"{{6F513D27-97C3-453C-87FE-B24AE50B1601}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\divx_h264\"; ValueType:string; ValueName:"formats";ValueData:"H264 AVC1";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\divx_h264;
;--------------------DS_VIDEO------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:string; ValueName:"";ValueData:"DScaler Mpeg2 Video Decoder";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:string; ValueName:"file";ValueData:"{app}\dscaler\MpegVideo.dll";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:binary; ValueName:"HasProperties";ValueData:"00";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:string; ValueName:"guid";ValueData:"{{F8904F1F-0371-4471-8866-90E6281ABDB6}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\ds_video\"; ValueType:string; ValueName:"formats";ValueData:"MPEG2";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
;--------------------MKV------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:string; ValueName:"";ValueData:"Matroska Splitter";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:string; ValueName:"file";ValueData:"{app}\splitter\MatroskaSplitter.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:binary; ValueName:"HasProperties";ValueData:"00";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:string; ValueName:"guid";ValueData:"{{149D2E01-C32E-4939-80F6-C07B81015A7A}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\ds_video;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mkv\"; ValueType:string; ValueName:"formats";ValueData:"MKV MKA";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mkv;
;--------------------MPEG_SPLITTER------------------------
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:string; ValueName:"";ValueData:"MPC - Mpeg Splitter (Gabest)";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:string; ValueName:"file";ValueData:"{app}\splitter\MpegSplitter.ax";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:binary; ValueName:"IsEnabled";ValueData:"01";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:binary; ValueName:"HasProperties";ValueData:"00";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:string; ValueName:"guid";ValueData:"{{DC257063-045F-4BE2-BD5B-E12279C464F0}";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
Root: HKLM; Subkey: "Software\SmallSoft\Codecs\mpeg\"; ValueType:string; ValueName:"formats";ValueData:"MPEG MPG MP1 MP2 MP3 TS";Flags: uninsdeletevalue uninsdeletekeyifempty noerror; Components:video\mpeg;
