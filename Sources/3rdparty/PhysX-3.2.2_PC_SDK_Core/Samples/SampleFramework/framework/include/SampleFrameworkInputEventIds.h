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
#ifndef _SAMPLE_FRAMEWORK_INPUT_EVENT_IDS_H
#define _SAMPLE_FRAMEWORK_INPUT_EVENT_IDS_H

static const float GAMEPAD_ROTATE_SENSITIVITY = 0.7f;
static const float GAMEPAD_DEFAULT_SENSITIVITY = 1.0f;

// InputEvents used by SampleApplication
enum SampleFrameworkInputEventIds
{
	CAMERA_SHIFT_SPEED,
	CAMERA_MOVE_LEFT,
	CAMERA_MOVE_RIGHT,
	CAMERA_MOVE_UP,
	CAMERA_MOVE_DOWN,
	CAMERA_MOVE_FORWARD,
	CAMERA_MOVE_BACKWARD,
	CAMERA_SPEED_INCREASE,
	CAMERA_SPEED_DECREASE,

	CAMERA_MOUSE_LOOK,
	CAMERA_MOVE_BUTTON,

	CAMERA_GAMEPAD_ROTATE_LEFT_RIGHT,
	CAMERA_GAMEPAD_ROTATE_UP_DOWN,
	CAMERA_GAMEPAD_MOVE_LEFT_RIGHT,
	CAMERA_GAMEPAD_MOVE_FORWARD_BACK,

	CAMERA_JUMP,
	CAMERA_CROUCH,
	CAMERA_CONTROLLER_INCREASE,
	CAMERA_CONTROLLER_DECREASE,

	NUM_SAMPLE_FRAMEWORK_INPUT_EVENT_IDS, 
};

#endif

