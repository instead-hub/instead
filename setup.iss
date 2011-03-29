[Setup]
AppName=INSTEAD
AppVerName=INSTEAD 1.3.5
DefaultDirName={pf}\Pinebrush games\INSTEAD
DefaultGroupName=Pinebrush games
UninstallDisplayIcon={app}\sdl-instead.exe
OutputDir=.
OutputBaseFilename=instead-1.3.5
AllowNoIcons=true
SetupIconFile=icon\sdl_instead.ico

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl

[Files]
Source: sdl-instead.exe; DestDir: {app}
Source: Microsoft.VC80.CRT\*; DestDir: {app}\Microsoft.VC80.CRT
Source: games\tutorial2\*; DestDir: {app}\games\tutorial2
Source: games\tutorial2-en\*; DestDir: {app}\games\tutorial2-en
Source: games\tutorial2-es\*; DestDir: {app}\games\tutorial2-es
Source: games\tutorial2-it\*; DestDir: {app}\games\tutorial2-it
Source: icon\*; DestDir: {app}\icon
Source: doc\*; DestDir: {app}\doc
Source: doc\modules\*; DestDir: {app}\doc\modules
Source: stead\*; DestDir: {app}\stead
Source: lang\*; DestDir: {app}\lang
Source: themes\default\*; DestDir: {app}\themes\default
Source: themes\book\*; DestDir: {app}\themes\book
Source: themes\original\*; DestDir: {app}\themes\original
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
Name: {pf}\Pinebrush games; Type: dirifempty

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
