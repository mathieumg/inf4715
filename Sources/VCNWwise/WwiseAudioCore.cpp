///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief FMOD core module implementation
///

#include "Precompiled.h"
#include "WwiseAudioCore.h"

// Project includes

// Engine includes
#include "VCNUtils/Constants.h"
#include "VCNResources/ResourceCore.h"

// Wwise includes
#include <AK/SoundEngine/Common/AkMemoryMgr.h>                  // Memory Manager
#include <AK/SoundEngine/Common/AkModule.h>                     // Default memory and stream managers
#include <AK/SoundEngine/Common/IAkStreamMgr.h>                 // Streaming Manager
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Comm/AkCommunication.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>                // Music Engine
#include <AK/Tools/Common/AkPlatformFuncs.h>                    // Thread defines

VCN_TYPE( VCNWwiseAudioCore, VCNAudioCore ) ;

#define WIN32_BLOCKING_DEVICE_NAME		(AKTEXT("Win32 Blocking"))	// Default blocking device name.

// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook( size_t in_size )
	{
		return malloc( in_size );
	}
	void FreeHook( void * in_ptr )
	{
		free( in_ptr );
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
		)
	{
		return VirtualAlloc( in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect );
	}
	void VirtualFreeHook( 
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
		)
	{
		VirtualFree( in_pMemAddress, in_size, in_dwFreeType );
	}
#endif
}

