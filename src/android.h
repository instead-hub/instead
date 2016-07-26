#include <jni.h>
#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)
extern void JNICALL JAVA_EXPORT_NAME(SDLActivity_nativeSave)(JNIEnv *env, jobject obj) {
cfg_save();
}

extern void JNICALL JAVA_EXPORT_NAME(SDLActivity_nativeStop)(JNIEnv *env, jobject obj) {
game_done(0);	
exit(0);
}
