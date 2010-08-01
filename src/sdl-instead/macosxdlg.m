#include <AppKit/NSOpenPanel.h>
#include <Foundation/NSString.h>
static char *file_name[4096];

char *macosx_open_file_dialog(void) 
{
	const char *filename;
	NSOpenPanel * panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setAllowsMultipleSelection:NO];

	if ([panel runModalForTypes:nil] == NSOKButton) {
#ifdef __POWERPC__
		filename = [[panel filename] cString];
#else
		filename = [[panel filename] cStringUsingEncoding:NSUTF8StringEncoding];
#endif
		strcpy(file_name, filename);
		return file_name;
	}
	return NULL;
}