class VCNWwiseCoreImpl 
	: protected AK::StreamMgr::IAkFileLocationResolver
	, protected AK::StreamMgr::IAkIOHookBlocking
{
public:

	/// Called at startup
	VCNBool Initialize()
	{
		//
		// Create and initialize an instance of the default memory manager. Note
		// that you can override the default memory manager with your own. Refer
		// to the SDK documentation for more information.
		//

		AkMemSettings memSettings;
		memSettings.uMaxNumPools = 20;

		if ( AK::MemoryMgr::Init( &memSettings ) != AK_Success )
		{
			VCN_ASSERT_FAIL( "Could not create the memory manager." );
			return false;
		}

		//
		// Create and initialize an instance of the default streaming manager. Note
		// that you can override the default streaming manager with your own. Refer
		// to the SDK documentation for more information.
		//

		AkStreamMgrSettings stmSettings;
		AK::StreamMgr::GetDefaultSettings( stmSettings );

		// Customize the Stream Manager settings here.

		if ( !AK::StreamMgr::Create( stmSettings ) )
		{
			assert( ! "Could not create the Streaming Manager" );
			return false;
		}

		//
		// Create a streaming device with blocking low-level I/O handshaking.
		// Note that you can override the default low-level I/O module with your own. Refer
		// to the SDK documentation for more information.        
		//
		AkDeviceSettings deviceSettings;
		AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );

		// Customize the streaming device settings here.

		// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
		// File Location Resolver (this I/O hook is also able to resolve file location).
		if ( !AK::StreamMgr::GetFileLocationResolver() )
			AK::StreamMgr::SetFileLocationResolver( this );

		// Create a device in the Stream Manager, specifying this as the hook.
		mDeviceId = AK::StreamMgr::CreateDevice( deviceSettings, this );

		//
		// Create the Sound Engine
		// Using default initialization parameters
		//

		AkInitSettings initSettings;
		AkPlatformInitSettings platformInitSettings;
		AK::SoundEngine::GetDefaultInitSettings( initSettings );
		AK::SoundEngine::GetDefaultPlatformInitSettings( platformInitSettings );

		extern HWND GLOBAL_WINDOW_HANDLE;

		platformInitSettings.uLEngineDefaultPoolSize = initSettings.uDefaultPoolSize = 2*1024*1024;
		platformInitSettings.hWnd = GLOBAL_WINDOW_HANDLE;

		if ( AK::SoundEngine::Init( &initSettings, &platformInitSettings ) != AK_Success )
		{
			assert( ! "Could not initialize the Sound Engine." );
			return false;
		}

		//
		// Initialize the music engine
		// Using default initialization parameters
		//

		AkMusicSettings musicInit;
		AK::MusicEngine::GetDefaultInitSettings( musicInit );

		if ( AK::MusicEngine::Init( &musicInit ) != AK_Success )
		{
			assert( ! "Could not initialize the Music Engine." );
			return false;
		}

#ifndef AK_OPTIMIZED
		//
		// Initialize communications (not in release build!)
		//
		AkCommSettings commSettings;
		AK::Comm::GetDefaultInitSettings( commSettings );
		AKRESULT res = AK::Comm::Init( commSettings );
		if ( res != AK_Success )
		{
			VCN_ASSERT_FAIL ( AKTEXT("AK::Comm::Init() returned AKRESULT %d. Communication between the Wwise authoring application and the game will not be possible."), res );
		}
#endif // AK_OPTIMIZED

		// Load system bank		
		AK::StreamMgr::SetCurrentLanguage( AKTEXT("English(US)") );

		AkBankID bankID;
		AKRESULT eResult = AK::SoundEngine::LoadBank( AKTEXT("Sounds\\Init.bnk"), AK_DEFAULT_POOL_ID, bankID );
		VCN_ASSERT( eResult == AK_Success );
		eResult = AK::SoundEngine::LoadBank( AKTEXT("Sounds\\Vicuna.bnk"), AK_DEFAULT_POOL_ID, bankID );
		VCN_ASSERT( eResult == AK_Success );

        //AK::SoundEngine::SetListenerScalingFactor(0x01, 0.1f);
        //AK::SoundEngine::SetListenerScalingFactor(0x01, 0.0f);

		return true;
	}

	/// Called when the core gets destroyed.
	VCNBool Uninitialize()
	{
#ifndef AK_OPTIMIZED
		// Terminate communications between Wwise and the game
		AK::Comm::Term();
#endif // AK_OPTIMIZED

		AK::MusicEngine::Term();
		AK::SoundEngine::Term();

		if ( AK::StreamMgr::GetFileLocationResolver() == this )
			AK::StreamMgr::SetFileLocationResolver( NULL );
		AK::StreamMgr::DestroyDevice( mDeviceId );

		if ( AK::IAkStreamMgr::Get() )
			AK::IAkStreamMgr::Get()->Destroy();

		// Terminate the Memory Manager
		AK::MemoryMgr::Term();

		return true;
	}

	/// Called once every frame
	VCNBool Process(const float elapsedTime)
	{
		// Process bank requests, events, positions, RTPC, etc.
		AK::SoundEngine::RenderAudio();
		return true;
	}

    void SetListenerPosition(Vector3& position, Vector3& lookAt)
    {
        AkListenerPosition listenerPosition;
        AkVector positionVector = { position[0], position[1], position[2] };
        AkVector orientationFront = { lookAt[0], lookAt[1], lookAt[2] };
        AkVector orientationTop = { 0.0f, 1.0f, 0.0f };
        
        listenerPosition.Position = positionVector;
        listenerPosition.OrientationFront = orientationFront;
        listenerPosition.OrientationTop = orientationTop;
        
        AK::SoundEngine::SetListenerPosition(listenerPosition);
    }

    void RegisterGameObj(VCNUInt objectId, const VCNString& helperName)
    {
        AK::SoundEngine::RegisterGameObj(objectId, VCN_W2A(helperName));
    }

    void SetPosition(VCNUInt objectId, Vector3& position, Vector3& orientation)
    {
        AkSoundPosition objectPosition;

        AkVector positionVector = { position[0], position[1], position[2] };
        AkVector orientationVector = { orientation[0], orientation[1], orientation[2] };

        objectPosition.Position = positionVector;
        objectPosition.Orientation = orientationVector;

        /*
        objectPosition.Position.X = 95.0f;
        objectPosition.Position.Y = objectPosition.Position.Z = 0;
        objectPosition.Orientation.Z = 1;
        objectPosition.Orientation.X = objectPosition.Orientation.Y = 0;
        */
        
        AK::SoundEngine::SetPosition(objectId, objectPosition);
    }

    void SetAttenuationScalingFactor(VCNUInt objectId, VCNFloat scale)
    {
        AK::SoundEngine::SetAttenuationScalingFactor(objectId, scale);
    }

    void PostEvent(const VCNString& eventName, int gameObjectID)
    {
        AK::SoundEngine::PostEvent(eventName.c_str(), gameObjectID);
    }

    void SetState(const VCNString& stateName, const VCNString& stateValue)
    {
        AK::SoundEngine::SetState(stateName.c_str(), stateValue.c_str());
    }

    void SetSwitch(const VCNString& switchName, const VCNString& switchValue, VCNUInt gameObjectID)
    {
        AK::SoundEngine::SetSwitch(switchName.c_str(), switchValue.c_str(), gameObjectID);
    }

