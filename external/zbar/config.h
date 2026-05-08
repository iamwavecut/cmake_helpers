/* intentionally empty - all config is passed via CMake defines */

#if defined(_WIN32) && !defined(timeGetTime)
/* WIN32_LEAN_AND_MEAN excludes mmsystem.h from windows.h,
   but zbar/timer.h needs timeGetTime() with correct calling
   convention (__stdcall) to link on x86. */
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) unsigned long __stdcall timeGetTime(void);
#ifdef __cplusplus
}
#endif
#endif
