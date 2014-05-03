///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "ShaderCore.h"

VCN_TYPE( VCNShaderCore, VCNCore<VCNShaderCore> ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNShaderCore::VCNShaderCore()
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNShaderCore::~VCNShaderCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNShaderCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  // Load up all the shaders
  LoadShaders();

  // Listen to system transformations
  patterns::Attach( VCNXformCore::GetInstance(), this );

  // We're initialized
  return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNShaderCore::Uninitialize()
{
  UnloadShaders();
  
  return BaseCore::Uninitialize();
}


///////////////////////////////////////////////////////////////////////
void VCNShaderCore::UpdateObserver(VCNMsgTransformChanged& hint)
{
  switch ( hint.type )
  {
  // Set the world matrix
  case VCNMsgTransformChanged::TC_MODEL:
    SetWorldTransform( hint.mat4 );
    break;

  // Set the normal matrix
  case VCNMsgTransformChanged::TC_NORMAL_MATRIX:
    SetNormalMatrix( hint.mat4 );
    break;

  // Set the view matrix
  case VCNMsgTransformChanged::TC_VIEW:
    SetViewMatrix( hint.mat4 );
    break;

  // Set the project matrix
  case VCNMsgTransformChanged::TC_PROJECTION:
    /* not used */
    break;

  // Set the view-model matrix
  case VCNMsgTransformChanged::TC_MODEL_VIEW:
    SetModelViewMatrix( hint.mat4 );
    break;

  // Set the MVP matrix
  case VCNMsgTransformChanged::TC_MODEL_VIEW_PROJECTION:
    SetWorldViewProjMatrix( hint.mat4 );
    break;

  // Set the viewer's position
  case VCNMsgTransformChanged::TC_VIEW_POSITION:
    SetViewPosition( hint.vec3 );
    break;
  }
}
