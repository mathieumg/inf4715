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
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef SAMPLE_USER_INPUT_DEFINES_H
#define SAMPLE_UTILS_H

#if defined(RENDERER_WINDOWS)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) winKey
#elif defined (RENDERER_XBOX360)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) xbox360key
#elif defined (RENDERER_PSP2)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) psp2Key
#elif defined (RENDERER_PS3)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) ps3Key
#elif defined (RENDERER_ANDROID)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) andrKey
#elif defined (RENDERER_MACOSX)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) osxKey
#elif defined (RENDERER_IOS)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) iosKey
#elif defined (RENDERER_LINUX)
#	define OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey) linuxKey
#else
# error "Platform not supported!" 
#endif


#define DIGITAL_INPUT_EVENT_DEF_STRING(var, helpString, winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)   {\
	const physx::PxU16 eventKey = OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey);\
	const SampleFramework::InputEvent* retVal = (SampleFramework::SamplePlatform::platform()->getSampleUserInput()->registerInputEvent(SampleFramework::InputEvent(var, helpString, false), eventKey)); \
	if(retVal) inputEvents.push_back(retVal); }

#define ANALOG_INPUT_EVENT_DEF_STRING(var, helpString, sensitivity,  winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)  {\
	const physx::PxU16 eventKey = OS_EVENT_KEY(winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey);\
	const SampleFramework::InputEvent* retVal = (SampleFramework::SamplePlatform::platform()->getSampleUserInput()->registerInputEvent(SampleFramework::InputEvent(var, helpString, true, sensitivity), eventKey)); \
	if(retVal) inputEvents.push_back(retVal); }

#if defined(RENDERER_ANDROID) || defined(RENDERER_IOS)
#define TOUCH_INPUT_EVENT_DEF_STRING(var, helpString, caption, andrKey, iosKey)   {\
	const physx::PxU16 eventKey = OS_EVENT_KEY(andrKey, andrKey, andrKey, andrKey, iosKey, iosKey, iosKey, iosKey);\
	const SampleFramework::InputEvent* retVal = (SampleFramework::SamplePlatform::platform()->getSampleUserInput()->registerTouchInputEvent(SampleFramework::InputEvent(var, helpString, false), eventKey, caption)); \
	if(retVal) inputEvents.push_back(retVal); }
#else
#define TOUCH_INPUT_EVENT_DEF_STRING(var, helpString, caption, andrKey, iosKey) 
#endif

#define DIGITAL_INPUT_EVENT_DEF(var, winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)\
		DIGITAL_INPUT_EVENT_DEF_STRING(var, #var, winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)
#define ANALOG_INPUT_EVENT_DEF(var, sensitivity,  winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)\
		ANALOG_INPUT_EVENT_DEF_STRING(var, #var, sensitivity,  winKey, xbox360key, ps3Key, andrKey, osxKey, psp2Key, iosKey, linuxKey)
#define TOUCH_INPUT_EVENT_DEF(var, caption, andrKey, iosKey)\
		TOUCH_INPUT_EVENT_DEF_STRING(var, #var, caption, andrKey, iosKey)

#endif
