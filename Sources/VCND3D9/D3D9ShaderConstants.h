///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_SHADER_CONSTANTS
#define VICUNA_SHADER_CONSTANTS

#include "VCNUtils/Types.h"

// Dummy shader filename (contains constant pool)
extern const VCNTChar* kDummyShaderFile;

//-------------------------------------------------------------
// Shader filenames
extern const VCNTChar* kShaderFiles[];

// Handy for loading 
extern const VCNTChar* kShaderFolder;
extern const VCNTChar* kShaderExtension;

//-------------------------------------------------------------
// Common name of the base technique and pass found in shaders.
extern const char* kShaderBaseTechnique;

//-------------------------------------------------------------
// Names of the variables in the pool
extern const char* kShaderPoolConstantNames[];

#endif
