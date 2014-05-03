//DLLMain.cpp
#include "DllMain.h"
#include "mxsDebugCRT.h"
//#include "MxsStopWatch.h"
#include "MxsUnitAssert.h"
#include <plugapi.h>

HINSTANCE hInstance;

// ========================================================
// Grab onto this DLL's instance handle
BOOL WINAPI DllMain(HINSTANCE DLLhinst, DWORD fdwReason, LPVOID lpvReserved)
{
	UNUSED_PARAMETER(lpvReserved);

	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		MaxSDK::Util::UseLanguagePackLocale();
		hInstance = DLLhinst;
		break;
	default:
		break;
	}
	return TRUE;
}
// ========================================================
extern void InitMxsVLD();

__declspec(dllexport) void LibInit()
{
	InitMxsDebugCRT();
	//InitMxsStopWatch();
	InitMxsAssert();
	InitMxsVLD();
}

__declspec(dllexport) const MCHAR* LibDescription()
{
	return _M("Maxscript Diagnostic Debugging Routines");
}

__declspec(dllexport) ULONG LibVersion()
{
	// This macro found in plugapi.h
	return VERSION_3DSMAX;
}

// Formats a message string using the specified message and variable
// list of arguments.
void GetFormattedMessage(DWORD last_error)
{
	// Retrieve the system error message for the last-error code

	MCHAR lpMsgBuf[MAX_PATH] = {0};

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		last_error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpMsgBuf,
		MAX_PATH,
		NULL );

	// Display the error message and exit the process
	DebugPrint(_M("%s"), &lpMsgBuf);
}