#ifndef GAMESCENE_H
#define GAMESCENE_H

///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///  @brief Game Scene
///

#pragma once

#include "VCNCore/Core.h"
#include "VCNImporter/XMLLoader.h"
#include "VCNNodes/NodeCore.h"
#include "VCNUtils/Vector.h"

class VCNModel;

////////////////////////////////////////////////////////////////////////
/// @class GameScene
/// @brief GameScene class to load/edit the game scene.
/// @TODO Rename to GameResourceManager or GameResourceCore or MediaManager
////////////////////////////////////////////////////////////////////////
class MediaManager : public VCNCore<MediaManager>, private VCNXMLLoader
{
  VCN_CLASS;

public:

  MediaManager();
  virtual ~MediaManager();

  /// Load scene elements
  void LoadScene();

  /// Create instances of scene models
  VCNNode* CreateInstance(const VCNString& InstanceBaseModelName);
  
  /// Delete scene model instances
  void DeleteInstance(const VCNNodeID& nodeID);
  
  /// Get the initial transformation of scene model
  const Vector3 GetModelScaleTransform(const VCNString& InstanceBaseModelName) const;
  
  /// Get scene model properties
  const VCNString GetModelProperty(const VCNString& InstanceBaseModelName) const;

  /// Returns the number of scene models
  const int GetModelCount() const;

  /// Returns all model names.
  const std::vector<VCNString> GetModelNames() const;

protected:

// VCNCore interface

  /// Initializes the world core.
  virtual VCNBool Initialize() override;

  /// Releases resources and unregister entities if needed.
  virtual VCNBool Uninitialize() override;

  /// Processes world entities
  virtual VCNBool Process(const float elapsedTime) override;

private:

  void LoadMaterials();
  void LoadModels();
  void LoadMeshes();
  void LoadAnimations();

  /// Create a new XML node from a scene node instance that is not saved
  XMLElementPtr CreateNewNode(XMLDocPtr xmlDoc, VCNNode* instanceToSave);
  
  /// Checks if the XML node already has a Dynamic flash
  const VCNBool HasPropertyNode(const VCNString& propName, XMLNodePtr node, XMLNodePtr& propNode) const;

  VCNString mMaterialsPath;
  VCNString mModelsPath;
  VCNString mMeshesPath;
  VCNString mAnimationsPath;

  std::map<VCNString, VCNModel*> mModelsMap;
  std::map<VCNString, Vector3> mModelsScaleTransformsMap;
  std::map<VCNString, VCNString> mModelsPropertyMap;

};

inline MediaManager* MEDIA()
{
  return MediaManager::GetInstance();
}

#endif // GAMESCENE_H
