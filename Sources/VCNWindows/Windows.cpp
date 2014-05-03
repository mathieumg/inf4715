///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Windows service implementation
///

#include "Precompiled.h"
#include "Windows.h"

#include "VCNCore/CoreTimer.h"
#include "VCNLog/Log.h"
#include "VCNUtils/StackWalker.h"

#include <shlwapi.h>

VCN_TYPE( VCNWindows, VCNSystem ) ;

class VCNLogStackWalker : public StackWalker
{
public:
	VCNLogStackWalker() : StackWalker(), skipCounter(0) {}

protected:

	virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
	{
		// Skip the first two functions on the stack cause they are part of the stack print methods
		/*if (skipCounter++ < 2)
		return;*/

		if ( strlen(entry.moduleName) > 0 )
		{
			VCNLog << L"[" << entry.moduleName << L"!" << entry.undFullName << L"]\n";
		}
		if ( strlen(entry.lineFileName) > 0 )
		{
			VCNLog << entry.lineFileName << L"(" << entry.lineNumber << L")\n";
		}
	}

	int skipCounter;
};

LONG WINAPI Win32FaultHandler(struct _EXCEPTION_POINTERS*  ExInfo)
{ 
	VCNTChar* FaultTx = VCNTXT("");
	switch(ExInfo->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		FaultTx = VCNTXT("ACCESS VIOLATION"); 
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		FaultTx = VCNTXT("DATATYPE MISALIGNMENT");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		FaultTx = VCNTXT("DIVIDE BY ZERO"); 
		break;
	default: 
		FaultTx = VCNTXT("(unknown)");
		break;
	}

	int    wsFault    = ExInfo->ExceptionRecord->ExceptionCode;
	PVOID  CodeAdress = ExInfo->ExceptionRecord->ExceptionAddress;

	VCNLog.LogMessage( kError, VCNTXT("****************************************************\n") );
	VCNLog.LogMessage( kError, VCNTXT("*** A Program Fault occurred:\n") );
	VCNLog.LogMessageF( kError, VCNTXT("*** Error code %08X: %s\n"), wsFault, FaultTx );
	VCNLog.LogMessage( kError, VCNTXT("****************************************************\n") );
	VCNLog.LogMessageF( kError, VCNTXT("***   Address: %08X\n"), (int)CodeAdress );
	VCNLog.LogMessageF( kError, VCNTXT("***     Flags: %08X\n"), ExInfo->ExceptionRecord->ExceptionFlags );

	VCNLogStackWalker sw;
	sw.ShowCallstack(GetCurrentThread(), ExInfo->ContextRecord);

	VCN_FINAL_ASSERT( false, VCNTXT("A fatal error occurred: %s\nSee log file for more info."), FaultTx );

	return EXCEPTION_EXECUTE_HANDLER;
}


///
/// Used to declare OS specific handles since we don't want to expose them to 
/// the user since it might requires additional includes.
/// 
struct VCNWindowsImpl
{
	VCNWindowsImpl()
		: mFirstMemStat(0)
		, mMinMemStat(0)
		, mMaxMemStat(0)
		, mASecond(0.0f)
		, mDMPS(0)
		, mSMemStat(0)
		, mFMemStat(0)
	{
	}

	friend class VCNWindows;

	HMODULE     mAppHandle;
	HANDLE      mCurrentProcess;

	VCNUInt64   mFirstMemStat;
	VCNUInt64   mMinMemStat;
	VCNUInt64   mMaxMemStat;

	VCNFloat32  mASecond;
	VCNUInt64   mFMemStat;
	VCNUInt64   mSMemStat;
	VCNFloat32  mDMPS;
};

///////////////////////////////////////////////////////////////////////
VCNWindows::VCNWindows(void)
	: mImpl( new VCNWindowsImpl() )
{
	mImpl->mAppHandle = ::GetModuleHandle( NULL );
	mImpl->mCurrentProcess = ::GetCurrentProcess();
}


///////////////////////////////////////////////////////////////////////
VCNWindows::~VCNWindows(void)
{
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNWindows::Initialize()
{
	if ( !VCNSystem::Initialize() )
		return false;

	mImpl->mFirstMemStat = mImpl->mMinMemStat = mImpl->mMaxMemStat = GetUsedMemory();

	// Install a unhandled exception handler
	SetUnhandledExceptionFilter( (LPTOP_LEVEL_EXCEPTION_FILTER)Win32FaultHandler );

	return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNWindows::Uninitialize()
{
	return VCNSystem::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNWindows::Process(const float elapsedTime)
{
	const VCNUInt64 memStat = GetUsedMemory();

	mImpl->mMinMemStat = std::min( mImpl->mMinMemStat, memStat );
	mImpl->mMaxMemStat = std::max( mImpl->mMaxMemStat, memStat );

	mImpl->mASecond += elapsedTime;
	if ( mImpl->mASecond >= 1.0f )
	{
		mImpl->mDMPS = mImpl->mSMemStat / 1024.0f / 1024.0f / mImpl->mASecond;
		mImpl->mSMemStat = 0LL;
		mImpl->mASecond -= 1.0f;
	}
	else
	{
		if (memStat > mImpl->mFMemStat)
		{
			mImpl->mSMemStat += memStat - mImpl->mFMemStat;
		}
	}

	mImpl->mFMemStat = memStat;

	return true;
}

///////////////////////////////////////////////////////////////////////
VCNUInt64 VCNWindows::GetUsedMemory() const
{
	PROCESS_MEMORY_COUNTERS pmc;

	::GetProcessMemoryInfo( mImpl->mCurrentProcess, &pmc, sizeof(pmc) );

	return pmc.WorkingSetSize;
}


///////////////////////////////////////////////////////////////////////
VCNUInt64 VCNWindows::GetFreeMemory() const
{
	MEMORYSTATUS memoryInfo;
	MEMORYSTATUSEX memoryInfoEx;

	if ( ::GlobalMemoryStatusEx( &memoryInfoEx ) )
	{
		return memoryInfoEx.ullAvailPhys;
	}
	else
	{
		::GlobalMemoryStatus( &memoryInfo );
		return memoryInfo.dwAvailPhys;
	}

	return 0LL;
}

///////////////////////////////////////////////////////////////////////
VCNUInt64 VCNWindows::GetMinUsedMemory() const 
{
	return mImpl->mMinMemStat;
}

///////////////////////////////////////////////////////////////////////
VCNUInt64 VCNWindows::GetMaxUsedMemory() const 
{
	return mImpl->mMaxMemStat;
}

///////////////////////////////////////////////////////////////////////
VCNFloat32 VCNWindows::GetDMPS() const 
{
	return mImpl->mDMPS;
}

void VCNWindows::FindFilesRecursively(const VCNTChar* lpFolder, const VCNTChar* lpFilePattern, std::function<void(const VCNTChar* path)> func)
{
	TCHAR szFullPattern[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	// first we are going to process any subdirectories
	PathCombine(szFullPattern, lpFolder, _T("*"));
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 &&
				StrCmp( FindFileData.cFileName, _T(".") ) && StrCmp( FindFileData.cFileName, _T("..") ) )
			{
				// found a subdirectory; recurse into it
				PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);

				FindFilesRecursively(szFullPattern, lpFilePattern, func);
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
	// now we are going to look for the matching files
	PathCombine(szFullPattern, lpFolder, lpFilePattern);
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// found a file; do something with it
				PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
				func( szFullPattern );
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
}
