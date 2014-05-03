///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "D3D9MaterialCore.h"

#include "D3D9Texture.h"
#include "VCND3D9/D3D9.h"
#include "VCNUtils/Constants.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNDXMaterialCore, VCNMaterialCore );

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNDXMaterialCore::VCNDXMaterialCore()
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNDXMaterialCore::~VCNDXMaterialCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNDXMaterialCore::Initialize()
{
  return VCNMaterialCore::Initialize();
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNDXMaterialCore::Uninitialize()
{
  return VCNMaterialCore::Uninitialize();
}

//-------------------------------------------------------------
/// Process every frame
//-------------------------------------------------------------
VCNBool VCNDXMaterialCore::Process(const float elapsedTime)
{
  return true;
}

//-------------------------------------------------------------
/// Create a DirectX texture and add it to the resource core
//-------------------------------------------------------------
VCNResID VCNDXMaterialCore::CreateTexture( const VCNString& filename )
{
  VCNResID texID = kInvalidResID;

  // Make sure we have a name
  if( !filename.empty() )
  {
    // Check if the resource already exists.
    VCNResource* res = VCNResourceCore::GetInstance()->GetResource<VCNResource>( filename );
    if ( res )
    {
      return res->GetResourceID();
    }

    // Get our renderer
    VCND3D9* renderer = (VCND3D9*)VCND3D9::GetInstance();

    // Have directX take care of it for us
    LPDIRECT3DTEXTURE9 dxTexturePtr = NULL;
    HRESULT hr = D3DXCreateTextureFromFile( renderer->GetD3DDevice(), filename.c_str(), &dxTexturePtr );
    if( SUCCEEDED(hr) )
    {
      // Create the new texture and add it to the resource core
      VCND3D9Texture* resTexture = new VCND3D9Texture( dxTexturePtr );
      resTexture->SetName( filename );
      texID = VCNResourceCore::GetInstance()->AddResource( filename, resTexture );
    }
  }

  return texID;
}



///////////////////////////////////////////////////////////////////////
VCNResID VCNDXMaterialCore::CreateTexture(VCNUInt width, VCNUInt height, VCNUInt32 mipLevels, VCNTextureFlags flags)
{
  VCNResID texID = kInvalidResID;

  // Have directX take care of it for us
  LPDIRECT3DTEXTURE9 dxTexture = NULL;
  HRESULT hr = D3DXCreateTexture(
    GetDevice(),
    width, height,                    // w & h
    mipLevels,                        // mip levels
    VCND3D9Texture::ToD3DUsage(flags), // usage
    VCND3D9Texture::ToD3DFormat(flags),// format
    VCND3D9Texture::ToD3DPool(flags),  // memory pool
    &dxTexture);
  VCN_ASSERT_MSG( SUCCEEDED(hr), "Failed to create texture." );

  // Create the new texture and add it to the resource core
  VCND3D9Texture* resTexture = new VCND3D9Texture( dxTexture );
  texID = VCNResourceCore::GetInstance()->AddResource( resTexture );

  return texID;
}



///////////////////////////////////////////////////////////////////////
VCNResID VCNDXMaterialCore::CreateCubeTexture( const VCNString& filename )
{
    VCNResID texID = kInvalidResID;

    // Make sure we have a name
    if( filename.length() )
    {
        // Get our renderer
        VCND3D9* renderer = (VCND3D9*)VCND3D9::GetInstance();

        // Have directX take care of it for us
        LPDIRECT3DCUBETEXTURE9 dxTexturePtr = NULL;
        HRESULT hr = D3DXCreateCubeTextureFromFile( renderer->GetD3DDevice(), filename.c_str(), &dxTexturePtr );
        if( SUCCEEDED(hr) )
        {
            // Create the new texture and add it to the resource core
            VCND3D9Texture* resTexture = new VCND3D9Texture( (LPDIRECT3DTEXTURE9)dxTexturePtr );
            resTexture->SetName( filename );
            texID = VCNResourceCore::GetInstance()->AddResource( filename, resTexture );
        }
    }

    return texID;
}