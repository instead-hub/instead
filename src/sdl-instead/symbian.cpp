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

void s60delay(int u)
{
	if (u > 0) {
		User::AfterHighRes(u);
	}
}

char s60_data[] = "E:\\data\\instead";

#ifdef _USE_BROWSE
char *get_file_name(void)
{
	TFileName FileName;
	if (!AknCommonDialogs::RunSelectDlgLD(FileName, 0))
		return NULL;
	return desc2str(FileName);
}
#endif
}
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

		s60_data[0] = BitmapStoreName()[0];
		fprintf(stderr,"Using data from: %s\n", s60_data);
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
