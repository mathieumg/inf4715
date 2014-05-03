///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Terrain node implementation.
///

#include "Precompiled.h"
#include "ProceduralTerrain.h"

// Project includes
#include "Config.h"

// Engine includes
#include "VCNRenderer/EffectCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Macros.h"
#include "VCNUtils/Ray.h"
#include "VCNUtils/Utilities.h"

namespace
{
  // Intersect a ray with a 3D triangle.
  const bool IntersectRayTriangle(const VCNRay& R, const Vector3& V0, const Vector3& V1, const Vector3& V2, Vector3& I)
  {
    Vector3 e1,e2,h,s,q;
    float a,f,u,v;

    e1 = V1 - V0;
    e2 = V2 - V0;
    h = CrossProduct(R.m_vcDir, e2);
    a = DotProduct(e1,h);

    if (a > -VCN::EPSILON_VCN && a < VCN::EPSILON_VCN)
      return false;

    f = 1/a;
    s = R.m_vcOrig - V0;
    u = f * (DotProduct(s,h));

    if (u < 0.0 || u > 1.0)
      return false;

    q = CrossProduct(s,e1);
    v = f * DotProduct(R.m_vcDir,q);
    if (v < 0.0 || u + v > 1.0)
      return false;
    // At this stage we can compute t to find out where 
    // the intersection point is on the line
    float t = f * DotProduct(e2,q);
    if (t > VCN::EPSILON_VCN) 
    {
      // Ray intersection!
      I = R.m_vcOrig + R.m_vcDir * t;
      return true;
    }
    else 
    {
      // This means that there is a line intersection  
      // but not a ray intersection
      return false;
    }
  }

}

VCNString ProceduralTerrain::kDefaultMaterialName = VCNTXT("ProceduralTerrainMaterial");

////////////////////////////////////////////////////////////////////////
///
///  Construct a default terrain.
///
///  This function doesn't return a value
///
////////////////////////////////////////////////////////////////////////
ProceduralTerrain::ProceduralTerrain(
  VCNNodeID nodeID, 
  short  size, 
  float hillMin, 
  float hillMax, 
  short numHills, 
  short flattening, 
  int seed)
: VCNNode( nodeID )
, mVertexCache( kInvalidResID )
, mLightingCache( kInvalidResID )
, mTextureCache( kInvalidResID )
, mIndexCache( kInvalidResID )
, mGrassTexture( kInvalidResID )
, mSandTexture( kInvalidResID )
, mSnowTexture( kInvalidResID )
, mGrassRockTexture( kInvalidResID )
, mSnowRockTexture( kInvalidResID )
, mMaterial( kInvalidResID )
, mMesh( kInvalidResID )
, mVertexBuffer( 0 )
, mLightingBuffer( 0 )
, mTexBuffer( 0 )
, mIndexBuffer( 0 )
, mSize( size )
, mHillMin( hillMin )
, mHillMax( hillMax )
, mNumHills( numHills )
, mFlattening( flattening )
, mSeed( seed )
, mCellMap(NULL)
, mScale(1.0f)
{
  // check the parameters
  VCN_ASSERT( size > 0 );
  VCN_ASSERT( hillMin >= 0.0 );
  VCN_ASSERT( hillMax >= 0.0 );
  VCN_ASSERT( flattening >= 0 );

  SetTag( VCNTXT("ProceduralTerrain") );

  Generate(); 
  Compile();
  Prepare();
}



////////////////////////////////////////////////////////////////////////
///
///  Release terrain resources
///
///  This function doesn't return a value
///
////////////////////////////////////////////////////////////////////////
ProceduralTerrain::~ProceduralTerrain()
{
  Release();

  delete [] mVertexBuffer;
  delete [] mLightingBuffer;
  delete [] mTexBuffer;
  delete [] mIndexBuffer;

  // free up the cell buffer.
  delete [] mCellMap;
}



