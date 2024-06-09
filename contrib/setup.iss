; The file encoding is UTF-8 with BOM
; Script is developed and debugged based on Inno Setup Compiler 5.5.5 (u)

; Regenerate AppId if script will be used as template for another application,
; e.g., for installation version of game with built-in interpreter

; Constants to define key parameters of installer:
#define AppName "INSTEAD"
#define AppVersion "3.5.1"
#define AppDescription "Simple Text Adventure Interpreter."
#define AppPublisher "Peter Kosyh"
#define AppURL "https://instead-hub.github.io"
#define AppUpdatesURL "https://github.com/instead-hub/instead/releases/latest"
#define OutputFileName "instead" ; Version number will be added to this
#define OutputDir "."
#define SetupIconFile "icon\sdl_instead.ico"
#define ExeFile "{app}\sdl-instead.exe"

; Global settings used by the installer and uninstaller
[Setup]
; Unique identifier for this application
AppId={{9B009B55-7BAA-4C7F-95FD-2D2DA8D03380}
; Name of application (Product Name property value of installer's File)
AppName={#AppName}
; Version number of application 
; (Product Version property value of installer's File)
AppVersion={#AppVersion}
; Version number of binary file
; (File Version property value of installer's File)
VersionInfoVersion={#AppVersion}
; Copyright of application (Copyright property value of installer's File)
AppCopyright={#AppPublisher} <{#AppURL}>
; Description of application
; (File Description property value of installer's File)
VersionInfoDescription={#AppName} - {#AppDescription}
; Potentially localizable name of application plus its version number
AppVerName={#AppName} {#AppVersion}
; Publisher of application (Publisher value in Add/Remove Programs applet)
AppPublisher={#AppPublisher}
; Link to help page (Help Link value in Add/Remove Programs applet)
AppSupportURL={#AppURL}
; Link to update page (Update Link value in Add/Remove Programs applet)
AppUpdatesURL={#AppUpdatesURL}
; Text of comments (Comments value in Add/Remove Programs applet)
AppComments={cm:AppComments}
; Default destination directory path in installer's dialog
DefaultDirName={pf}\{#AppName}
; Default Start Menu folder name in installer's dialog
DefaultGroupName={#AppName}
; Allow user to disable Start Menu folder creation
AllowNoIcons=yes
; Minimum version of Windows that application being installed runs on.
; Currently minimum compatible version is undeclared,
; but INSTEAD runs even on very old versions of Windows.
;MinVersion=
; Processor architecture(s) on which installer is allowed to run on.
; Currently installer is allowed to run on all processor architectures
; capable of executing its 32-bit code (including ones it doesn't recognize).
; Normally 32-bit binaries run in user mode will run without issue
; on the x64 and Itanium editions of Windows via the WOW64 emulator.
;ArchitecturesAllowed=
; 64-bit processor architecture(s) on which installer should install
; in 64-bit mode.
; Currently INSTEAD is always installed in 32-bit mode.
;ArchitecturesInstallIn64BitMode=
; Name of resulting installer file
OutputBaseFilename={#OutputFileName}-{#AppVersion}
; Directory of resulting installer file
OutputDir={#OutputDir}
; Custom program icon to use for Setup/Uninstall
SetupIconFile={#SetupIconFile}
; Icon file (executable or .ico file) to display for Uninstall entry
; in Add/Remove Programs applet
UninstallDisplayIcon={#ExeFile}
; Tell Windows Explorer to refresh its file associations information
; at the end of installation and uninstallation of the application
ChangesAssociations=yes
; Installer will notify other running applications (notably Windows Explorer)
; that they should reload their environment variables from registry
ChangesEnvironment=yes
; how installer will show Select Destination Location wizard page.
; If this is set to auto, at installer will look in the registry to see
; if the same application is already installed, and if so,
; it will not show Select Destination Location wizard page.
DisableDirPage=auto
; how installer will show Select Start Menu Folder wizard page.
; If this is set to auto, at startup installer will look in registry to see
; if the same application is already installed, and if so,
; it will not show Select Start Menu Folder wizard page.
DisableProgramGroupPage=auto
; Installer will use task settings of previous installation as default settings
UsePreviousTasks=yes
; Method of compression to use on files, and optionally level of compression
Compression=lzma
; Enable solid compression
; (all files to be compressed at once instead of separately)
SolidCompression=yes

; User-customizable tasks installer will perform during installation
[Tasks]
; Checked checkbox to create desktop icon
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
	GroupDescription: "{cm:AdditionalIcons}"
; Unchecked checkbox to create Quick Launch icon
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; \
	GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
; Checked checkbox to associate application with .idf file extension
Name: "associate"; Description: "{cm:AssocFileExtension,{#AppName},.idf}"; \
	GroupDescription: "{cm:OtherTasks}"
; Unchecked checkbox to add application directory to Path
Name: "addtopath"; Description: "{cm:AddToPath,{#AppName}}"; \
	GroupDescription: "{cm:OtherTasks}"; Flags: unchecked

; Files to install on user's system
[Files]
#define DirFlags "recursesubdirs createallsubdirs"
#define FileFlags "ignoreversion sortfilesbyextension sortfilesbyname"
; Directories:
Source: "doc\*"; DestDir: "{app}\doc"; Flags: {#DirFlags} {#FileFlags}
Source: "games\*"; DestDir: "{app}\games"; Flags: {#DirFlags} {#FileFlags}
Source: "icon\*"; DestDir: "{app}\icon"; Flags: {#DirFlags} {#FileFlags}
Source: "lang\*"; DestDir: "{app}\lang"; Flags: {#DirFlags} {#FileFlags}
Source: "Microsoft.VC80.CRT\*"; DestDir: "{app}\Microsoft.VC80.CRT"; \
	Flags: {#DirFlags} {#FileFlags}
Source: "stead\*"; DestDir: "{app}\stead"; Flags: {#DirFlags} {#FileFlags}
Source: "themes\*"; DestDir: "{app}\themes"; Flags: {#DirFlags} {#FileFlags}
; Files:
Source: "*.dll"; DestDir: "{app}"; Flags: {#FileFlags}
Source: "COPYING"; DestDir: "{app}"; Flags: {#FileFlags}
Source: "sdl-instead.exe"; DestDir: "{app}"; Flags: {#FileFlags}

; Shortcuts to create in Start Menu and/or other locations
[Icons]
; Shortcuts in the Start Menu
Name: "{group}\{#AppName}"; Filename: "{#ExeFile}"; WorkingDir: "{app}"; \
	Comment: "{cm:AppComments}"; Flags: runmaximized
Name: "{group}\{cm:ProgramOnTheWeb,{#AppName}}"; Filename: "{#AppURL}"; \
	Flags: runmaximized
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"; \
	WorkingDir: "{app}"; Flags: runmaximized
; Shortcut on desktop
Name: "{commondesktop}\{#AppName}"; Filename: "{#ExeFile}"; \
	WorkingDir: "{app}"; Comment: "{cm:AppComments}"; Flags: runmaximized; \
	Tasks: desktopicon
; Shortcut in Quick Launch toolbar
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#AppName}"; \
	Filename: "{#ExeFile}"; WorkingDir: "{app}"; Comment: "{cm:AppComments}"; \
	Flags: runmaximized; Tasks: quicklaunchicon

; Languages available for selection in installer
[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; \
	LicenseFile: "COPYING"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"; \
	LicenseFile: "COPYING"
; To fully localize installer into other languages,
; need to include Inno Setup message translation file (.isl) and 
; to prepare translations of additional messages in [CustomMessages] section

; Custom message values for {cm:...} constants
[CustomMessages]
; Text of comments (Comments value in Add/Remove Programs applet)
en.AppComments={#AppDescription}
ru.AppComments=Интерпретатор тексто-графических приключений.
; Label of Other tasks task group (displayed in application install dialog)
en.OtherTasks=Other tasks:
ru.OtherTasks=Другие задачи:
; Label of Add to Path task checkbox (displayed in application install dialog)
en.AddToPath=Add %1 to the &Path
ru.AddToPath=До&бавить %1 в Path
; Text of delete settings dialog (displayed at end of uninstallation)
en.DeleteSettings=Would you like to delete settings and saved games?
ru.DeleteSettings=Вы хотите удалить настройки и сохранённые игры?
; Text of delete directory from Path dialog (displayed at end of uninstallation)
en.DeleteFromPath=Would you like to delete the %1 directory from the Path environment variable?%nThe directory is located at the path%n%2
ru.DeleteFromPath=Вы хотите удалить директорию %1 из переменной среды Path?%nДиректория находится по пути%n%2

; Registry keys/values to create, modify, or delete on user's system
[Registry]
; Association of .idf files with application in common settings for all users.
; These keys are created/recreated during installation
; and deleted during uninstallation.
Root: HKLM; Subkey: "SOFTWARE\Classes\.idf"; \
	ValueType: string; ValueName: ""; ValueData: "INSTEAD.DATA"; \
	Flags: deletekey uninsdeletekey; Tasks: associate
Root: HKLM; Subkey: "SOFTWARE\Classes\INSTEAD.DATA"; \
	ValueType: string; ValueName: ""; ValueData: "INSTEAD data file"; \
	Flags: deletekey uninsdeletekey; Tasks: associate
Root: HKLM; Subkey: "SOFTWARE\Classes\INSTEAD.DATA\DefaultIcon"; \
	ValueType: string; ValueName: ""; ValueData: "{#ExeFile},0"; \
	Tasks: associate
Root: HKLM; Subkey: "SOFTWARE\Classes\INSTEAD.DATA\shell\open\command"; \
	ValueType: string; ValueName: ""; ValueData: """{#ExeFile}"" ""%1"""; \
	Tasks: associate
; Association of .idf files with application in settings for current user.
; These keys are deleted during uninstallation,
; but are not creating during installation.
Root: HKCU; Subkey: "SOFTWARE\Classes\.idf"; \
	Flags: dontcreatekey uninsdeletekey; Tasks: associate
Root: HKCU; Subkey: "SOFTWARE\Classes\idf_auto_file"; \
	Flags: dontcreatekey uninsdeletekey; Tasks: associate
Root: HKCU; Subkey: "SOFTWARE\Classes\INSTEAD.DATA"; \
	Flags: dontcreatekey uninsdeletekey; Tasks: associate
Root: HKCU; \
	Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.idf"; \
	Flags: dontcreatekey uninsdeletekey; Tasks: associate

; Operations to execute after application has been successfully installed
[Run]
; Checked checkbox to run application in maximized window
Filename: "{#ExeFile}"; Description: "{cm:LaunchProgram,{#AppName}}"; \
	WorkingDir: "{app}"; Flags: nowait postinstall runmaximized

; Pascal script to customize Setup or Uninstall in many ways
[Code]
// Function returns True if passed directory exists
// in Path environment variable for all users in HKEY_LOCAL_MACHINE,
// and False if it doesn't exist
function DirExistsInPath(sDir: String): Boolean;
var
	sPath: String;
	sDirWithoutBS: String;
	sDirWithBS: String;
begin
	if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
		'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
		'Path', sPath) then
	begin
		Result := False;
		exit;
	end;
	sDir := AnsiLowercase(sDir);
	sDirWithoutBS := ';' + RemoveBackslashUnlessRoot(sDir) + ';';
	sDirWithBS := ';' + AddBackslash(sDir) + ';';
	sPath := ';' + AnsiLowercase(sPath) + ';';
	Result := (Pos(sDirWithoutBS, sPath) > 0) or (Pos(sDirWithBS, sPath) > 0);
end;

// Procedure adds passed directory to Path environment variable
// for all users in HKEY_LOCAL_MACHINE
procedure AddDirToPath(sDir: String);
var
	sPath: String;
	iLen: Integer;
begin
	RegQueryStringValue(HKEY_LOCAL_MACHINE,
		'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
		'Path', sPath);
	iLen := Length(sPath);
	if (iLen > 0) and (Copy(sPath, iLen, 1) <> ';') then sPath := sPath + ';';
	sPath := sPath + sDir;
	RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
		'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
		'Path', sPath);
end;

// Procedure deletes passed directory from Path environment variable
// for all users in HKEY_LOCAL_MACHINE
procedure DelDirFromPath(sDir: String);
var
	sPath: String;
	sPathLC: String;
	sDirLC: String;
	iPos: Integer;
	iLen: Integer;
begin
	RegQueryStringValue(HKEY_LOCAL_MACHINE,
		'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
		'Path', sPath);
	sPath := ';' + sPath + ';';
	sDir := ';' + sDir + ';';
	sPathLC := AnsiLowercase(sPath);
	sDirLC := AnsiLowercase(sDir);
	iPos := Pos(sDirLC, sPathLC);
	if iPos = 0 then exit;
	iLen := Length(sDir);
	sDir := Copy(sPath, iPos, iLen);
	StringChangeEx(sPath, sDir, ';', True);
	iLen := Length(sPath);
	sPath := Copy(sPath, 2, iLen - 2);
	RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
		'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
		'Path', sPath);
end;

// Procedure deletes passed directory from Path environment variable
// for all users in HKEY_LOCAL_MACHINE
// Smart procedure deletes both directory without trailing backslash and with it
procedure DelDirFromPathSmart(sDir: String);
var
	sDirWithoutBS: String;
	sDirWithBS: String;
begin
	sDirWithoutBS := RemoveBackslashUnlessRoot(sDir);
	sDirWithBS := AddBackslash(sDir);
	DelDirFromPath(sDirWithoutBS);
	DelDirFromPath(sDirWithBS);
end;

// Event procedure to perform additional pre-install and post-install tasks
procedure CurStepChanged(CurStep: TSetupStep);
var
	sApp: String;
begin
	case CurStep of
		ssPostInstall:
			// Code to perform post-install tasks
			begin
				sApp := ExpandConstant('{app}');
				if IsTaskSelected('addtopath')
				and not DirExistsInPath(sApp) then
					AddDirToPath(sApp);
			end;
	end;
end;

// Event procedure to perform additional pre-uninstall and post-uninstall tasks
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
	sAppData: String;
	sApp: String;
begin
	case CurUninstallStep of
		usPostUninstall:
			// Code to perform post-uninstall tasks
			begin
				sAppData := ExpandConstant('{localappdata}\instead');
				if DirExists(sAppData) and
					(MsgBox(CustomMessage('DeleteSettings'), mbConfirmation,
					MB_YESNO or MB_DEFBUTTON2) = idYes) then
				begin
					// Delete directory and everything inside it
					DelTree(sAppData, True, True, True);
				end;
				sApp := ExpandConstant('{app}');
				if DirExistsInPath(sApp) then
				begin
					if not DirExists(sApp) or (MsgBox(
						FmtMessage(CustomMessage('DeleteFromPath'), \
							[ExpandConstant('{#AppName}'), sApp]),
						mbConfirmation, MB_YESNO or MB_DEFBUTTON2) = idYes)
					then DelDirFromPathSmart(sApp);
				end;
			end;
	end;
end;
