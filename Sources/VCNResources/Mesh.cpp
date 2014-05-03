///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Mesh.h"

#include "Cache.h"
#include "ResourceCore.h"

VCN_TYPE( VCNMesh, VCNResource ) ;


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNMesh::VCNMesh() 
  : VCNResource()
  , mFaceCacheID(kInvalidResID)
  , mMaterialID(kInvalidResID)
  , mPrimitiveType( PT_TRIANGLELIST )
  , mFaceCount(0)
  , m_BoneInfluenceCount(0)
  , m_BoneTransforms(nullptr)
{
  for( VCNUInt i=0; i<VT_NUM_VERTEX_TYPES; ++i )
  {
    SetCacheID( (VCNCacheType)i, kInvalidResID );
  }
}


//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNMesh::~VCNMesh()
{
}

//-------------------------------------------------------------
/// Returns the number of vertex in this mesh
//-------------------------------------------------------------
VCNUInt VCNMesh::GetVertexCount() const
{
  // We take the count from the position cache (since every mesh needs one)
  if( GetCacheID(VT_POSITION) != kInvalidResID )
  {
    VCNCache* cache = VCNResourceCore::GetInstance()->GetResource<VCNCache>( GetCacheID(VT_POSITION) );
    return cache->GetCount();
  }

  return 0;
}

//-------------------------------------------------------------
/// Returns the number of faces in this mesh
//-------------------------------------------------------------
VCNUInt VCNMesh::GetFaceCount() const
{
  // We take the count from the position cache (since every mesh needs one)
  if( mFaceCacheID != kInvalidResID )
  {
    VCNCache* cache = VCNResourceCore::GetInstance()->GetResource<VCNCache>( mFaceCacheID );
    if ( mPrimitiveType == PT_TRIANGLELIST )
      return cache->GetCount() / 3;
    else if ( mPrimitiveType == PT_TRIANGLESTRIP )
      return cache->GetCount()-2;
    else
    {
      VCN_ASSERT_FAIL( "not supported" );
    }

    return 0;
  }
  
  return mFaceCount != 0 ? mFaceCount : GetVertexCount();
}

const VCNSphere& VCNMesh::GetBoundingSphere() const
{
  return mBoundingSphere;
}

void VCNMesh::SetBoundingSphere( const VCNSphere& sphere )
{
  mBoundingSphere = sphere;
}

const VCNAabb& VCNMesh::GetBoundingBox() const
{
	return mBoundingAabb;
}

void VCNMesh::SetBoundingBox( const VCNAabb& box )
{
	mBoundingAabb = box;
}

VCNResID VCNMesh::GetCacheID( VCNCacheType type ) const
{
  return mCacheIDs[type];
}

//-------------------------------------------------------------
void VCNMesh::SetCacheID( VCNCacheType type, VCNResID cacheID )
{
  mCacheIDs[type] = cacheID;
}

//-------------------------------------------------------------
void VCNMesh::SetFaceCache( VCNResID cacheID )
{
  mFaceCacheID = cacheID;
}

//-------------------------------------------------------------
VCNResID VCNMesh::GetFaceCache() const
{
  return mFaceCacheID;
}

//-------------------------------------------------------------
const VCNBool VCNMesh::IsIndexed() const
{
  return (GetFaceCache() != kInvalidResID);
}

///////////////////////////////////////////////////////////////////////
void VCNMesh::SetMaterialID(const VCNResID materialID)
{
  mMaterialID = materialID;
}

///////////////////////////////////////////////////////////////////////
const VCNResID VCNMesh::GetMaterialID() const
{
  return mMaterialID;
}
