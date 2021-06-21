[Setup]
AppName=INSTEAD
AppVerName=INSTEAD 3.3.5
DefaultDirName={pf}\Games\INSTEAD
DefaultGroupName=Games
UninstallDisplayIcon={app}\sdl-instead.exe
OutputDir=.
OutputBaseFilename=instead-3.3.5
AllowNoIcons=true
SetupIconFile=icon\sdl_instead.ico
ChangesAssociations=yes

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl

[Files]
Source: sdl-instead.exe; DestDir: {app}
Source: Microsoft.VC80.CRT\*; DestDir: {app}\Microsoft.VC80.CRT
Source: games\tutorial3\*; DestDir: {app}\games\tutorial3
Source: icon\*; DestDir: {app}\icon
Source: doc\*; DestDir: {app}\doc
Source: stead\stead2\*; DestDir: {app}\stead\stead2
Source: stead\stead2\ext\*; DestDir: {app}\stead\stead2\ext
Source: stead\stead3\*; DestDir: {app}\stead\stead3
Source: stead\stead3\ext\*; DestDir: {app}\stead\stead3\ext
Source: lang\*; DestDir: {app}\lang
Source: themes\default\*; DestDir: {app}\themes\default
Source: themes\book\*; DestDir: {app}\themes\book
Source: themes\fantasy\*; DestDir: {app}\themes\fantasy
Source: themes\arctic\*; DestDir: {app}\themes\arctic
Source: themes\bluesteel\*; DestDir: {app}\themes\bluesteel
Source: themes\clearlooks\*; DestDir: {app}\themes\clearlooks
Source: *.dll; DestDir: {app}

[CustomMessages]
CreateDesktopIcon=Create a &desktop icon
LaunchGame=Launch &game
UninstallMsg=Uninstall INSTEAD
RmSettingsMsg=Would you like to remove settings and saved games?
ru.CreateDesktopIcon=Создать &ярлык на рабочем столе
ru.LaunchGame=Запустить &игру
ru.UninstallMsg=Удалить INSTEAD
ru.RmSettingsMsg=Удалить настройки и сохранённые игры?

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}

[Run]
Filename: {app}\sdl-instead.exe; Description: {cm:LaunchGame}; WorkingDir: {app}; Flags: postinstall

[Icons]
Name: {commondesktop}\INSTEAD; Filename: {app}\sdl-instead.exe; WorkingDir: {app}; Tasks: desktopicon
Name: {group}\INSTEAD; Filename: {app}\sdl-instead.exe; WorkingDir: {app}
Name: {group}\{cm:UninstallMsg}; Filename: {uninstallexe}

[UninstallDelete]
Name: {app}; Type: dirifempty
Name: {pf}\Games; Type: dirifempty

[Registry]
Root: HKCR; Subkey: ".idf"; ValueType: string; ValueName: ""; ValueData: "INSTEAD"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "INSTEAD"; ValueType: string; ValueName: ""; ValueData: "INSTEAD data file"; Flags: uninsdeletekey
Root: HKCR; Subkey: "INSTEAD\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\sdl-instead.exe,0"
Root: HKCR; Subkey: "INSTEAD\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\sdl-instead.exe"" ""%1"""

[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  case CurUninstallStep of
    usPostUninstall:
      begin
        if MsgBox(CustomMessage('RmSettingsMsg'), mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = idYes then
        begin
          // remove settings and saved games manually
          DelTree(ExpandConstant('{localappdata}\instead'), True, True, True);
        end;
      end;
  end;
end;