////////////////////////////////////////////////////////////////////////
///
///  Prepare terrain rendering resources
///
///  @param [in]       renderer : renderer
///
///  This function doesn't return a value
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Prepare()
{
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();
  VCNMaterialCore* materialCore = VCNMaterialCore::GetInstance();

  // Create texture
  mGrassTexture = materialCore->CreateTexture( VCNTXT("Textures\\grass01.bmp") );
  mSandTexture = materialCore->CreateTexture( VCNTXT("Textures\\ground_03.bmp") );
  mSnowTexture = materialCore->CreateTexture( VCNTXT("Textures\\snow.jpg") );
  mGrassRockTexture = materialCore->CreateTexture( VCNTXT("Textures\\ground_02.bmp") );
  mSnowRockTexture = materialCore->CreateTexture( VCNTXT("Textures\\rock512.bmp") );
  
  // Find or create the terrain material.
  mMaterial = VCNResourceCore::GetInstance()->GetResourceID( kDefaultMaterialName );
  if ( mMaterial == kInvalidResID )
  {
    VCNMaterial* material = new VCNMaterial();
    material->SetName( kDefaultMaterialName );
    material->SetAmbientColor( VCNColor(1.f,1.f,1.f,1) );
    material->SetDiffuseColor( VCNColor(1.f,1.f,1.f,1) );
    material->SetSpecularColor( VCNColor(0.5f,1.0f,0.5f,1) );
    material->SetSpecularPower( 12.0f );

    VCNEffectParamSet& params = material->GetEffectParamSet();
    params.SetEffectID( eidTerrain );
		params.AddResource( VCNTXT("DiffuseTexture"), mGrassTexture );
    params.AddResource( VCNTXT("GrassTexture"), mGrassTexture );
    params.AddResource( VCNTXT("SandTexture"), mSandTexture );
    params.AddResource( VCNTXT("SnowTexture"), mSnowTexture );
    params.AddResource( VCNTXT("GrassRockTexture"), mGrassRockTexture );
    params.AddResource( VCNTXT("SnowRockTexture"), mSnowRockTexture );

    // Add material as a resource.
    mMaterial = VCNResourceCore::GetInstance()->AddResource( material->GetName(), material );
  }

  // Create caches
  mVertexCache = renderer->CreateCache( VT_POSITION, mVertexBuffer, VertexCount() * kCacheStrides[VT_POSITION] );
  mLightingCache = renderer->CreateCache( VT_LIGHTING, mLightingBuffer, VertexCount() * kCacheStrides[VT_LIGHTING] );
  mTextureCache = renderer->CreateCache( VT_DIFFUSE_TEX_COORDS, mTexBuffer, VertexCount() * kCacheStrides[VT_DIFFUSE_TEX_COORDS] );
  mIndexCache = renderer->CreateCache( VT_INDEX, mIndexBuffer, IndexCount() * kCacheStrides[VT_INDEX] );

  VCNMesh* mesh = new VCNMesh();
  mesh->SetName( VCNTXT("TerrainMesh") );
  mesh->SetCacheID( VT_POSITION, mVertexCache );
  mesh->SetCacheID( VT_LIGHTING, mLightingCache );
  mesh->SetCacheID( VT_DIFFUSE_TEX_COORDS, mTextureCache );
  //TODO: mMesh->SetBoundingSphere( boundingSphere );
  mesh->SetFaceCache( mIndexCache );
  mesh->SetPrimitiveType( PT_TRIANGLESTRIP );

  // We have a new resource ready, add it!
  mMesh = VCNResourceCore::GetInstance()->AddResource( mesh->GetName(), mesh );
}



////////////////////////////////////////////////////////////////////////
///
///  Release terrain rendering resources
///
///  @param [in]       renderer : renderer
///
///  This function doesn't return a value
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Release()
{
  // Release caches
  VCNResourceCore::GetInstance()->DeleteResource( mVertexCache );
  VCNResourceCore::GetInstance()->DeleteResource( mLightingCache );
  VCNResourceCore::GetInstance()->DeleteResource( mTextureCache );
  VCNResourceCore::GetInstance()->DeleteResource( mIndexCache );
  VCNResourceCore::GetInstance()->DeleteResource( mGrassTexture );
  VCNResourceCore::GetInstance()->DeleteResource( mSnowTexture );
  VCNResourceCore::GetInstance()->DeleteResource( mSandTexture );
  VCNResourceCore::GetInstance()->DeleteResource( mGrassRockTexture );
  VCNResourceCore::GetInstance()->DeleteResource( mSnowRockTexture );
  VCNResourceCore::GetInstance()->DeleteResource( mMaterial );
  VCNResourceCore::GetInstance()->DeleteResource( mMesh );
}



