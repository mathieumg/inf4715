///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef STDAFX_H
#define STDAFX_H

#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifdef WIN32 
#include <windows.h>
#elif LINUX
#include "string.h"
#elif defined(_XBOX)
#  include <xtl.h>
#endif

// PhysX includes
#include <PxPhysicsAPI.h>
#include "common/PxIO.h"
#include "extensions/PxExtensionsAPI.h"
#include "extensions/PxRigidBodyExt.h"
#include "extensions/PxSimpleFactory.h"
#include "extensions/PxVisualDebuggerExt.h"
#include "foundation/PxAllocatorCallback.h"
#include "physxprofilesdk/PxProfileZoneManager.h"
#include "pxtask/PxCudaContextManager.h"

// STL includes
#include <stdexcept>
#include <fstream>
#include <unordered_map>

// C includes
#include <stdlib.h>
#include <stdio.h>

using namespace physx;
using namespace pxtask;

#endif // STDAFX_H
