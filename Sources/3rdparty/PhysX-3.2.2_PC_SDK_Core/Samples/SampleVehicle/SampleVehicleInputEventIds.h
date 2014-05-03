// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
#ifndef _SAMPLE_VEHICLE_INPUT_EVENT_IDS_H
#define _SAMPLE_VEHICLE_INPUT_EVENT_IDS_H

#include <SampleBaseInputEventIds.h>

// InputEvents used by SampleVehicle
enum SampleVehicleInputEventIds
{
	SAMPLE_VEHICLE_FIRST_ID = NUM_SAMPLE_BASE_INPUT_EVENT_IDS,

	ACCELERATE,
	BRAKE,
	ACCELERATE_BRAKE,
	STEER_LEFT,
	STEER_RIGHT,
	STEER,

	HANDBRAKE_PAD,
	GEAR_UP_PAD,
	GEAR_DOWN_PAD,
	HANDBRAKE_KBD,
	GEAR_UP_KBD,
	GEAR_DOWN_KBD,
	
	TANK_THRUST_LEFT,
	TANK_THRUST_RIGHT,
	TANK_BRAKE_LEFT,
	TANK_BRAKE_RIGHT,

	AUTOMATIC_GEAR ,
	DEBUG_RENDER_FLAG,
	DEBUG_RENDER_WHEEL ,
	DEBUG_RENDER_ENGINE,
	RETRY,
	FIX_CAR,
	CAMERA_LOCK,
	W3MODE,

	CAMERA_KBD_ROTATE_LEFT,
	CAMERA_KBD_ROTATE_RIGHT,
	CAMERA_KBD_ROTATE_UP,
	CAMERA_KBD_ROTATE_DOWN,

	NUM_SAMPLE_VEHICLE_INPUT_EVENT_IDS,
};

#endif
