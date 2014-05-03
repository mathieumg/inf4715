///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "D3D9ShaderCore.h"

#include "D3D9.h"
#include "D3D9Shader.h"
#include "D3D9ShaderConstants.h"

#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Macros.h"

///////////////////////////////////////////////////////////////////////
VCNDXShaderCore::VCNDXShaderCore()
  : mActiveShader()
  , mParamPool(NULL)
  , mDummyShader(NULL)
{
}


///////////////////////////////////////////////////////////////////////
VCNDXShaderCore::~VCNDXShaderCore()
{
}


//-------------------------------------------------------------
/// Helper function to get a hold of the D3D device
//-------------------------------------------------------------
VCNBool VCNDXShaderCore::Initialize()
{
  return VCNShaderCore::Initialize();
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNDXShaderCore::Uninitialize()
{
  return VCNShaderCore::Uninitialize();
}

//-------------------------------------------------------------
/// Tests for shader capabilities
//-------------------------------------------------------------
const VCNBool VCNDXShaderCore::CanSupportShaders() const
{
  bool supported = true;

  // Make sure the device capabilities report that we have what we need
  D3DCAPS9 d3dCaps;
  if( FAILED(GetD3DDevice()->GetDeviceCaps(&d3dCaps)) ) 
  {
    TRACE( L"VCND3D::CreateShaders - ERROR: GetDeviceCaps() in CreateShaders failed" );
    supported = false;
  }
  else if( d3dCaps.VertexShaderVersion < D3DVS_VERSION(2,0) ) 
  {
    TRACE( L"VCND3D::CreateShaders - warning: Vertex Shader Version < 2.0 => no support" );
    supported = false;
  }
  else if( d3dCaps.PixelShaderVersion < D3DPS_VERSION(3,0) )
  {
    TRACE( L"VCND3D::CreateShaders - warning: Pixel Shader Version < 2.0 => no support" );
    supported = false;
  }

  return supported;
}

//-------------------------------------------------------------
/// Load up all the shaders
//-------------------------------------------------------------
VCNBool VCNDXShaderCore::LoadShaders()
{
  // First see if they're supported
  if( !CanSupportShaders() )
    return false;

  // Now grab the handles of the constants in the pool, for convenience
  InitConstantPool();

  return true;
}

//-------------------------------------------------------------
/// Unload all the shaders
//-------------------------------------------------------------
VCNBool VCNDXShaderCore::UnloadShaders()
{
  // Unload all the shaders in there
  mActiveShader.reset();
  mShaderMap.clear();
  
  mDummyShader->Release();
  mDummyShader = NULL;

  // Release the pool
  mParamPool->Release();
  mParamPool = NULL;

  // Reset our handles to the constants
  for( VCNUInt i=0; i<kNumShaderPoolConstants; i++ )
    mShaderPoolConstants[i] = NULL;

  return true;
}


//-------------------------------------------------------------
/// Load the specified shader
//-------------------------------------------------------------
LPD3DXEFFECT VCNDXShaderCore::LoadEffectFile( const VCNTChar* filename, VCNULong flags )
{
#if defined( DEBUG )
  // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  flags |= D3DXSHADER_DEBUG;
  //    flags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
  //    flags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

  VCNString path = kShaderFolder;
  path = path + filename;
  path = path + kShaderExtension;

  LPD3DXBUFFER errorMsg = NULL;
  LPD3DXEFFECT effectPtr = NULL;
  HRESULT hr = D3DXCreateEffectFromFile( GetD3DDevice(), path.data(), NULL, NULL, flags, mParamPool, &effectPtr, &errorMsg );
  if( FAILED(hr) )
  {
    switch( hr )
    {
    case D3DERR_INVALIDCALL:  TRACE( L"VCNDXShaderCore::LoadShader - Warning: Invalid call" ); break;
    case D3DXERR_INVALIDDATA: TRACE( L"VCNDXShaderCore::LoadShader - Warning: Invalid data" ); break;
    case E_OUTOFMEMORY:       TRACE( L"VCNDXShaderCore::LoadShader - Warning: Direct3D is out of memory" ); break;
    default:                  TRACE( L"VCNDXShaderCore::LoadShader - Warning: COULD NOT LOAD SHADER FILE!" ); break;
    }

    // Get a pointer to the error if it exists
    if( errorMsg && errorMsg->GetBufferPointer() )
    {
      char* errorText = (char*)errorMsg->GetBufferPointer();
      VCN_ASSERT_FAIL( VCNTXT("Shader failed to load:\n%s\n"), VCN_A2W(errorText) );
    }
  }

  return effectPtr;
}


///////////////////////////////////////////////////////////////////////
VCNShaderHandle VCNDXShaderCore::LoadShader(const VCNTChar* filename, VCNULong flags /* = 0 */)
{
  LPD3DXEFFECT effect = LoadEffectFile(filename, flags);
  if ( effect )
  {
    mShaderMap[ filename ] = VCNShaderPointer( new VCND3D9Shader(effect) );
    return mShaderMap[ filename ];
  }

  return VCNShaderHandle();
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNDXShaderCore::SelectShader(VCNShaderHandle shaderHandle)
{
  // If it's already active, we're good
  if( mActiveShader.lock() == shaderHandle.lock() )
    return true;

  // Bind the shader and that's it!
  if ( VCNShaderPointer shader = shaderHandle.lock() )
  {
    shader->Bind();

    // Keep track of the active shader
    mActiveShader = shaderHandle;

    // We're done
    return true;
  }

  return false;
}

//-------------------------------------------------------------
/// Pick up the handles on all those constants and create the
/// pool.
//-------------------------------------------------------------
VCNBool VCNDXShaderCore::InitConstantPool()
{
  // Create the pool so that the loaded shaders can link to it
  VCN_ASSERT( mParamPool == NULL );
  if( FAILED(D3DXCreateEffectPool(&mParamPool)) )
    return false;
  
  // Load up the dummy shader (contains the pool)
  VCN_ASSERT( mDummyShader == NULL && "Loading dummy twice!?!" );
  mDummyShader = LoadEffectFile( kDummyShaderFile, 0 );

  // If there a problem with our most basic shader, there's a problem
  if( !mDummyShader )
  {
    VCN_ASSERT( false && "Our most basic shader isn't loaded!  Pool will not function properly!" );
    return false;
  }

  // Get handles on all the constants
  for( VCNUInt i=0; i<kNumShaderPoolConstants; i++ )
  {
    const char* constantName = kShaderPoolConstantNames[i];
    mShaderPoolConstants[i] = mDummyShader->GetParameterByName( NULL, constantName );
  }

  return true;
}


//-------------------------------------------------------------
/// Sets an INT constant in our shader
//-------------------------------------------------------------
void VCNDXShaderCore::SetIntPoolConstant( VCNShaderPoolConstants index, const VCNInt* array, const VCNUInt size )
{
  // Safety check!  Make sure we don't set constants before shaders are loaded
  VCN_ASSERT( mDummyShader );

  // Set it in the pool
  HRESULT hr = mDummyShader->SetIntArray( mShaderPoolConstants[index], array, size );
  VCN_ASSERT( SUCCEEDED(hr) );
}

//-------------------------------------------------------------
/// Sets a FLOAT constant in our shader
//-------------------------------------------------------------
void VCNDXShaderCore::SetFloatPoolConstant( VCNShaderPoolConstants index, const VCNFloat* array, const VCNUInt size )
{
  // Safety check!  Make sure we don't set constants before shaders are loaded
  VCN_ASSERT( mDummyShader );

  // Set it in the pool
  HRESULT hr = mDummyShader->SetFloatArray( mShaderPoolConstants[index], array, size );
  VCN_ASSERT( SUCCEEDED(hr) );
}

void VCNDXShaderCore::SetFloatPoolConstant( VCNShaderPoolConstants index, const VCNFloat f )
{
    // Safety check!  Make sure we don't set constants before shaders are loaded
    VCN_ASSERT( mDummyShader );

    // Set it in the pool
    HRESULT hr = mDummyShader->SetFloat( mShaderPoolConstants[index], f );
    VCN_ASSERT( SUCCEEDED(hr) );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetWorldTransform(const Matrix4& world)
{
  SetFloatPoolConstant( SPC_World, world.m, 16 );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetWorldViewProjMatrix(const Matrix4& mat)
{
  SetFloatPoolConstant( SPC_WorldViewProjection, mat.m, 16 );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetViewMatrix(const Matrix4& mat)
{
  SetFloatPoolConstant( SPC_View, mat.m, 16 );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetViewPosition(const Vector3& postion)
{
  SetFloatPoolConstant( SPC_ViewPosition, postion.vec, 3 );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetModelViewMatrix(const Matrix4& mat)
{
  SetFloatPoolConstant( SPC_ModelView, mat.m, 16 );
}

///////////////////////////////////////////////////////////////////////
void VCNDXShaderCore::SetNormalMatrix(const Matrix4& mat)
{
  SetFloatPoolConstant( SPC_NormalMatrix, mat.m, 16 );
}

void VCNDXShaderCore::SetFadeAmount(const VCNFloat fadeAmount)
{
  SetFloatPoolConstant(SPC_FadeAmount, fadeAmount);
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNDXShaderCore::Process(const float elapsedTime)
{
  // We reset the current shader here so in the next frame, this first effect is restored successfully.
  mActiveShader.reset();
  return true;
}

