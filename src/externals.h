#include "instead/system.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/html5.h"
#endif

#ifdef _USE_HARFBUZZ
#include <hb.h>
#endif
