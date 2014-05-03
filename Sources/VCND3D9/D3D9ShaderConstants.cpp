///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Constants only used while loading shaders
///

#include "Precompiled.h"
#include "D3D9ShaderConstants.h"

#include "VCNUtils/RenderTypes.h"

// Dummy shader filename (contains constant pool)
const VCNTChar* kDummyShaderFile = VCNTXT("DummyShader");


//-------------------------------------------------------------
// Shader filenames
// TO BE REMOVED
const VCNTChar* kShaderFiles[] =
{
  VCNTXT("SSAONormalDepthMap"),
  VCNTXT("SSAO")
};

// Handy for loading 
const VCNTChar* kShaderFolder = VCNTXT("Shaders/");
const VCNTChar* kShaderExtension = VCNTXT(".fx");


//-------------------------------------------------------------
// Common name of the base technique and pass found in shaders.
const char* kShaderBaseTechnique = ("BaseTechnique");


//-------------------------------------------------------------
// Names of the variables in the pool
const char* kShaderPoolConstantNames[kNumShaderPoolConstants] =
{
  ("gWorld"),
  ("gView"),
  ("gModelView"),
  ("gWorldViewProjection"),
  ("gNormalMatrix"),
  ("gViewPosition"),
  ("gFadeAmount"),
};
