///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Skybox node implementation
///

#include "Precompiled.h"
#include "Skybox.h"

// Engine includes
#include "VCNRenderer/EffectCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/StringUtils.h"

///////////////////////////////////////////////////////////////////////
Skybox::Skybox( VCNNodeID nodeID ) 
  : VCNNode(nodeID)
  , mSkyboxSphere(10000.0f, mLocalTranslation)
{
  SetTag( VCNTXT("Skybox") );

  Prepare();
}

///////////////////////////////////////////////////////////////////////
Skybox::~Skybox()
{
  Release();
}

///////////////////////////////////////////////////////////////////////
void Skybox::Process(const float elapsedTime)
{
  if (!mActive)
    return;
  
  SetTranslation( VCNXformCore::GetInstance()->GetViewPosition() + Vector3(0, 2, 0) );

  // Let the base class do its stuff
  VCNNode::Process( elapsedTime );
}

///////////////////////////////////////////////////////////////////////
void Skybox::Render() const
{
  const bool isPostEffect = VCNEffectCore::GetInstance()->IsSceneEffect();
  const bool isShadeSolid = VCNRenderCore::GetInstance()->GetShadeMode() == RS_SHADE_SOLID;
  if ( !mActive || isPostEffect || !isShadeSolid )
    return;

  GPU_PROFILE_BLOCK();

  VCNRenderCore* renderer = VCNRenderCore::GetInstance();

  // Set rendering state to draw the skybox
  renderer->SetTextureMode( RS_TEXTURE_CLAMP );
  renderer->SetDepthBufferMode( RS_DEPTH_NONE );
  renderer->SetBlendMode( RS_BLEND_NONE );
  
  // Tell the transform manager what the new current transform is
  VCNXformCore::GetInstance()->SetWorldTransform( mWorld );

  // TODO OPTIM: Only render face within view 
  for (int i = 0; i < FaceCount; ++i)
  {
    // Select the current material
    VCNMaterialCore::GetInstance()->SelectMaterial( mFaceMaterials[i] );

    // Render the mesh
    VCNEffectCore::GetInstance()->RenderMesh( mFaceMeshes[i], mSkyboxSphere );
  }

  // Restore state
  renderer->SetTextureMode( RS_TEXTURE_WRAP );
  renderer->SetDepthBufferMode( RS_DEPTH_READWRITE );
}