////////////////////////////////////////////////////////////////////////
///
///  Render the terrain geometry
///
///  @param [in]       renderer : renderer
///
///  This function doesn't return a value
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Render() const
{
  if (!mActive)
    return;

  GPU_PROFILE_BLOCK();

  // Draw stuff above the terrain first to minimize overdraw.
  VCNNode::Render();
  
  // Tell the transform manager what the new current transform is
  VCNXformCore::GetInstance()->SetWorldTransform( mWorld );
  
  // Select the current material
  VCNMaterialCore::GetInstance()->SelectMaterial( mMaterial );

  // Render the mesh
  VCNEffectCore::GetInstance()->RenderMesh( mMesh, VCNSphere((VCNFloat)mSize, mLocalTranslation) );
}



////////////////////////////////////////////////////////////////////////
///
/// Determines the height of the map at the given cell.
///
/// @param[in] x, y : the position of the cell. range: 0 to height map size - 1
///
/// @return the value of the height map at that cell.
///
////////////////////////////////////////////////////////////////////////
float ProceduralTerrain::GetCell(int x, int y) const
{
  // make sure we have a terrain
  ASSERT( mCellMap != NULL );

  // check the parameters
  if( ( x < 0 ) || ( x >= mSize ) )
    return 0;
  if( ( y < 0 ) || ( y >= mSize ) )
    return 0;

  return( mCellMap[ x + ( y * mSize ) ] );
}



////////////////////////////////////////////////////////////////////////
///
/// Sets the height of the map at the given cell.
///
/// @param[in] x, y : the position of the cell. range: 0 to height map size - 1
/// @param[in] value : the height to assign to that cell.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::SetCell(int x, int y, float value)
{
  // make sure we have a terrain
  ASSERT( mCellMap != NULL );

  // check the parameters
  ASSERT( ( x >= 0 ) && ( x < mSize ) );
  ASSERT( ( y >= 0 ) && ( y < mSize ) );

  // set the cell
  mCellMap[ x + ( y * mSize ) ] = value;
}



////////////////////////////////////////////////////////////////////////
///
/// Modifies the height of the map at the given cell.
///
/// @param[in] x, y: the position of the cell. range: 0 to height map size - 1
/// @param[in] value: the height to add to that cell.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::OffsetCell(int x, int y, float value)
{
  // make sure we have a terrain
  ASSERT( mCellMap != NULL );

  // check the parameters
  ASSERT( ( x >= 0 ) && ( x < mSize ) );
  ASSERT( ( y >= 0 ) && ( y < mSize ) );

  // offset the cell
  mCellMap[ x + ( y * mSize ) ] += value;
}



////////////////////////////////////////////////////////////////////////
///
/// Clears, regenerates, normalizes, and flattens the terrain using the
/// current parameters. unless the seed is changed, this will generate
/// the same terrain every time. call this after setting the parameters
/// to create a terrain.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Generate( void )
{
  AllocateTerrain();

  // Make sure there is a terrain
  ASSERT( mCellMap != NULL );

  ResetCells();

  // Set the seed
  srand( mSeed );

  // Add as many hills as needed
  for( int i = 0; i < mNumHills; ++i ) 
  {
    AddHill();
  }

  // Now clean it up
  NormalizeCells();
  Flatten();

  // Reestablish randomness
  if ( CST_BOOL("Random.Enabled") )
  {
    // Let this session be unique!
    // We don't want this in debug, so we can track performance in a 
    // more deterministic way.
    srand((unsigned int)time(0));
  }
  else
  {
    srand( CST_INT("Random.Seed") );
  }
}



////////////////////////////////////////////////////////////////////////
///
/// Sets all cells to 0
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::ResetCells( void )
{
  // make sure there is a terrain
  ASSERT( mCellMap != NULL );

  for( int x = 0; x < mSize; ++x ) {
    for( int y = 0; y < mSize; ++y ) {
      SetCell( x, y, 0 );
    }
  }
}