protected:

	std::map< VCNString, HANDLE > mFileHandles;

	virtual AKRESULT Open( const AkOSChar* in_pszFileName, AkOpenMode in_eOpenMode, AkFileSystemFlags * in_pFlags, bool & io_bSyncOpen, AkFileDesc & out_fileDesc ) 
	{
		HANDLE hFile = ::CreateFile( in_pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return AK_InvalidFile;

		mFileHandles[ in_pszFileName ] = hFile;
		out_fileDesc.hFile = hFile;
		out_fileDesc.iFileSize = ::GetFileSize(hFile, NULL);;
		out_fileDesc.uSector = 0;
		out_fileDesc.deviceID = mDeviceId;
		out_fileDesc.pCustomParam = NULL;
		out_fileDesc.uCustomParamSize	= 0;
		
		return AK_Success;
	}

	virtual AKRESULT Open( AkFileID in_fileID, AkOpenMode in_eOpenMode, AkFileSystemFlags * in_pFlags, bool & io_bSyncOpen, AkFileDesc & out_fileDesc ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual AKRESULT Read( AkFileDesc & in_fileDesc, const AkIoHeuristics & in_heuristics, void * out_pBuffer, AkIOTransferInfo & in_transferInfo ) 
	{
		DWORD bytesRead = 0;
		BOOL status = ::ReadFile( in_fileDesc.hFile, out_pBuffer, in_transferInfo.uRequestedSize, &bytesRead, NULL );
		VCN_ASSERT( status && in_transferInfo.uRequestedSize == bytesRead );
		
		return status ? AK_Success : AK_Fail;
	}

	virtual AKRESULT Write( AkFileDesc & in_fileDesc, const AkIoHeuristics & in_heuristics, void * in_pData, AkIOTransferInfo & io_transferInfo ) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

	virtual AKRESULT Close( AkFileDesc & in_fileDesc ) 
	{
		::CloseHandle( in_fileDesc.hFile );
		return AK_Success;
	}

	virtual AkUInt32 GetBlockSize( AkFileDesc & in_fileDesc ) 
	{
		return 1;
	}

	virtual void GetDeviceDesc( AkDeviceDesc & out_deviceDesc ) 
	{
		if( mDeviceId != AK_INVALID_DEVICE_ID )
		{
			out_deviceDesc.deviceID       = mDeviceId;
			out_deviceDesc.bCanRead       = false;
			out_deviceDesc.bCanWrite      = false;
			AKPLATFORM::SafeStrCpy( out_deviceDesc.szDeviceName, WIN32_BLOCKING_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
			out_deviceDesc.uStringSize   = (AkUInt32)wcslen( out_deviceDesc.szDeviceName ) + 1;
		}
	}

	virtual AkUInt32 GetDeviceData() 
	{
		return 0; // sync
	}

private:

	AkDeviceID mDeviceId;

};

///////////////////////////////////////////////////////////////////////
///
/// Constructs Core
///
VCNWwiseAudioCore::VCNWwiseAudioCore()
	: mImpl( new VCNWwiseCoreImpl() )
{
}

///////////////////////////////////////////////////////////////////////
///
/// Destructs core
///
VCNWwiseAudioCore::~VCNWwiseAudioCore() 
{
	delete mImpl;
}

///////////////////////////////////////////////////////////////////////
///
/// Initialize FMOD system. Initializing FMOD can failed for many reasons
/// but we don't necessarily want the game to failed loading, so we
/// mark initialization as successfully. but any further called
/// to the core should be done silently.
///
/// @return none
///
VCNBool VCNWwiseAudioCore::Initialize() 
{
  if ( !VCNAudioCore::Initialize() )
    return false;

	return mImpl->Initialize();
}

///////////////////////////////////////////////////////////////////////
///
/// Releases FMOD.
///
/// @return true if everything was released successfully.none
///
VCNBool VCNWwiseAudioCore::Uninitialize()
{
	mImpl->Uninitialize();

  return VCNAudioCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
///
/// Updates FMOD system. We update 3D settings and tick the system.
///
/// @return true if everything went smoothly.
///
VCNBool VCNWwiseAudioCore::Process(const float elapsedTime)
{
	return mImpl->Process( elapsedTime );
}

///////////////////////////////////////////////////////////////////////
///
/// Creates a 2D sound. 
/// 
/// @param sndID    [IN] sound unique ID.
/// @param filename [IN] filename of the sound be loaded on disk.
/// @param loop     [IN] indicates if the sound shall be loop.
/// @param volume   [IN] initial sound volume.
/// @param playback [IN] playback type of the sound.
///
/// @return unique resource ID
///
const VCNResID VCNWwiseAudioCore::Create2DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
                                                      const VCNBool loop, const VCNFloat volume, 
                                                      PlaybackType playback)
{
	::MessageBeep(0);

  return kInvalidResID;
}

///////////////////////////////////////////////////////////////////////
///
/// Creates a 3D sound.
/// 
/// @param sndID        [IN] sound unique ID.
/// @param filename     [IN] filename of the sound be loaded on disk.
/// @param position     [IN] initial sound position
/// @param minDistance  [IN] min distance of the sound to be heard
/// @param maxDistance  [IN] max distance the sound gets attenuated.
/// @param loop         [IN] indicates if the sound shall be loop.
/// @param volume       [IN] initial sound volume.
/// @param playback     [IN] playback type of the sound.
///
/// @return resource unique id
///
const VCNResID VCNWwiseAudioCore::Create3DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
                                                      const Vector3& position, const VCNFloat minDistance, 
                                                      const VCNFloat maxDistance, const VCNBool loop, 
                                                      const VCNFloat volume, PlaybackType playback)
{
  ::MessageBeep(0);

  return kInvalidResID;
}

///////////////////////////////////////////////////////////////////////
///
/// Set the position of the listener in the game world.
/// 
void VCNWwiseAudioCore::SetListenerPosition(Vector3& position, Vector3& lookAt)
{
    mImpl->SetListenerPosition(position, lookAt);
}

///////////////////////////////////////////////////////////////////////
///
/// Register a game object with the sound engine.
/// 
void VCNWwiseAudioCore::RegisterGameObj(VCNUInt objectId, const VCNString& helperName)
{
    mImpl->RegisterGameObj(objectId, helperName);
}

///////////////////////////////////////////////////////////////////////
///
/// Set the position of a game object in the game world.
/// 
void VCNWwiseAudioCore::SetPosition(VCNUInt objectId, Vector3& position, Vector3& orientation)
{
    mImpl->SetPosition(objectId, position, orientation);
}

///////////////////////////////////////////////////////////////////////
///
/// Set attenuation based on distance of game object.
/// 
void VCNWwiseAudioCore::SetAttenuationScalingFactor(VCNUInt objectId, VCNFloat scale)
{
    mImpl->SetAttenuationScalingFactor(objectId, scale);
}

///////////////////////////////////////////////////////////////////////
///
/// Send an event to the sound engine.
/// 
void VCNWwiseAudioCore::PostEvent(const VCNString& eventName, VCNUInt gameObjectID)
{
    mImpl->PostEvent(eventName, gameObjectID);
}

///////////////////////////////////////////////////////////////////////
///
/// Send a state to the sound engine.
/// 
void VCNWwiseAudioCore::SetState(const VCNString& stateName, const VCNString& stateValue)
{
    mImpl->SetState(stateName, stateValue);
}

///////////////////////////////////////////////////////////////////////
///
/// Send a state to the sound engine.
/// 
void VCNWwiseAudioCore::SetSwitch(const VCNString& switchName, const VCNString& switchValue, VCNUInt gameObjectID)
{
    mImpl->SetSwitch(switchName, switchValue, gameObjectID);
}

///////////////////////////////////////////////////////////////////////
///
/// Stops all sound at once.
/// 
void VCNWwiseAudioCore::StopAll()
{
  VCN_ASSERT_FAIL( "Not implemented" );
}

///////////////////////////////////////////////////////////////////////
///
/// Release all sounds.
///
void VCNWwiseAudioCore::DestroyAll()
{
 VCN_ASSERT_FAIL( "Not implemented" );
}

///////////////////////////////////////////////////////////////////////
///
/// Sets viewer settings. Viewer settings are applied in the process tick.
/// 
/// @param viewerPosition   [IN] viewer world position in 3D
/// @param velocity         [IN] viewer current velocity.
/// @param forwardDirection [IN] viewer facing/forward direction.
/// @param upDirection      [IN] viewer up direction.
///
void VCNWwiseAudioCore::SetViewerSettings(const Vector3& viewerPosition, const Vector3& velocity, 
                                         const Vector3& forwardDirection, const Vector3& upDirection)
{
  //VCN_ASSERT_FAIL( "Not implemented" );
}

///////////////////////////////////////////////////////////////////////
///
/// Apply 3D world settings such as distance factor.
/// Distance factor, maps FMOD units into your unit system.
/// If you use centimeter, then sets 100.0f as the distance factor.
/// 
/// @param distanceFactor [IN] Distance factor to apply.
///
/// @return none
///
void VCNWwiseAudioCore::Set3DSettings(VCNFloat distanceFactor)
{
  //VCN_ASSERT_FAIL( "Not implemented" );
}
