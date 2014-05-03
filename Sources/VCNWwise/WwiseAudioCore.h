///
/// Copyright (C) 2013 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna Wwise integration
///

#pragma once

#include "VCNAudio/AudioCore.h"
#include "VCNUtils/StringUtils.h"

class VCNWwiseCoreImpl;


class VCNWwiseSoundData : VCNResource
{

public:
	VCNWwiseSoundData(int gameObjectID);

	inline void setEventData(const VCNString& name)
	{
		mEventName = name;
	}


	VCNString getEventName() const { return mEventName; }
	void setEventName(const VCNString& val) { mEventName = val; }

	VCNString getStateName() const { return mStateName; }
	void setStateName(const VCNString& val) { mStateName = val; }

	VCNString getStateValue() const { return mStateValue; }
	void setStateValue(const VCNString& val) { mStateValue = val; }

	VCNString getSwitchName() const { return mSwitchName; }
	void setSwitchName(const VCNString& val) { mSwitchName = val; }

	VCNString getSwitchValue() const { return mSwitchValue; }
	void setSwitchValue(const VCNString& val) { mSwitchValue = val; }

private:
	int mGameObjectID; // Wwise gameobject id
	VCNString mEventName;
	VCNString mStateName;
	VCNString mStateValue;
	VCNString mSwitchName;
	VCNString mSwitchValue;
	
};


class VCNWwiseAudioCore : public VCNAudioCore
{
	VCN_CLASS;

public:

	/// Default constructor.
	VCNWwiseAudioCore();

	/// Default constructor.
	virtual ~VCNWwiseAudioCore();

	// VCNCore Interface

	/// Called at startup
	virtual VCNBool Initialize() override;

	/// Called when the core gets destroyed.
	virtual VCNBool Uninitialize() override;

	/// Called once every frame
	virtual VCNBool Process(const float elapsedTime) override;

	// VCNAudioCore Interface

	/// Sets world 3D settings
	virtual void Set3DSettings(VCNFloat distanceFactor) override;

	/// Sets 3D/World settings.
	virtual void SetViewerSettings(
		const Vector3& viewerPosition, 
		const Vector3& velocity, 
		const Vector3& forwardDirection, 
		const Vector3& upDirection) override;

	/// Creates a 2D sound.
	virtual const VCNResID Create2DSoundEmitter(
		const VCNString& sndID, 
		const VCNString& filename, 
		const VCNBool loop, 
		const VCNFloat volume, 
		PlaybackType playback) override;  

	/// Create a 3D sound
	virtual const VCNResID Create3DSoundEmitter(
		const VCNString& sndID, 
		const VCNString& filename, 
		const Vector3& position, 
		const VCNFloat minDistance, 
		const VCNFloat maxDistance, 
		const VCNBool loop, 
		const VCNFloat volume, 
		PlaybackType playback) override;

    /// Set location of listener in game world.
    virtual void SetListenerPosition(Vector3& position, Vector3& lookAt) override;

    /// Register a game object with the sound engine.
    virtual void RegisterGameObj(VCNUInt objectId, const VCNString& helperName) override;

    /// Set the position of a game object in the game world.
    virtual void SetPosition(VCNUInt objectId, Vector3& position, Vector3& orientation) override;

    /// Set attenuation based on distance of game object.
    virtual void SetAttenuationScalingFactor(VCNUInt objectId, VCNFloat scale) override;
    
    /// Send an event to the sound engine.
    virtual void PostEvent(const VCNString& eventName, VCNUInt gameObjectID) override;

    /// Send a state to the sound engine.
    virtual void SetState(const VCNString& stateName, const VCNString& stateValue) override;

    /// Send a switch to the sound engine.
    virtual void SetSwitch(const VCNString& switchName, const VCNString& switchValue, VCNUInt gameObjectID) override;

	/// Stop all audio at once
	virtual void StopAll() override;

	/// Clean up!
	virtual void DestroyAll() override;

private:

	VCNWwiseCoreImpl* mImpl;

};
