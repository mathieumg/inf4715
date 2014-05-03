///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief FMOD core module implementation
///

#include "Precompiled.h"
#include "FMODAudioCore.h"

// Project includes
#include "VCNFMODSound/FMODSoundEmitter2D.h"
#include "VCNFMODSound/FMODSoundEmitter3D.h"

// Engine includes
#include "VCNUtils/Constants.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNFMODAudioCore, VCNAudioCore ) ;

///////////////////////////////////////////////////////////////////////
///
/// Constructs Core
///
VCNFMODAudioCore::VCNFMODAudioCore()
  : mFMODSystem(NULL)
  , mFailedInitOutput(false)
{
}

///////////////////////////////////////////////////////////////////////
///
/// Destructs core
///
VCNFMODAudioCore::~VCNFMODAudioCore() 
{
  mFMODSystem->close();
  mFMODSystem->release();
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
VCNBool VCNFMODAudioCore::Initialize() 
{
  if ( !VCNAudioCore::Initialize() )
    return false;

  // Create the main system object.
  FMOD_RESULT result = FMOD::System_Create(&mFMODSystem);
  VCNFMODAudioCore::FMODASSERT(result);
  if ( result != FMOD_OK )
  {
    return false;
  }

  // Initialize FMOD.
  result = mFMODSystem->init(32, FMOD_INIT_NORMAL, 0);
  if ( result != FMOD_OK )
  {
    mFailedInitOutput = true;
    return true;
  }

  // Default world units
  mFMODSystem->set3DSettings(1.0, 1.0f, 1.0);

  return true;
}

///////////////////////////////////////////////////////////////////////
///
/// Releases FMOD.
///
/// @return true if everything was released successfully.none
///
VCNBool VCNFMODAudioCore::Uninitialize()
{
  DestroyAll();

  if ( mFMODSystem )
  {
    mFMODSystem->release();
  }

  return VCNAudioCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
///
/// Updates FMOD system. We update 3D settings and tick the system.
///
/// @return true if everything went smoothly.
///
VCNBool VCNFMODAudioCore::Process(const float elapsedTime)
{
  if ( mFailedInitOutput )
    return true;

  mFMODSystem->set3DListenerAttributes(0, 
    (const FMOD_VECTOR*)&mViewerPosition, 
    (const FMOD_VECTOR*)&mViewerVelocity, 
    (const FMOD_VECTOR*)&mViewerForwardDirection, 
    (const FMOD_VECTOR*)&mViewerUpDirection);
  mFMODSystem->update();

  return true;
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
const VCNResID VCNFMODAudioCore::Create2DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
                                                      const VCNBool loop, const VCNFloat volume, 
                                                      PlaybackType playback)
{
  if ( mFailedInitOutput )
    return kInvalidResID;

  // Create the new emitter
  VCNFMOD2DSoundEmitter* pEmitter = new VCNFMOD2DSoundEmitter( mFMODSystem );
  
  // Load the sound
  pEmitter->LoadSound( filename, playback );
  pEmitter->SetLoop( loop );
  pEmitter->SetVolume( volume );  

  // Put it in the resource core  
  pEmitter->SetName( sndID );
  VCNResID resID = VCNResourceCore::GetInstance()->AddResource( sndID, pEmitter );
  
  // Keep track of the sound resource.
  mSounds.push_back ( resID );

  return resID;
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
const VCNResID VCNFMODAudioCore::Create3DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
                                                      const Vector3& position, const VCNFloat minDistance, 
                                                      const VCNFloat maxDistance, const VCNBool loop, 
                                                      const VCNFloat volume, PlaybackType playback)
{
  if ( mFailedInitOutput )
    return kInvalidResID;

  // Create the new emitter
  VCNFMOD3DSoundEmitter* pEmitter = new VCNFMOD3DSoundEmitter(mFMODSystem, position);
  
  // Load the sound
  pEmitter->LoadSound( filename, playback );

  pEmitter->SetLoop(loop);
  pEmitter->SetVolume(volume);   
  pEmitter->SetMinMaxDistance(minDistance, maxDistance);
  
  // Put it in the resource core
  pEmitter->SetName( sndID );
  VCNResID resID = VCNResourceCore::GetInstance()->AddResource( sndID, pEmitter );

  // Keep track of the sound resource.
  mSounds.push_back ( resID );

  return resID;
}

///////////////////////////////////////////////////////////////////////
///
/// Stops all sound at once.
/// 
void VCNFMODAudioCore::StopAll()
{
  if ( mFailedInitOutput )
    return;

  std::vector<VCNResID>::iterator iter = mSounds.begin();
  while(iter != mSounds.end())
  {
    VCNResourceCore::GetInstance()->GetResource<VCNSoundEmitter>((*iter))->Stop();
    ++iter;
  }
}

///////////////////////////////////////////////////////////////////////
///
/// Release all sounds.
///
void VCNFMODAudioCore::DestroyAll()
{
  if ( mFailedInitOutput )
    return;

  std::vector<VCNResID>::iterator iter = mSounds.begin();
  while(iter != mSounds.end())
  {
    VCNResourceCore::GetInstance()->DeleteResource((*iter));
    ++iter;
  }
  mSounds.clear();
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
void VCNFMODAudioCore::SetViewerSettings(const Vector3& viewerPosition, const Vector3& velocity, 
                                         const Vector3& forwardDirection, const Vector3& upDirection)
{
  mViewerPosition = viewerPosition;
  mViewerVelocity = velocity;
  mViewerForwardDirection = forwardDirection;
  mViewerUpDirection = upDirection;
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
void VCNFMODAudioCore::Set3DSettings(VCNFloat distanceFactor)
{
  if ( mFMODSystem )
  {
    mFMODSystem->set3DSettings( 1.0f, distanceFactor, 1.0f );
  }
}

///////////////////////////////////////////////////////////////////////
void VCNFMODAudioCore::FMODASSERT(int result)
{
  if (result != FMOD_OK)
  {
    VCN_ASSERT_FAIL( VCNTXT("FMOD error! (%d) %s\n"), result, VCN_A2W(FMOD_ErrorString(static_cast<FMOD_RESULT>(result))) );
  }
}
