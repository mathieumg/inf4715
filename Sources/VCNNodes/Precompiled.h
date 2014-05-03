///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Node module precompiled header
///

#pragma once

#include <Windows.h>

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
#include <algorithm>
#include <unordered_map>
#include <fstream>

// C includes
#include <cstdarg>