////////////////////////////////////////////////////////////////////////
///
/// Add a random hill to the terrain
///
/// @param[in] : 
///
/// @return 
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::AddHill( void )
{
  // make sure there is a terrain
  ASSERT( mCellMap != NULL );

  // pick a size for the hill
  float fRadius = VCN::Random( mHillMin, mHillMax );

  // pick a center point for the hill
  float x, y;

  x = VCN::Random( -fRadius, mSize + fRadius );
  y = VCN::Random( -fRadius, mSize + fRadius );
  // note that the range of the hill is used to determine the
  // center point. this allows hills to have their center point off the
  // edge of the terrain as long as part of the hill is in bounds. this
  // makes the terrains appear continuous all the way to the edge of the
  // map.

  // square the hill radius so we don't have to square root the distance 
  float fRadiusSq = fRadius * fRadius;
  float fDistSq;
  float fHeight;

  // find the range of cells affected by this hill
  int xMin = (int)(x - fRadius - 1);
  int xMax = (int)(x + fRadius + 1);
  // don't affect cell outside of bounds
  if( xMin < 0 ) xMin = 0;
  if( xMax >= mSize ) xMax = mSize - 1;

  int yMin = (int)(y - fRadius - 1);
  int yMax = (int)(y + fRadius + 1);
  // don't affect cell outside of bounds
  if( yMin < 0 ) yMin = 0;
  if( yMax >= mSize ) yMax = mSize - 1;

  // for each affected cell, determine the height of the hill at that point
  // and add it to that cell
  for( int h = xMin; h <= xMax; ++h ) {
    for( int v = yMin; v <= yMax; ++v ) {
      // determine how far from the center of the hill this point is
      fDistSq = ( x - h ) * ( x - h ) + ( y - v ) * ( y - v );
      // determine the height of the hill at this point
      fHeight = fRadiusSq - fDistSq;

      // don't add negative hill values (i.e. outside the hill's radius)
      if( fHeight > 0 ) {
        // add the height of this hill to the cell
        OffsetCell( h, v, fHeight );
      }  
    }
  }
}



////////////////////////////////////////////////////////////////////////
///
/// Normalizes all cells. Normalizing cells is used to be portable.
/// It is easy to scale a terrain using a normalized terrain.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::NormalizeCells( void )
{
  // make sure there is a terrain
  ASSERT( mCellMap != NULL );

  float fMin = GetCell( 0, 0 );
  float fMax = GetCell( 0, 0 );

  // find the min and max
  for( int x = 0; x < mSize; ++x ) 
  {
    for( int y = 0; y < mSize; ++y ) 
    {
      float z = GetCell( x, y );
      if( z < fMin ) fMin = z;
      if( z > fMax ) fMax = z;
    }
  }

  // avoiding divide by zero (unlikely with floats, but just in case)
  if( fMax != fMin ) 
  {
    // divide every height by the maximum to normalize to ( 0.0, 1.0 )
    for( int x = 0; x < mSize; ++x ) 
    {
      for( int y = 0; y < mSize; ++y ) 
      {
        SetCell( x, y, ( GetCell( x, y ) - fMin ) / ( fMax - fMin ) );
      }
    }
  }
  else 
  {
    // if the min and max are the same, then the terrain has no height, so just clear it
    // to 0.0.
    ResetCells();
  }
}  



