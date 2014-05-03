///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Sounds Manager
///

#include "Precompiled.h"
#include "SoundManager.h"

#include "Camera.h"
#include "Config.h"
#include "PlayState.h"
#include "SoundManagerConstants.h"
#include "StateMachine.h"

// Engine includes
#include "VCNAudio/AudioCore.h"
#include "VCNPhysic/PhysicCore.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Types.h"

//////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager()
: mSoundEnabled( CST_BOOL("Sound.Enabled") )
, mMenuTracksPlaying(false)
, mIgnoreCollision(true)
, mCollisionSoundElapsedTime(0)
, mInactiveDelayAtStart(1.0f)
{
}

//////////////////////////////////////////////////////////////////////////
SoundManager::~SoundManager()
{
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::LoadSounds()
{
  XMLNodePtr soundsRootNode = 0;    
  XMLNodePtr newSoundNode = 0;
  XMLNodePtr newSoundNodeProperty = 0;
  XMLNodeListPtr soundsRootNodeChildren = 0;  
  
  VCNString soundID;
  VCNString soundFileName;
  VCNFloat  soundVolume;
  VCNBool   soundLoopProperty;
  VCNString soundDimension;
  VCNString soundType;
  VCNFloat  soundPositionX;
  VCNFloat  soundPositionY;
  VCNFloat  soundPositionZ;
  VCNFloat  soundMinDistance;
  VCNFloat  soundMaxDistance;
  Vector3   soundPosition;
  VCNLong   numberOfSounds  = 0;

  // Open the XML document and loads the root
  XMLElementPtr soundsFileRoot = LoadDocumentRoot( kSoundsFileName );

  VCN_ASSERT_MSG( soundsFileRoot != NULL, VCNTXT("Could not load sounds!") );

  // Select the "Instances" root node and its children
  soundsFileRoot->selectSingleNode( (VCNTChar*)kSoundsRootNodeName, &soundsRootNode );  

  // Select all the root's children and calculates the amount of children
  soundsRootNode->get_childNodes( &soundsRootNodeChildren );  
  soundsRootNodeChildren->get_length( &numberOfSounds );

  // Iterate through all the sounds that have to be loaded
  for( VCNLong soundIndex = 0; soundIndex < numberOfSounds; soundIndex++ )
  {
    // Select a new sound to load
    soundsRootNodeChildren->get_item( soundIndex, &newSoundNode );

    GetAttributeString(newSoundNode, kSoundIDPropertyName,        soundID);
    GetAttributeString(newSoundNode, kSoundFilenamePropertyName,  soundFileName);
    GetAttributeFloat(newSoundNode,  kSoundVolumePropertyName,    soundVolume);
    GetAttributeBool(newSoundNode,   kSoundLoopPropertyName,      soundLoopProperty);
    GetAttributeString(newSoundNode, kSoundDimensionPropertyName, soundDimension);
    GetAttributeString(newSoundNode, kSoundTypePropertyName,      soundType);

    if(soundDimension == VCNTXT("3D"))
    {
      GetAttributeFloat(newSoundNode, k3DSoundPosXPropertyName, soundPositionX);
      GetAttributeFloat(newSoundNode, k3DSoundPosYPropertyName, soundPositionY);
      GetAttributeFloat(newSoundNode, k3DSoundPosZPropertyName, soundPositionZ);
      GetAttributeFloat(newSoundNode, k3DSoundMinDistancePropertyName, soundMinDistance);
      GetAttributeFloat(newSoundNode, k3DSoundMaxDistancePropertyName, soundMaxDistance);

      soundPosition.x = soundPositionX;
      soundPosition.y = soundPositionY;
      soundPosition.z = soundPositionZ;
    }  

    // Store the resource ID for this particular track in a map
    // The index is the ID of the sound (see XML file)    
    VCNResID soundResID = kInvalidResID;
    PlaybackType playbackSoundType = Playback_Stream;

    if ( soundType == kContactSoundPropertyName )
    {
      playbackSoundType = Playback_Sound;
    }

    if(soundDimension == VCNTXT("2D"))
    {
      soundResID = VCNAudioCore::GetInstance()->Create2DSoundEmitter(
        soundID, 
        soundFileName, 
        soundLoopProperty, 
        soundVolume / 100.0f,
        playbackSoundType);
    }
    else if(soundDimension == VCNTXT("3D"))
    {
      soundResID = VCNAudioCore::GetInstance()->Create3DSoundEmitter(
        soundID, 
        soundFileName, 
        soundPosition,
        soundMinDistance, 
        soundMaxDistance, 
        soundLoopProperty, 
        soundVolume / 100.0f,
        playbackSoundType);
    }

    mTracksRessourceIDs[soundType][soundID] = soundResID;

    // Check if we are handling a contact sound so we add it to to the contact map for quick access
    if ( soundType == kContactSoundPropertyName )
    {
      long objectCount = 0;
      XMLNodeListPtr objectNodeList = 0;  
      newSoundNode->selectNodes( _bstr_t(kContactObjectTagName.c_str()), &objectNodeList);
      objectNodeList->get_length( &objectCount );

      for (long objectIdx = 0; objectIdx < objectCount; ++objectIdx)
      {
        VCNString objectName;
        XMLNodePtr objectNode = 0;
        objectNodeList->get_item( objectIdx, &objectNode );
        GetAttributeString(objectNode, kContactObjectPropertyName, objectName);

        mContactSounds[objectName].push_back( soundResID );
      }
    }
  }

  ReleaseDocument();
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PlayAllAmbientTracks()
{
  if ( !mSoundEnabled )
    return;

  SoundResourceMap::const_iterator ambientTracksIterator;
  const SoundResourceMap& ambientTracksRessourceIDs = mTracksRessourceIDs[kAmbientSoundPropertyName];

  for (ambientTracksIterator  = ambientTracksRessourceIDs.begin();
       ambientTracksIterator != ambientTracksRessourceIDs.end(); 
       ++ambientTracksIterator)
  {
    VCNSoundEmitter* ambientTrack = VCNResourceCore::GetInstance()->GetResource<VCNSoundEmitter>(ambientTracksIterator->second);
    if ( ambientTrack && !ambientTrack->IsPlaying() )
    {
      ambientTrack->Play();
    }
  }  
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::StopAllAmbientTracks()
{
  std::map<VCNString, VCNResID>::iterator ambientTracksIterator;
  std::map<VCNString, VCNResID> ambientTracksRessourceIDs = (mTracksRessourceIDs[kAmbientSoundPropertyName]);

  for (ambientTracksIterator  = ambientTracksRessourceIDs.begin();
    ambientTracksIterator != ambientTracksRessourceIDs.end(); 
    ++ambientTracksIterator)
  {
    VCNSoundEmitter* ambientTrack = VCNResourceCore::GetInstance()->GetResource<VCNSoundEmitter>(ambientTracksIterator->first);
    ambientTrack->Stop();
  }  
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PlayAllMenuTracks()
{
  if ( !mSoundEnabled )
    return;

  //Start menu tracks only if not already playing
  if (!mMenuTracksPlaying)
  {
    std::map<VCNString, VCNResID>::iterator menuTracksIterator;
    std::map<VCNString, VCNResID> menuTracksRessourceIDs = (mTracksRessourceIDs[kMenu2DSoundPropertyName]);

    for (menuTracksIterator = menuTracksRessourceIDs.begin(); menuTracksIterator != menuTracksRessourceIDs.end(); ++menuTracksIterator)
    {
      VCN2DSoundEmitter* menuTrack = VCNResourceCore::GetInstance()->GetResource<VCN2DSoundEmitter>(menuTracksIterator->first);
      menuTrack->Play();        
    }  

    mMenuTracksPlaying = true;
  }
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::StopAllMenuTracks()
{
  std::map<VCNString, VCNResID>::iterator menuTracksIterator;
  std::map<VCNString, VCNResID> menuTracksRessourceIDs = (mTracksRessourceIDs[kMenu2DSoundPropertyName]);

  for (menuTracksIterator = menuTracksRessourceIDs.begin(); menuTracksIterator != menuTracksRessourceIDs.end(); ++menuTracksIterator)
  {
    VCN2DSoundEmitter* menuTrack = VCNResourceCore::GetInstance()->GetResource<VCN2DSoundEmitter>(menuTracksIterator->first);
    menuTrack->Stop();
  }  
  mMenuTracksPlaying = false;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PlaySound(const VCNString& soundID)
{
  if ( !mSoundEnabled )
    return;

  VCN2DSoundEmitter* soundEmitter = VCNResourceCore::GetInstance()->GetResource<VCN2DSoundEmitter>(soundID);

  if (soundEmitter != NULL)
  {
    // Resume or play sound.
    soundEmitter->Play();
  }
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PauseSound(const VCNString& soundID)
{
  if ( !mSoundEnabled )
    return;

  VCN2DSoundEmitter* soundEmitter = VCNResourceCore::GetInstance()->GetResource<VCN2DSoundEmitter>(soundID);

  if (soundEmitter != NULL)
  {
    soundEmitter->Pause();
  }
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::StopSound(const VCNString& soundID)
{
  VCN2DSoundEmitter* soundEmitter = VCNResourceCore::GetInstance()->GetResource<VCN2DSoundEmitter>(soundID);
  
  if (soundEmitter != NULL)
  {
    if(soundEmitter->IsPlaying())
    {
      soundEmitter->Stop();
    }  
  }
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PlayCollisionSound(void* userData, 
                                      const VCNNode* node, 
                                      const Vector3& at, 
                                      const Vector3& force,
                                      const Vector3& friction,
                                      const Vector3& velocity)
{
  const VCNFloat kCollisionSoundTimeThreshold = 0.15f;
  SoundManager* soundMgr = static_cast<SoundManager*>( userData );
  VCN_ASSERT( soundMgr );

  if ( !soundMgr->mSoundEnabled )
    return;

  // Ignore collision sounds at launch (waiting for objects to stabilize)
  if (soundMgr->mIgnoreCollision)
    return;

  SoundList& soundList = soundMgr->mContactSounds[node->GetTag()];
  for (size_t i = 0; i < soundList.size(); ++i)
  {
    VCN3DSoundEmitter* sound = VCNResourceCore::GetInstance()->GetResource<VCN3DSoundEmitter>(soundList[i]);
    VCN_ASSERT ( sound );
    const float strength = force.Length();
    const float vel = velocity.Length(); 
    if (strength > 2000.0f && vel > 10.0f)
    {
      const float volume = vel / (strength / 60.0f);
      if (volume > 0.05f)
      {
        // Don't play more than one sound every 100 ms (prevent noise)
        if (soundMgr->mCollisionSoundElapsedTime >= 0)
          return;
        soundMgr->mCollisionSoundElapsedTime = kCollisionSoundTimeThreshold;

        sound->SetPosition(at);
        sound->SetVelocity(velocity);
        sound->SetVolume(volume);
        sound->Update();
        sound->Play();
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::Update(const float elapsedTime)
{
  // Wait for sound collision to stabilize to prevent noise
  if (mIgnoreCollision)
  {
    mInactiveDelayAtStart -= elapsedTime;
    if ( mInactiveDelayAtStart > 0.0f )
    {
      mIgnoreCollision = false;
    }
  }

  // TODO: Could the dependence to PlayState be remove?
  const Camera* playerCamera = &StateMachine::GetInstance().GetState<PlayState>(GS_PLAY)->GetCamera();
  const Vector3& playerPosition = playerCamera->GetViewerPosition();
  const Vector3 forwardDirection = playerCamera->GetDirection();
  const Vector3& upDirection = playerCamera->GetUpDirection();
  const Vector3 velocity = (playerPosition - mLastPos) * elapsedTime;

  VCNAudioCore::GetInstance()->SetViewerSettings( playerPosition, velocity, forwardDirection, upDirection );
  
  mLastPos = playerPosition;
  mCollisionSoundElapsedTime -= elapsedTime;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::RequestCollisionSoundStabilization()
{
  mIgnoreCollision = true;
  mInactiveDelayAtStart = 1.0f;
}
