///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Effect Core
/// Core module for effect management.
///

#ifndef VCNEFFECTCORE_H
#define VCNEFFECTCORE_H

#pragma once

#include "VCNCore/Core.h"
#include "VCNResources/EffectParamSet.h"
#include "VCNUtils/Matrix.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/Types.h"

class VCNEffect;
class VCNMesh;
class VCNSphere;

class VCNEffectCore : public VCNCore<VCNEffectCore>
{
  VCN_CLASS;

public:
  VCNEffectCore(void);
  virtual ~VCNEffectCore(void);

	typedef const std::vector<Matrix4>* TransformListPtr;

  // Core module overloads
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;

  // Check if we are currently generating a shadow map
  virtual const VCNBool IsGeneratingShadowMap() const { return mGeneratingShadowMap; }

  // Check if we are rendering pre or post effect
  virtual const bool IsSceneEffect() const { return mGeneratingShadowMap; }

  // Function where we create the effects (API specific)
  virtual VCNBool CreateEffects() = 0;

  /// Renders some debugging information such as pre-post processing maps.
  virtual const VCNBool DebugRender() const = 0;

  // Prepare effects for the next frame
  virtual void PrepareFrame();

  // Function to add an effect to the map
  virtual void AddEffect(VCNEffectID effectID, VCNEffect* effectPtr);

  // Render a mesh
  virtual void RenderMesh(VCNResID meshID, const VCNSphere& boundingSphere);

  // Render a mesh
  virtual void RenderMesh(VCNMesh* mesh, const VCNSphere& boundingSphere);

  // Render a shadow map of the scene
  virtual void GenerateShadowMap() = 0;

protected:

  // Just make things easier
  typedef std::vector<VCNEffect*> EffectList;
  
  // Map that holds all the effects
  EffectList        mEffectMap;

  // Indicates that we are generating a shadow map of the scene
  // This state is used by the effect core to force a given effect 
  // instead of using the effect set by the object material
  bool              mGeneratingShadowMap;

  // Cache pre-process effects
  VCNEffect*        mShadowMapEffect;

  // Variable passed when effect parameters aren't really needed.
  VCNEffectParamSet mDummyEffectParam;
};

#endif // VCNEFFECTCORE_H
