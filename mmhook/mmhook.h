// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MMHOOKS64_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MMHOOKS64_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MMHOOK_EXPORTS
#define MMHOOK_API __declspec(dllexport)
#else
#define MMHOOK_API __declspec(dllimport)
#endif

#define LOG_ERROR 0
#define LOG_WARN 1
#define LOG_INFO 2

/*#if defined(MMHOOKS64_CPP)
bool bActivated = false;
#else
#endif*/

//extern MMHOOKS64_API int nmmhooks64;
