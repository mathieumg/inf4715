///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Static terrain implementation
///

#include "Precompiled.h"
#include "Terrain.h"

#include "MediaManager.h"

#include "VCNNodes/RenderNode.h"
#include "VCNResources/Cache.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"

VCN_TYPE( Terrain, VCNNode );

namespace
{
  float Sign(const Vector3& p1, const Vector3& p2, const Vector3& p3)
  {
    return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
  }

  bool IsPointInTri(const Vector3& pt, const Vector3& v1, const Vector3& v2, const Vector3& v3)
  {
    bool b1, b2, b3;

    b1 = Sign(pt, v1, v2) < 0.0f;
    b2 = Sign(pt, v2, v3) < 0.0f;
    b3 = Sign(pt, v3, v1) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
  }
}


///////////////////////////////////////////////////////////////////////
Terrain::Terrain(VCNNodeID nodeID)
  : VCNNode(nodeID)
  , mTerrainNode( 0 )
{
  SetTag( VCNTXT("Terrain") );
  mTerrainNode = MEDIA()->CreateInstance( VCNTXT("Terrain") )->Cast<VCNRenderNode>();
  AttachChild( mTerrainNode->GetNodeID() );

  GenerateTerrainMap();
}


///////////////////////////////////////////////////////////////////////
Terrain::~Terrain()
{
}


///////////////////////////////////////////////////////////////////////
const float Terrain::GetHeightAt(float x, float y) const
{
  /*
  const int i = static_cast<int>(std::floor(x + 0.5f)) - mixMin;
  const int j = static_cast<int>(std::floor(y + 0.5f)) - miyMin;

  if ( i > 0 && i < mWidth-1 &&
       j > 0 && j < mHeight-1 )
    return mTerrainMap[j][i].height;

  return 0;  
  */
  
  x = std::floor(x + 0.5f) - static_cast<float>( mixMin );
  y = std::floor(y + 0.5f) - static_cast<float>( miyMin );

  Vector2 a(x ,     y );
  Vector2 b(x+1.0f, y );
  Vector2 c(x ,     y+1.0f );
  Vector2 d(x+1.0f, y+1.0f );

  // check the parameters
  if ( ( a.x >= 0 ) && ( a.x < (float)mWidth ) && ( a.y >= 0 ) && ( a.y < (float)mHeight ) &&
       ( b.x >= 0 ) && ( b.x < (float)mWidth ) && ( b.y >= 0 ) && ( b.y < (float)mHeight ) &&
       ( c.x >= 0 ) && ( c.x < (float)mWidth ) && ( c.y >= 0 ) && ( c.y < (float)mHeight ) &&
       ( d.x >= 0 ) && ( d.x < (float)mWidth ) && ( d.y >= 0 ) && ( d.y < (float)mHeight ) ) 
  {

    float heightA = mTerrainMap[static_cast<int>(a.y)][static_cast<int>(a.x)].height;
    float heightB = mTerrainMap[static_cast<int>(b.y)][static_cast<int>(b.x)].height;
    float heightC = mTerrainMap[static_cast<int>(c.y)][static_cast<int>(c.x)].height;
    float heightD = mTerrainMap[static_cast<int>(d.y)][static_cast<int>(d.x)].height;

    float D = (b.x - a.x) * (c.y - a.y);
    return (    (heightA / D * (b.x - x) * (c.y - y) ) +
                (heightB / D * (x - a.x) * (c.y - y) ) +
                (heightC / D * (b.x - x) * (y - a.y) ) +
                (heightD / D * (x - a.x) * (y - a.y) ) );

  }

  return 0;
}

///////////////////////////////////////////////////////////////////////
const Vector3 Terrain::GetNormalAt(float x, float y) const
{
  return Vector3(0, 1, 0);
}


