///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game precompiled header
///

#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#pragma once

// Disable warning messages 4279
#pragma warning( disable : 4279 )

// Windows application
#include <Windows.h>
#include <WindowsX.h>
#include <Shlwapi.h>

// STL
#include <algorithm>
#include <array>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <tuple>
#include <vector>
#include <iomanip>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <iostream>

// Standard headers
#include <tchar.h>
#include <cstdio>
#include <ctime>

// AntTweakBar includes
// @TODO Create VCNUI module
#include <AntTweakBar.h>

// MS XML
#pragma warning( push )
#pragma warning( disable : 4192 )
#include <atlcomcli.h>
#import <msxml6.dll> raw_interfaces_only
#pragma warning( pop )

#include "resource.h"

#endif // PRECOMPILED_H