///////////////////////////////////////////////////////////////////////
///
/// 24 vertices = 6 faces (cube) * 4 vertices per face
///
/// Example diagram of "front" quad
/// The numbers are vertices
/// 
/// 2  __________ 4
///   |\         |
///   |  \       |
///   |    \     |
///   |      \   |
/// 1 |        \ | 3
///    ----------
///
void Skybox::Prepare()
{
  // Get managers
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();
  VCNMaterialCore* materialCore = VCNMaterialCore::GetInstance();
  
  // Load skybox textures
  mFaceTextures[Back]   = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\back.jpg") );
  mFaceTextures[Front]  = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\front.jpg") );
  mFaceTextures[Left]   = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\left.jpg") );
  mFaceTextures[Right]  = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\right.jpg") );
  mFaceTextures[Top]    = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\top.jpg") );
  mFaceTextures[Bottom] = materialCore->CreateTexture( VCNTXT("Textures\\Skybox\\bottom.jpg") );

  // Create face materials
  mFaceMaterials[Back] = CreateFaceMaterial( mFaceTextures[Back] );
  mFaceMaterials[Front] = CreateFaceMaterial( mFaceTextures[Front] );
  mFaceMaterials[Left] = CreateFaceMaterial( mFaceTextures[Left] );
  mFaceMaterials[Right] = CreateFaceMaterial( mFaceTextures[Right] );
  mFaceMaterials[Top] = CreateFaceMaterial( mFaceTextures[Top] );
  mFaceMaterials[Bottom] = CreateFaceMaterial( mFaceTextures[Bottom] );

  // Create face vertex buffers
  mVertexBuffers[Back][0].Set(  10.0f, -10.0f, -10.0f );  mTexBuffers[Back][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Back][1].Set(  10.0f,  10.0f, -10.0f );  mTexBuffers[Back][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Back][2].Set( -10.0f, -10.0f, -10.0f );  mTexBuffers[Back][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Back][3].Set( -10.0f,  10.0f, -10.0f );  mTexBuffers[Back][3].Set( 1.0f, 0.0f );

  mVertexBuffers[Front][0].Set( -10.0f, -10.0f,  10.0f );  mTexBuffers[Front][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Front][1].Set( -10.0f,  10.0f,  10.0f );  mTexBuffers[Front][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Front][2].Set(  10.0f, -10.0f,  10.0f );  mTexBuffers[Front][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Front][3].Set(  10.0f,  10.0f,  10.0f );  mTexBuffers[Front][3].Set( 1.0f, 0.0f );

  mVertexBuffers[Left][0].Set( -10.0f, -10.0f, -10.0f );  mTexBuffers[Left][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Left][1].Set( -10.0f,  10.0f, -10.0f );  mTexBuffers[Left][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Left][2].Set( -10.0f, -10.0f,  10.0f );  mTexBuffers[Left][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Left][3].Set( -10.0f,  10.0f,  10.0f );  mTexBuffers[Left][3].Set( 1.0f, 0.0f );

  mVertexBuffers[Right][0].Set( 10.0f, -10.0f,  10.0f );  mTexBuffers[Right][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Right][1].Set( 10.0f,  10.0f,  10.0f );  mTexBuffers[Right][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Right][2].Set( 10.0f, -10.0f, -10.0f );  mTexBuffers[Right][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Right][3].Set( 10.0f,  10.0f, -10.0f );  mTexBuffers[Right][3].Set( 1.0f, 0.0f );

  mVertexBuffers[Top][0].Set( -10.0f,  10.0f,  10.0f );  mTexBuffers[Top][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Top][1].Set( -10.0f,  10.0f, -10.0f );  mTexBuffers[Top][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Top][2].Set(  10.0f,  10.0f,  10.0f );  mTexBuffers[Top][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Top][3].Set(  10.0f,  10.0f, -10.0f );  mTexBuffers[Top][3].Set( 1.0f, 0.0f );

  mVertexBuffers[Bottom][0].Set( -10.0f, -10.0f, -10.0f );  mTexBuffers[Bottom][0].Set( 0.0f, 1.0f ); 
  mVertexBuffers[Bottom][1].Set( -10.0f, -10.0f,  10.0f );  mTexBuffers[Bottom][1].Set( 0.0f, 0.0f );
  mVertexBuffers[Bottom][2].Set(  10.0f, -10.0f, -10.0f );  mTexBuffers[Bottom][2].Set( 1.0f, 1.0f );
  mVertexBuffers[Bottom][3].Set(  10.0f, -10.0f,  10.0f );  mTexBuffers[Bottom][3].Set( 1.0f, 0.0f );
  
  // Create face meshes
  for (int i = 0; i < 6; ++i)
  {
    VCNResID vertexCacheID = renderer->CreateCache( VT_POSITION, mVertexBuffers[i], FaceVertexCount * kCacheStrides[VT_POSITION] );
    VCNResID texCacheID = renderer->CreateCache( VT_DIFFUSE_TEX_COORDS, mTexBuffers[i], FaceVertexCount * kCacheStrides[VT_DIFFUSE_TEX_COORDS] );

    VCNString meshName = StringBuilder( VCNTXT("SkyboxFace") )( i );
  
    VCNMesh* mesh = new VCNMesh();
    mesh->SetName( meshName );
    mesh->SetCacheID( VT_POSITION, vertexCacheID );
    mesh->SetCacheID( VT_DIFFUSE_TEX_COORDS, texCacheID );
    mesh->SetPrimitiveType( PT_TRIANGLESTRIP );
    mesh->SetFaceCount( 2 );

    // We have a new resource ready, add it!
    mFaceMeshes[i] = VCNResourceCore::GetInstance()->AddResource( mesh->GetName(), mesh );
  }

  // Set an initial rotation to the skybox
  VCNQuat initialRotation;
  initialRotation.SetFromEuler(0.0f, 110.0f, 0.0f);
  SetRotation( initialRotation );
}

///////////////////////////////////////////////////////////////////////
void Skybox::Release()
{
}

///////////////////////////////////////////////////////////////////////
const VCNResID Skybox::CreateFaceMaterial(const VCNResID textureID)
{
  // Create a new material that will get managed by the resource manager.
  VCNMaterial* material = new VCNMaterial();

  // Build unique material name
  VCNString matName = StringBuilder( VCNTXT("SkyboxMaterial") )( textureID );
  
  // Set default material property
  material->SetName( matName );
  material->SetDiffuseColor( VCNColor(1.0f,1.0f,1.0f,1.0f) );
  
  VCNEffectParamSet& params = material->GetEffectParamSet();
  params.SetEffectID( eidTextured );
  params.AddResource( VCNTXT("DiffuseTexture"), textureID );

  // Add material as a resource.
  return VCNResourceCore::GetInstance()->AddResource( material->GetName(), material );
  
}
