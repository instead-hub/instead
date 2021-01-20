[Setup]
AppName=INSTEAD-KIT
AppVerName=INSTEAD 3.3.3
DefaultDirName={pf}\Games\INSTEAD
DefaultGroupName=Games
UninstallDisplayIcon={app}\sdl-instead.exe
OutputDir=.
OutputBaseFilename=instead-kit-3.3.3
AllowNoIcons=true

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl

[Files]
Source: sdl-instead.exe; DestDir: {app}
Source: instead-launcher.exe; DestDir: {app}
Source: qt_ru.qm; DestDir: {app}
Source: appdata\*; DestDir: {app}\appdata

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
Source: themes\wide\*; DestDir: {app}\themes\wide
Source: *.dll; DestDir: {app}

[CustomMessages]
CreateDesktopIcon=Create a &desktop icons
LaunchGame=Launch &game
UninstallMsg=Uninstall INSTEAD-KIT
ru.CreateDesktopIcon=Создать &ярлыки на рабочем столе
ru.LaunchGame=Запустить &игру
ru.UninstallMsg=Удалить INSTEAD
ru.RmSettingsMsg=Удалить настройки и сохранённые игры?

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}

[Run]
Filename: {app}\instead-launcher.exe; Description: {cm:LaunchGame}; WorkingDir: {app}; Flags: postinstall

[Icons]
Name: {commondesktop}\INSTEAD; Filename: {app}\sdl-instead.exe; WorkingDir: {app}; Tasks: desktopicon
Name: {commondesktop}\INSTEAD-Launcher; Filename: {app}\instead-launcher.exe; WorkingDir: {app}; Tasks: desktopicon
Name: {group}\INSTEAD; Filename: {app}\sdl-instead.exe; WorkingDir: {app}
Name: {group}\INSTEAD-Launcher; Filename: {app}\instead-launcher.exe; WorkingDir: {app}
Name: {group}\{cm:UninstallMsg}; Filename: {uninstallexe}

[UninstallDelete]
Name: {app}; Type: dirifempty
Name: {pf}\Games; Type: dirifempty
