///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game specific defines smart pointer types
///
/// @remark Changing this file might recompile everything
///

#ifndef GAMESMARTTYPES_H
#define GAMESMARTTYPES_H

#pragma once

#include <memory>

// Forward declarations
class CameraController;
class DebugGraph;
class HUD;
class Inventory;

// Smart pointer predefined types
typedef std::shared_ptr<CameraController> CameraControllerPointer;
typedef std::shared_ptr<DebugGraph> DebugGraphPointer;
typedef std::shared_ptr<HUD> HUDPointer;
typedef std::shared_ptr<Inventory> InventoryPointer;

// Smart pointer list predefined types

#endif // GAMESMARTTYPES_H
