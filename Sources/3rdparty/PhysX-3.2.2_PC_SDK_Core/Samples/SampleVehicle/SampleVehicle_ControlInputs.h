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


#ifndef SAMPLE_VEHICLE_CONTROL_INPUTS_H
#define SAMPLE_VEHICLE_CONTROL_INPUTS_H

#include "common/PxPhysXCommon.h"

using namespace physx;

class SampleVehicle_ControlInputs
{
public:

	SampleVehicle_ControlInputs();
	~SampleVehicle_ControlInputs();

	//Camera inputs
	void					setRotateY(const PxF32 f) 					{mCameraRotateInputY=f;}
	void					setRotateZ(const PxF32 f) 					{mCameraRotateInputZ=f;}
	PxF32					getRotateY() const							{return mCameraRotateInputY;}
	PxF32					getRotateZ() const							{return mCameraRotateInputZ;}

	//Driving inputs - keys
	void					setAccelKeyPressed(const bool b) 			{mAccelKeyPressed=b;}
	void					setBrakeKeyPressed(const bool b) 			{mBrakeKeyPressed=b;}
	void					setHandbrakeKeyPressed(const bool b)		{mHandbrakeKeyPressed=b;}
	void					setSteerLeftKeyPressed(const bool b)		{mSteerLeftKeyPressed=b;}
	void					setSteerRightKeyPressed(const bool b)		{mSteerRightKeyPressed=b;}
	void					setGearUpKeyPressed(const bool b) 			{mGearUpKeyPressed=b;}
	void					setGearDownKeyPressed(const bool b)			{mGearDownKeyPressed=b;}
	bool					getAccelKeyPressed() const					{return mAccelKeyPressed;}
	bool					getBrakeKeyPressed() const					{return mBrakeKeyPressed;}
	bool					getHandbrakeKeyPressed() const				{return mHandbrakeKeyPressed;}
	bool					getSteerLeftKeyPressed() const				{return mSteerLeftKeyPressed;}
	bool					getSteerRightKeyPressed() const				{return mSteerRightKeyPressed;}
	bool					getGearUpKeyPressed() const					{return mGearUpKeyPressed;}
	bool					getGearDownKeyPressed() const				{return mGearDownKeyPressed;}

	//Driving inputs - gamepad
	void					setAccel(const PxF32 f) 					{mAccel=f;}
	void					setBrake(const PxF32 f) 					{mBrake=f;}
	void					setSteer(const PxF32 f) 					{mSteer=f;}
	void					setGearUp(const bool b) 					{mGearup=b;}
	void					setGearDown(const bool b) 					{mGeardown=b;}
	void					setHandbrake(const bool b) 					{mHandbrake=b;}
	PxF32					getAccel() const							{return mAccel;}
	PxF32					getBrake() const							{return mBrake;}
	PxF32					getSteer() const							{return mSteer;}
	bool					getGearUp() const							{return mGearup;}
	bool					getGearDown() const							{return mGeardown;}
	bool					getHandbrake() const						{return mHandbrake;}

private:

	//Camera inputs.
	PxF32			mCameraRotateInputY;
	PxF32			mCameraRotateInputZ;

	//Driving inputs - keys
	bool			mAccelKeyPressed;
	bool			mBrakeKeyPressed;
	bool			mHandbrakeKeyPressed;
	bool			mSteerLeftKeyPressed;
	bool			mSteerRightKeyPressed;
	bool			mGearUpKeyPressed;
	bool			mGearDownKeyPressed;

	//Driving inputs - gamepad
	PxF32			mAccel;
	PxF32			mBrake;
	PxF32			mSteer;
	bool			mGearup;
	bool			mGeardown;
	bool			mHandbrake;
};

#endif //SAMPLE_VEHICLE_CONTROL_INPUTS_H