///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Sounds Manager
///

#ifndef SOUNDS_MANAGER_H
#define SOUNDS_MANAGER_H

#include "VCNAudio/SoundEmitter2D.h"
#include "VCNAudio/SoundEmitter3D.h"
#include "VCNImporter/XMLLoader.h"
#include "VCNUtils/Singleton.h"

class VCNNode;

///
/// Class used to load all sound assets found in Sounds.xml
///
class SoundManager : public Singleton<SoundManager>, private VCNXMLLoader
{
  SINGLETON_DECLARATION( SoundManager );

public:
  SoundManager();
  virtual ~SoundManager();

  typedef VCNString SoundType;
  typedef VCNString SoundID;
  
  void PlayAllAmbientTracks();
  void StopAllAmbientTracks();
  void PlayAllMenuTracks();
  void StopAllMenuTracks();
  
  void PlaySound(const VCNString& soundID);
  void PauseSound(const VCNString& soundID);
  void StopSound(const VCNString& soundID);  

  void LoadSounds();

  static void PlayCollisionSound(
    void* userData, 
    const VCNNode* node, 
    const Vector3& at, 
    const Vector3& force,
    const Vector3& friction,
    const Vector3& velocity);

  void Update(const float elapsedTime);

  void RequestCollisionSoundStabilization();

private:

  typedef VCNString                             ObjectName;
  typedef std::vector<VCNResID>                 SoundList;
  typedef std::map<SoundID, VCNResID>           SoundResourceMap;
  typedef std::map<SoundType, SoundResourceMap> SoundTypeResourceMap;
  typedef std::map<ObjectName, SoundList>       ContactSoundMap;

  VCNBool               mSoundEnabled;
  VCNBool               mMenuTracksPlaying;
  SoundTypeResourceMap  mTracksRessourceIDs;
  ContactSoundMap       mContactSounds;
  VCNBool               mIgnoreCollision;
  VCNFloat              mCollisionSoundElapsedTime;
  Vector3               mLastPos;
  VCNFloat              mInactiveDelayAtStart;
};

#endif // SOUNDS_MANAGER_H
