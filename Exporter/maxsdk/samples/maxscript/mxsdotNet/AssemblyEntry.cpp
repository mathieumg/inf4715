#include "stdafx.h"

extern void LibInit(); // mxsdotNet_main.cpp
extern int LibShutdown(); // mxsdotNet_main.cpp

namespace MaxSDK
{
	namespace Maxscript
	{
		public ref class AssemblyEntry
		{
		public:
			static void AssemblyLoad();
			static void AssemblyMain();
			static void AssemblyInitializationCleanup();
			static void AssemblyShutdown();
		};

		void AssemblyEntry::AssemblyLoad()
		{
			// do nothing.
			// However calling this will be sufficient to construct all global variables (including static global variables). 
			// For maxscript plugins, this two-phase initialization MUST happen. 
			// That is global variables must be constructed before the standard initialization methods are called.
			// This is because in native code, calling LoadLibrary will be sufficient to construct all global and static global variables.
			// In .NET, calling Assembly.Load will NOT construct these global and static global variables. First what must happen is that managed
			// code must first get called. When the first managed code is executed, only then will the native global variables get constructed.
		}
		void AssemblyEntry::AssemblyMain()
		{
			LibInit();
		}
		void AssemblyEntry::AssemblyInitializationCleanup()
		{
			// do nothing
		}
		void AssemblyEntry::AssemblyShutdown()
		{
			LibShutdown();
		}
	}
}

