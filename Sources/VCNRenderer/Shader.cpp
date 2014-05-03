///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Base shader class
///

#include "Precompiled.h"
#include "Shader.h"

VCN_TYPE( VCNShader, VCNAtom );

///////////////////////////////////////////////////////////////////////
VCNShader::VCNShader()
{
}

///////////////////////////////////////////////////////////////////////
VCNShader::~VCNShader()
{
}

///////////////////////////////////////////////////////////////////////
void VCNShader::CommitChanges()
{
  throw std::exception("The method or operation is not implemented.");
}