////////////////////////////////////////////////////////////////////////
///
/// Flattens the terrain. Can be used to distort the terrain?!?
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Flatten( void )
{
  // make sure there is a terrain
  ASSERT( mCellMap != NULL );

  // if flattening is one, then nothing would be changed, so just skip the
  // process altogether.
  if( mFlattening > 1 ) 
  {
    for( int x = 0; x < mSize; ++x ) 
    {
      for( int y = 0; y < mSize; ++y ) 
      {
        float fFlat   = 1.0;
        float fOriginal  = GetCell( x, y );

        // flatten as many times as desired
        for( int i = 0; i < mFlattening; ++i ) 
        {
          fFlat *= fOriginal;
        }

        // put it back into the cell
        SetCell( x, y, fFlat );
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////
///
/// Calculate the normal using 3 points
///
/// @param[in] pt1, pt2, pt3:  points
/// @param[out] normal: the computed normal
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::CalculateNormal(const Vector3& pt1, const Vector3& pt2, const Vector3& pt3, Vector3& normal) const
{
  Vector3 a(pt1, pt3), b(pt1, pt2);

  normal = CrossProduct(a, b).Normalized(); // right handed
  normal.y = -normal.y;
  normal.x = -normal.x;
}

////////////////////////////////////////////////////////////////////////
///
/// Computes the normal for a specific cell. This normal is calculated
/// using adjacent cells while checking map'S bounds.
///
/// @param[in] x, y: cell index
/// @param[out] normal: computed normal
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::ComputeVertexNormal(int x, int y, Vector3& normal) const
{
  Vector3 faceNormals[4];

  float fx = static_cast<float>(x) * mScale, fy = static_cast<float>(y) * mScale;

  const float heightScale = GetHeight();
  Vector3 pt(  fx,  GetCell(x,  y)   * heightScale ,fy   );
  Vector3 pt1( fx,  GetCell(x,  y-1) * heightScale ,fy-1   );
  Vector3 pt2( fx-1,GetCell(x-1,y)   * heightScale ,fy   );
  Vector3 pt3( fx+1,GetCell(x+1,y)   * heightScale ,fy   );
  Vector3 pt4( fx,  GetCell(x,  y+1) * heightScale ,fy+1  );

  int faceCount = 0;
  if (x > 0 && y > 0) 
  {
    CalculateNormal(pt, pt1, pt2, faceNormals[faceCount++]);
  }
  if (x < (mSize-1) && y > 0) 
  {
    CalculateNormal(pt, pt3, pt1, faceNormals[faceCount++]);
  }
  if (x < (mSize-1) && y < (mSize-1)) 
  {
    CalculateNormal(pt, pt4, pt3, faceNormals[faceCount++]);
  }
  if (x > 0 && y < (mSize-1)) 
  {
    CalculateNormal(pt, pt2, pt4, faceNormals[faceCount++]);
  }

  normal = Vector3::Zero;
  for (int i = 0; i < faceCount; ++i) 
  {
    normal += faceNormals[i];
  }
  normal.Normalize();
}



////////////////////////////////////////////////////////////////////////
///
/// Compiles the VBOs used for rendering. Vertexes are compiling using
/// triangles strip and pushed as rows for performance.
/// 
/// p1-------------p2
/// |             / |
/// |    A     /    |
/// |       /       |
/// |    /     B    |
/// | /             |
/// p3-------------p4
/// |             / |
/// |    C     /    |
/// |       /       |
/// |    /     D    |
/// | /             |
/// p5-------------p6
/// 
/// 
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::Compile()
{
  float s = 0.0;
  float t = 0.0;
  const float tiling = 0.115f;

  mVertexBuffer = new Vector3[ VertexCount() ];
  mLightingBuffer = new Lighting[ VertexCount() ];
  mTexBuffer = new Vector2[ VertexCount() ];
  mIndexBuffer = new unsigned short[ IndexCount() ];

  const float terrainHeight = GetHeight();
  const int HalfTerrainSize = mSize / 2;

  // Create terrain vertices
  int idx = 0;
  for (int y = mSize-1, fy = -HalfTerrainSize; y >=0 ; --y, ++fy)
  {
    for (int x = 0, fx = -HalfTerrainSize; x < mSize; ++x, ++fx, ++idx) 
    {
      Vector3 vertex((float)fx, GetCell(x, y) * terrainHeight, (float)fy);
      vertex.x *= mScale;
      vertex.z *= mScale;
      Vector3 normal;
      Vector2 uv(s, t);

      ComputeVertexNormal(x, y, normal);

      mVertexBuffer[idx] = vertex;
      mLightingBuffer[idx].normal = normal;
      mTexBuffer[idx] = uv;
      
      s += tiling * mScale;
    }

    s = 0;
    t -= tiling * mScale;
  }

  idx = 0;
  for ( int z = 0; z < mSize-1; z++ )
  {
    // Even rows move left to right, odd rows move right to left.
    if ( z % 2 == 0 )
    {
      // Even row
      int x;
      for ( x = 0; x < mSize; x++ )
      {
        mIndexBuffer[idx++] = x + (z * mSize);
        mIndexBuffer[idx++] = x + (z * mSize) + mSize;
      }
      // Insert degenerate vertex if this isn't the last row
      if ( z != mSize - 2)
      {
        mIndexBuffer[idx++] = --x + (z * mSize);
      }
    }
    else
    {
      // Odd row
      int x;
      for ( x = mSize - 1; x >= 0; x-- )
      {
        mIndexBuffer[idx++] = x + (z * mSize);
        mIndexBuffer[idx++] = x + (z * mSize) + mSize;
      }
      // Insert degenerate vertex if this isn't the last row
      if ( z != mSize - 2 )
      {
        mIndexBuffer[idx++] = ++x + (z * mSize);
      }
    }
  }

  // Fix last patch
  if ( mSize % 2 == 1 )
  {
    mIndexBuffer[idx-3] = VertexCount()-mSize;
    mIndexBuffer[idx-2] = VertexCount()-mSize;
    mIndexBuffer[idx-1] = VertexCount()-mSize;
  }
  else
  {
    mIndexBuffer[idx-3] = VertexCount()-1;
    mIndexBuffer[idx-2] = VertexCount()-1;
    mIndexBuffer[idx-1] = VertexCount()-1;
  }

  // Build AABB terrain patch for faster picking.
  const float fmin = std::numeric_limits<float>::max();
  const float fmax = std::numeric_limits<float>::lowest();
  int sliceHeight = mSize / PATCH_COUNT;
  for (int k = 0; k < PATCH_COUNT; ++k)
  {
    Vector3 min(fmin, fmin, fmin), max(fmax, fmax, fmax);
    const int kStart = k * sliceHeight;
    const int kEnd = k == (PATCH_COUNT-1) ? mSize : (k+1) * sliceHeight + 1;
    for (int j = kStart; j < kEnd; ++j)
    {
      for (int i = 0; i < mSize; ++i)
      {
        const Vector3& v = mVertexBuffer[j * mSize + i];

        min.x = std::min(v.x, min.x);
        min.y = std::min(v.y, min.y);
        min.z = std::min(v.z, min.z);

        max.x = std::max(v.x, max.x);
        max.y = std::max(v.y, max.y);
        max.z = std::max(v.z, max.z);
      }
    }

    // Construct AABB
    mTerrainPatch[k] = VCNAabb(min, max);
  }
}



////////////////////////////////////////////////////////////////////////
///
/// Returns the height of a cell after scaling. The height is calculated using
/// bilinear interpolation (http://en.wikipedia.org/wiki/Bilinear_interpolation)
/// Using bilinear interpolation gives a smooth effect when waling on it.
/// The bilinear interpolation use the variable:
///        a---------b
///        |         |
///        |         |
///        |      p  |
///        |         |
///        c---------d
///
/// @param[in] x, y: cell index
///
/// @return the world height (y) coordinate for a cell.
///
////////////////////////////////////////////////////////////////////////
float ProceduralTerrain::GetHeightAt(float x, float y) const
{
  const Vector3& pos = GetWorldTransformation().GetTranslation();
  const float TerrainSize = (float)mSize;
  const float HalfTerrainSize = (float)mSize / (2.0f);
  const float heightScale = GetHeight();

  x = x - pos[0] + HalfTerrainSize;
  y = TerrainSize - (y - pos[1] + HalfTerrainSize);

  Vector2 a(floor(x) ,   floor(y) );
  Vector2 b(floor(x)+1,  floor(y) );
  Vector2 c(floor(x) ,   floor(y)+1 );
  Vector2 d(floor(x)+1 , floor(y)+1 );

  // check the parameters
  if ( ( static_cast<int>(a[0]/mScale) >= 0 ) && ( static_cast<int>(a[0]/mScale) < mSize )  && ( static_cast<int>(a[1]/mScale) >= 0 ) && ( static_cast<int>(a[1]/mScale) < mSize ) &&
       ( static_cast<int>(b[0]/mScale) >= 0 ) && ( static_cast<int>(b[0]/mScale) < mSize )  && ( static_cast<int>(b[1]/mScale) >= 0 ) && ( static_cast<int>(b[1]/mScale) < mSize ) &&
       ( static_cast<int>(c[0]/mScale) >= 0 ) && ( static_cast<int>(c[0]/mScale) < mSize )  && ( static_cast<int>(c[1]/mScale) >= 0 ) && ( static_cast<int>(c[1]/mScale) < mSize ) &&
       ( static_cast<int>(d[0]/mScale) >= 0 ) && ( static_cast<int>(d[0]/mScale) < mSize )  && ( static_cast<int>(d[1]/mScale) >= 0 ) && ( static_cast<int>(d[1]/mScale) < mSize ) ) 
  {

      float heightA = GetCell(static_cast<int>(a[0]/mScale), static_cast<int>(a[1]/mScale)) * heightScale;
      float heightB = GetCell(static_cast<int>(b[0]/mScale), static_cast<int>(b[1]/mScale)) * heightScale;
      float heightC = GetCell(static_cast<int>(c[0]/mScale), static_cast<int>(c[1]/mScale)) * heightScale;
      float heightD = GetCell(static_cast<int>(d[0]/mScale), static_cast<int>(d[1]/mScale)) * heightScale;

      float D = (b[0] - a[0]) * (c[1] - a[1]);
      return (    (heightA / D * (b[0] - (x)) * (c[1] - (y)) ) +
                  (heightB / D * ((x) - a[0]) * (c[1] - (y)) ) +
                  (heightC / D * (b[0] - (x)) * ((y) - a[1]) ) +
                  (heightD / D * ((x) - a[0]) * ((y) - a[1]) ) )  + pos[2];

  }

  return 0;
}



////////////////////////////////////////////////////////////////////////
/// Gets the height of the terrain at @a pos. 
///
/// @param  pos   - The position to check. 
///
/// @retval  The height. 
////////////////////////////////////////////////////////////////////////
float ProceduralTerrain::GetHeightAt(const Vector2& pos) const
{
  return GetHeightAt(pos.x, pos.y);
}



////////////////////////////////////////////////////////////////////////
/// Gets the height of the terrain at @a pos. 
///
/// @param  pos   - The position to check. 
///
/// @retval  The height. 
////////////////////////////////////////////////////////////////////////
float ProceduralTerrain::GetHeightAt(const Vector3& pos) const
{
  return GetHeightAt(pos.x, pos.z);
}

////////////////////////////////////////////////////////////////////////
///
///  Check if the position (x, y) is valid over the terrain.
///
///  @param [in]       x : x world coordinate to validate
///  @param [in]       y : y world coordinate to validate
///
///  @return - true if the position is valid, 
///          - otherwise, false is returned.
///
////////////////////////////////////////////////////////////////////////
bool ProceduralTerrain::IsValidPosition(float x, float y) const
{
  const Vector3& pos = GetWorldTransformation().GetTranslation();
  const float TerrainSize = (float)mSize;
  const float HalfTerrainSize = (float)mSize / (2.0f);

  x = x - pos[0] + HalfTerrainSize;
  y = TerrainSize - (y - pos[1] + HalfTerrainSize);

  Vector2 a(floor(x) ,   floor(y) );
  Vector2 b((floor(x)+1) , floor(y) );
  Vector2 c(floor(x) ,   (floor(y)+1) );
  Vector2 d((floor(x)+1) , (floor(y)+1) );

  // check the parameters
  if ( ( static_cast<int>(a[0]/mScale) >= 0 ) && ( static_cast<int>(a[0]/mScale) < mSize )  && ( static_cast<int>(a[1]/mScale) >= 0 ) && ( static_cast<int>(a[1]/mScale) < mSize ) &&
       ( static_cast<int>(b[0]/mScale) >= 0 ) && ( static_cast<int>(b[0]/mScale) < mSize )  && ( static_cast<int>(b[1]/mScale) >= 0 ) && ( static_cast<int>(b[1]/mScale) < mSize ) &&
       ( static_cast<int>(c[0]/mScale) >= 0 ) && ( static_cast<int>(c[0]/mScale) < mSize )  && ( static_cast<int>(c[1]/mScale) >= 0 ) && ( static_cast<int>(c[1]/mScale) < mSize ) &&
       ( static_cast<int>(d[0]/mScale) >= 0 ) && ( static_cast<int>(d[0]/mScale) < mSize )  && ( static_cast<int>(d[1]/mScale) >= 0 ) && ( static_cast<int>(d[1]/mScale) < mSize ) ) 
  {
      return true;
  }

  return false;
}



////////////////////////////////////////////////////////////////////////
///
/// Allocates the terrain memory. 
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void ProceduralTerrain::AllocateTerrain()
{
  if (mCellMap != NULL)
  {
    delete [] mCellMap;
    mCellMap = NULL;
  }

  // allocate the map
  mCellMap = new float[ mSize * mSize ];

  ASSERT( mCellMap != NULL);
}



////////////////////////////////////////////////////////////////////////
/// Gets the normal vector at (x, y)
///
/// @param  x   - The x world coordinate. 
/// @param  y   - The y world coordinate. 
///
/// @retval  normal
////////////////////////////////////////////////////////////////////////
const Vector3 ProceduralTerrain::GetNormalAt(float x, float y) const
{
  Vector3 n = Vector3::Zero;

  const Vector3& pos = GetWorldTransformation().GetTranslation();
  const float TerrainSize = (float)mSize;
  const float HalfTerrainSize = (float)mSize / (2.0f);

  x = x - pos[0] + HalfTerrainSize;
  y = TerrainSize - (y - pos[1] + HalfTerrainSize);

  ComputeVertexNormal(static_cast<int>(x/mScale), static_cast<int>(y/mScale), n);

  return n;
}

////////////////////////////////////////////////////////////////////////
///
/// Tests the terrain for pixel chunk have been picked.
/// 
/// @param ray    [IN]  3d world ray
/// @param result [OUT] picking results
///
/// @return true if the terrain was picked.
///
const bool ProceduralTerrain::Probe(const VCNRay& ray, TerrainProbeResult& result)
{
  typedef std::vector<int> SliceList;

  bool picked = false;
  SliceList sliceCandidate;

  //const int kEnd = (k * PATCH_COUNT) + sliceHeight;
  const int kSliceHeight = mSize / PATCH_COUNT;

  // Optimize intersection using quad patch testing first
  for (int k = 0; k < PATCH_COUNT; ++k)
  {
    if ( mTerrainPatch[k].Intersects( ray ) )
    {
      sliceCandidate.push_back( k );
    }
  }

  // We should always have one match.
  VCN_ASSERT( !sliceCandidate.empty() );

  while ( !picked && !sliceCandidate.empty() )
  {
    SliceList::const_iterator sliceItr = sliceCandidate.end()-1;
    const int k = *sliceItr;
    const int kStart = k * kSliceHeight;
    const int kEnd = k == (PATCH_COUNT-1) ? mSize - 1 : (k+1) * kSliceHeight;

    for (int j = kStart; !picked && j < kEnd; ++j)
    {
      for (int i = 0; !picked && i < mSize - 1; ++i)
      {
        const Vector3& v1 = mVertexBuffer[  j   * mSize + i];
        const Vector3& v4 = mVertexBuffer[  j   * mSize + (i+1)];
        const Vector3& v2 = mVertexBuffer[(j+1) * mSize + i];
        const Vector3& v3 = mVertexBuffer[(j+1) * mSize + (i+1)];

        picked = IntersectRayTriangle(ray, v1, v2, v4, result.hitpos) || 
                 IntersectRayTriangle(ray, v2, v3, v4, result.hitpos);
        if ( picked )
        {
          // found!
          result.hitcell.Set(i, j);
          result.hitnormal = GetNormalAt(result.hitpos.x, result.hitpos.z);
        } 
      }
    }

    sliceCandidate.erase( sliceItr );
  }

  return picked;
}
