#include <akncommondialogs.h> // For single function calls
#include <eikapp.h>
#include <e32base.h>
#include <sdlapp.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <string.h>
#include <bautils.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
FILE* mystdout = NULL;
FILE* mystderr = NULL;
#ifdef _USE_BROWSE
char* desc2str(const TDesC& aDescriptor)
{
	static char fname[256];
	TInt length = aDescriptor.Length();
	HBufC8* buffer = HBufC8::NewLC(length);
	buffer->Des().Copy(aDescriptor);
	char* str = fname; 
	Mem::Copy(str, buffer->Ptr(), length);
	str[length] = '\0';
	CleanupStack::PopAndDestroy(buffer);
	return str;
}
extern "C" {

char s60_data[] = "E:\\data\\instead";

char *get_file_name(void)
{
	TFileName FileName;
	if (!AknCommonDialogs::RunSelectDlgLD(FileName, 0))
		return NULL;
	return desc2str(FileName);
}
void get_drives(char* drivelist) 
{
	RFs& fs = CEikonEnv::Static()->FsSession();
	TPtr8 ptr((unsigned char*) drivelist,26);
	TDriveList list;
	fs.DriveList(list);
	ptr.Copy(list);
}

}
#endif
#if 1
int	 isFolder(const char * filename)
{
	TBool folder = EFalse;
	TPtrC8 ptr((unsigned char*) filename);
	TFileName fname;
	fname.Copy(ptr);
	for(TInt loop = 0; loop<fname.Length(); loop++) {
		if(fname[loop] == '/')
			fname[loop] = '\\';
	}
	TInt res = BaflUtils::IsFolder(CEikonEnv::Static()->FsSession(), fname, folder);
	return folder;
}
#endif
class CSymbianApp : public CSDLApp {
public:
	CSymbianApp();
	~CSymbianApp();
	void PreInitializeAppL()
	{
		mystdout = fopen("c:\\data\\instead\\stdout.txt","w");
		mystderr = fopen("c:\\data\\instead\\stderr.txt","w");
		*stdout = *mystdout;
		*stderr = *mystderr;

		char drives[26];
		char testpath[] = "C:/data/instead/appdata/stead";
		get_drives(drives);

		for (int drive = 0; drive < 26; drive++) {
			if (!drives[drive])
				continue;
			testpath[0] = drive + 'A';
			if (isFolder(testpath)) {
				s60_data[0] = 'A' + drive;
				break;
			}
		}
	}
	TUid AppDllUid() const;
};

#ifdef EPOC_AS_APP
// this function is called automatically by the SymbianOS to deliver the new CApaApplication object
#if !defined (UIQ3) && !defined (S60V3)
EXPORT_C 
#endif
CApaApplication* NewApplication() {
	return new CSymbianApp;
}

#if defined (UIQ3) || defined (S60V3)
#include <eikstart.h>
// E32Main() contains the program's start up code, the entry point for an EXE.
GLDEF_C TInt E32Main() {
 	return EikStart::RunApplication(NewApplication);
}
#endif

#endif // EPOC_AS_APP

#if !defined (UIQ3) && !defined (S60V3)
GLDEF_C  TInt E32Dll(TDllReason) {
	return KErrNone;
}
#endif

CSymbianApp::CSymbianApp() 
{
}

CSymbianApp::~CSymbianApp() 
{
}

#if defined (UIQ3)
#include <gp2xpectrum.rsg>
/**
 * Returns the resource id to be used to declare the views supported by this UIQ3 app
 * @return TInt, resource id
 */
TInt CSymbianApp::ViewResourceId() {
	return R_SDL_VIEW_UI_CONFIGURATIONS;
}
#endif

/**
 *   Responsible for returning the unique UID of this application
 * @return unique UID for this application in a TUid
 **/
TUid CSymbianApp::AppDllUid() const {
	return TUid::Uid(0xA0020410);
}
