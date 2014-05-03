///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "EffectCore.h"

#include "VCNRenderer/Effect.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNResources/EffectParamSet.h"
#include "VCNResources/Material.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNEffectCore, VCNCore<VCNEffectCore> ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNEffectCore::VCNEffectCore()
: mGeneratingShadowMap(false)
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNEffectCore::~VCNEffectCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNEffectCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  mEffectMap.resize( kNumEffects );

  // Create the effects and fill the array, use that result as a basis for success
  if ( !CreateEffects() )
    return false;

  // Cache some effect that are called regularly.
  mShadowMapEffect = mEffectMap[ eidShadowMap ];

  return true;
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNEffectCore::Uninitialize()
{
  for (EffectList::iterator itr = mEffectMap.begin(), end = mEffectMap.end(); itr != end; ++itr)
  {
    delete *itr;
  }
  mEffectMap.clear();

  return BaseCore::Uninitialize();
}

//-------------------------------------------------------------
/// Render a mesh
//-------------------------------------------------------------
void VCNEffectCore::RenderMesh(VCNMesh* mesh, const VCNSphere& boundingSphere)
{ 
  // Make sure the mesh is valid
  VCN_ASSERT( mesh && mesh->GetCacheID(VT_POSITION) != kInvalidResID );
  
  // Check if we must use a overall scene effect
  if ( mGeneratingShadowMap )
  { 
    mShadowMapEffect->RenderMesh( mesh, boundingSphere, mDummyEffectParam );
  }
  else
  {
    // Get the current material form the material core
    VCNMaterial* material = VCNMaterialCore::GetInstance()->GetCurrentMaterial();

    // If we have a material and an effect
    if( material && material->GetEffectParamSet().IsValid() )
    {
      const VCNEffectParamSet& paramSet = material->GetEffectParamSet();

      // Find the effect by that name
      auto effect = paramSet.GetEffectID();
      VCNEffect* materialEffect = mEffectMap[ effect ];
      materialEffect->RenderMesh( mesh, boundingSphere, paramSet );
    }
  }
}

//////////////////////////////////////////////////////////////////////////
void VCNEffectCore::RenderMesh(VCNResID meshID, const VCNSphere& boundingSphere)
{
  VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>( meshID );
  RenderMesh( mesh, boundingSphere );
}

//-------------------------------------------------------------
// Function to add an effect to the map
//-------------------------------------------------------------
void VCNEffectCore::AddEffect( VCNEffectID effectID, VCNEffect* effectPtr )
{
  mEffectMap[effectID] = effectPtr;
}

//////////////////////////////////////////////////////////////////////////
void VCNEffectCore::PrepareFrame()
{
  for (EffectList::iterator itr = mEffectMap.begin(), end = mEffectMap.end(); itr != end; ++itr)
  {
    (*itr)->Prepare();
  }
}