///////////////////////////////////////////////////////////////////////
void Terrain::GenerateTerrainMap()
{
  VCN_ASSERT( mTerrainNode );

  // Get some managers
  VCNResourceCore* resMgr = VCNResourceCore::GetInstance();

  // Get the mesh ID resource
  const VCNResID meshID = mTerrainNode->GetMeshID();

  // Get terrain geometry info
  VCNMesh* mesh = resMgr->GetResource<VCNMesh>( meshID );
  
  const VCNResID faceCacheID = mesh->GetFaceCache();
  const VCNResID vetexCacheID = mesh->GetCacheID( VT_POSITION );

  VCNCache* faceCache = resMgr->GetResource<VCNCache>( faceCacheID );
  VCNCache* vertexCache = resMgr->GetResource<VCNCache>( vetexCacheID );

  // Get the vertex buffer
  std::vector<Vector3> vertexBuffer;
  if ( !vertexCache->GetBuffer( vertexBuffer ) )
  {
    VCN_ASSERT_FAIL( "Failed to get terrain vertex buffer" );
  }

  // Get the index buffer
  struct Face { unsigned short v1, v2, v3; };
  std::vector<Face> faceBuffer;
  if ( !faceCache->GetBuffer( faceBuffer ) )
  {
    VCN_ASSERT_FAIL( "Failed to get terrain vertex buffer" );
  }

  // Get min and max width and height
  VCNFloat xMin = std::numeric_limits<VCNFloat>::max(), xMax = -std::numeric_limits<VCNFloat>::max();
  VCNFloat yMin = std::numeric_limits<VCNFloat>::max(), yMax = -std::numeric_limits<VCNFloat>::max();

  for (size_t i = 0, end = vertexBuffer.size(); i < end; ++i)
  {
    const Vector3& v = vertexBuffer[i];
    xMin = std::min( xMin, v.x );
    xMax = std::max( xMax, v.x );

    yMin = std::min( yMin, v.z );
    yMax = std::max( yMax, v.z );
  }

  // Determine discrete bounds
  const float kBias = 0.0001f;
  mixMin = static_cast<int>( std::floor(xMin + kBias) );
  mixMax = static_cast<int>( std::ceil(xMax - kBias) );
  miyMin = static_cast<int>( std::floor(yMin + kBias) );
  miyMax = static_cast<int>( std::ceil(yMax - kBias) );

  // Determine the sizes
  mWidth = std::abs( mixMax - mixMin ) + 1;
  mHeight = std::abs( miyMax - miyMin ) + 1;

  // Allocate the map
  TerrainRow defaultRow( mWidth );
  mTerrainMap.resize( mHeight, defaultRow );

  // For each faces fill in the includes terrain points
  xMin = std::numeric_limits<VCNFloat>::max();
  xMax = -std::numeric_limits<VCNFloat>::max();
  yMin = std::numeric_limits<VCNFloat>::max();
  yMax = -std::numeric_limits<VCNFloat>::max();
  for (int f = 0, fend = faceBuffer.size(); f < fend; ++f)
  {
    const Vector3& v1 = vertexBuffer[faceBuffer[f].v1];
    const Vector3& v2 = vertexBuffer[faceBuffer[f].v2];
    const Vector3& v3 = vertexBuffer[faceBuffer[f].v3];

    // Get rectangular bounds to limit our filling
    xMin = std::min( v1.x, std::min( v2.x, v3.x ) );
    xMax = std::max( v1.x, std::max( v2.x, v3.x ) );
    yMin = std::min( v1.z, std::min( v2.z, v3.z ) );
    yMax = std::max( v1.z, std::max( v2.z, v3.z ) );

    int ixMin = static_cast<int>( std::floor(xMin + kBias) );
    int ixMax = static_cast<int>( std::ceil(xMax - kBias) );
    int iyMin = static_cast<int>( std::floor(yMin + kBias) );
    int iyMax = static_cast<int>( std::ceil(yMax - kBias) );

    for (int j = iyMin; j < iyMax; ++j)
    {
      for (int i = ixMin; i < ixMax; ++i)
      {
        TerrainPoint& tPt = mTerrainMap[j - miyMin][i - mixMin];

        // Check if point in triangle
        Vector3 c((float)i, 0.0f, (float)j);
        if ( IsPointInTri( c, v1, v2, v3 ) )
        {
          //c.y = (c.x*v1.y*v2.z+(v1.x*v2.y-v2.x*v1.y)*c.z-c.x*v2.y*v1.z)/(v1.x*v2.z-v2.x*v1.z);

          const Vector3 n = CrossProduct(v2 - v1, v3 - v1);
          c.y = (n.x*(v1.x-c.x) + n.z*(v1.z-c.z) + n.y*v1.y) / n.y;

          tPt.height += c.y;
          tPt.sourceCount++;
        }
      }
    }
  }

  // Finalize terrain map
  // Compute normals and fill empty cells
  bool normalized = false;
  while ( !normalized )
  {
    normalized = true;
    for (size_t j = 0; j < mTerrainMap.size(); ++j)
    {
      for (size_t i = 0; i < defaultRow.size(); ++i)
      {
        TerrainPoint& tPt = mTerrainMap[j][i];
        if ( tPt.sourceCount == 0 )
        {
          float c = 0;
          float h = 0;

          if ( i > 0 && mTerrainMap[j][i-1].sourceCount > 0 )
          {
            c++;
            h += mTerrainMap[j][i-1].height;
          }

          if ( j > 0 && mTerrainMap[j-1][i].sourceCount > 0 )
          {
            c++;
            h += mTerrainMap[j-1][i].height;
          }

          if ( i < defaultRow.size()-1 && mTerrainMap[j][i+1].sourceCount > 0  )
          {
            c++;
            h += mTerrainMap[j][i+1].height;
          }

          if ( j < mTerrainMap.size() - 1 && mTerrainMap[j+1][i].sourceCount > 0  )
          {
            c++;
            h += mTerrainMap[j+1][i].height;
          }

          if ( c > 0.0f )
          {
            tPt.height = h / c;
            tPt.sourceCount++;
          }
          else
          {
            normalized = false;
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
const VCNResID Terrain::GetMeshID() const
{
  VCN_ASSERT( mTerrainNode );
  return mTerrainNode->GetMeshID();
}

///////////////////////////////////////////////////////////////////////
const Vector3 Terrain::GetPositionAt(float x, float y, float heightOffset /*= 0.0f*/) const
{
  Vector3 pos(x, heightOffset, y);
  pos.y += GetHeightAt(x, y);
  return pos;
}
