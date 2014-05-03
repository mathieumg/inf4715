///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implementation of the particle effect
///

#include "Precompiled.h"
#include "ParticleEffect.h"

#include "VCNRenderer/ShaderCore.h"
#include "VCNRenderer/Shader.h"

VCN_TYPE( VCNParticleEffect, VCNEffect );

///////////////////////////////////////////////////////////////////////
VCNParticleEffect::VCNParticleEffect()
  : mShader( VCNShaderCore::GetInstance()->LoadShader( VCNTXT("Particles") ) )
{
  // Load up our effect file with all our shaders inside
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->SetTechnique( "Particles" );

    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION,  ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(0, 12, ELT_USAGE_COLOR,     ELT_TYPE_COLOR));
    declarationList.push_back(VCNDeclarationElement(0, 16, ELT_USAGE_TEXCOORD0, ELT_TYPE_FLOAT3));
    shader->CreateVertexDeclaration( declarationList );

    // Get effect parameter handles
    mProjEffectParam = shader->GetParameterByName( "Proj" );
    mViewEffectParam = shader->GetParameterByName( "View" );
    mTextureEffectParam = shader->GetParameterByName( "partTex" );

    // Ensure effect parameter are valid
    VCN_ASSERT( mProjEffectParam && mViewEffectParam && mTextureEffectParam );
  }
  else
  {
    VCN_ASSERT_FAIL( "Shader failed to load" );
  }
}


///////////////////////////////////////////////////////////////////////
VCNParticleEffect::~VCNParticleEffect()
{
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::Prepare()
{
  
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams)
{
  VCN_ASSERT_FAIL( VCNTXT("DO NOT USE") );
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::Begin(VCNCache* indices, VCNCache* vertexes)
{
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->Bind();

    shader->Begin();
    shader->BeginPass(0);

    shader->SetMatrix( mViewEffectParam, VCNXformCore::GetInstance()->GetViewMatrix() );
    shader->SetMatrix( mProjEffectParam, VCNXformCore::GetInstance()->GetProjectionMatrix() );

    shader->BindIndexCache( indices );
    shader->BindVertexCache( 0, vertexes, 0, sizeof(VCNParticleVertex) );
  }
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::End()
{
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->EndPass();
    shader->End();
  }
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::SetTexture(VCNResID texID)
{
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->SetTexture( mTextureEffectParam, texID );
  }
}


///////////////////////////////////////////////////////////////////////
void VCNParticleEffect::RenderIndexedPrimitive(
  VCNPrimitiveType primitiveType, VCNInt baseVertexIndex, 
  VCNUInt minVertexIndex, VCNUInt numVertices, VCNUInt startIndex, VCNUInt primCount)
{
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->CommitChanges();
    shader->RenderIndexedPrimitive( primitiveType, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount );
  }
}
