///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Lighting core implementation
///

#include "Precompiled.h"
#include "LightingCore.h"

#include "VCNNodes/NodeCore.h"
#include "VCNRenderer/DirectionalLight.h"
#include "VCNRenderer/PointLight.h"
#include "VCNRenderer/SpotLight.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNLightingCore, VCNCore<VCNLightingCore> );

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNLightingCore::VCNLightingCore()
{
  // Init the ambient color
  memset( &mAmbientLightColor, 0, sizeof(mAmbientLightColor) );
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNLightingCore::~VCNLightingCore()
{
  mLights.clear();
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNLightingCore::Initialize()
{
  return BaseCore::Initialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNLightingCore::Uninitialize()
{
  return BaseCore::Uninitialize();
}

//-------------------------------------------------------------
/// Create a light of the given type
//-------------------------------------------------------------
VCNLight* VCNLightingCore::CreateLight( const VCNLightType type, const VCNString& name )
{
  // Our return pointer
  VCNLight* newLight = NULL;

  // Create the light
  switch(type)
  {
  case LT_DIRECTIONAL:
    newLight = VCNNodeCore::GetInstance()->CreateNode<VCNDirectionalLight>();
    break;

  case LT_OMNI:
    newLight = VCNNodeCore::GetInstance()->CreateNode<VCNPointLight>();
    break;

  case LT_SPOT:
    newLight = VCNNodeCore::GetInstance()->CreateNode<VCNSpotLight>();
    break;

  default:
    VCN_ASSERT( false && "UNKNOWN LIGHT TYPE!" );
  }

  // Give it it's name
  newLight->SetTag( name );

  AddLight( newLight->GetNodeID() );

  return newLight;
}


///////////////////////////////////////////////////////////////////////
void VCNLightingCore::AddLight(const VCNNodeID resID)
{
  // Add it to our list of lights
  mLights.push_back( resID );
}


//-------------------------------------------------------------
/// Removes a light form the array (does not destroy it)
//-------------------------------------------------------------
void VCNLightingCore::RemoveLight( const VCNResID resID )
{
  VCNUInt i = 0;
  for( i=0; i<mLights.size(); i++ )
  {
    if( mLights[i] == resID )
      break;
  }

  if( i < mLights.size() )
  {
    // Crush that index
    for( i=i+1; i<mLights.size(); i++ )
      mLights[i-1] = mLights[i];

    // Get rid of the last element
    mLights.pop_back();
  }
}

//-------------------------------------------------------------
/// Returns an array of lights that intersect with a sphere
//-------------------------------------------------------------
void VCNLightingCore::IntersectLights( const VCNSphere& sphere, std::vector<VCNLight*>& lights )
{
  for( VCNUInt i=0; i<mLights.size(); i++ )
  {
    // Get the light, and consider it if it's on
    VCNLight* light = (VCNLight*)VCNNodeCore::GetInstance()->GetNode( mLights[i] );
    if( light && light->IsOn() && light->Intersects( sphere ) )
    {
      lights.push_back( light );
    }
  }
}

//-------------------------------------------------------------
/// Set ambient light color
//-------------------------------------------------------------
void VCNLightingCore::SetAmbientLight( VCNFloat red, VCNFloat green, VCNFloat blue )
{
  mAmbientLightColor.A = 1.0f;
  mAmbientLightColor.R = red;
  mAmbientLightColor.G = green;
  mAmbientLightColor.B = blue;
}

//-------------------------------------------------------------
/// Return the indexed light
//-------------------------------------------------------------
VCNLight* VCNLightingCore::GetLight( const VCNUInt idx ) const
{
  return static_cast<VCNLight*>( VCNNodeCore::GetInstance()->GetNode( mLights[idx] ) );
}
