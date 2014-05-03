///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Procedural terrain interface
///

#ifndef __TERRAIN_NODE_H__
#define __TERRAIN_NODE_H__

#include "VCNNodes/Node.h"
#include "VCNUtils/Aabb.h"

class VCNRay;

struct TerrainProbeResult
{
  Vector2i hitcell;
  Vector3  hitpos;
  Vector3  hitnormal;
};

////////////////////////////////////////////////////////////////////////
/// @class  Terrain
///
/// @brief  The terrain node acts as a based static terrain. The terrain is
///         generated procedurally.
////////////////////////////////////////////////////////////////////////
class ProceduralTerrain : public VCNNode
{
public:

  static VCNString kDefaultMaterialName;

  /// Construct a terrain
  ProceduralTerrain(
    VCNNodeID nodeID,
    short     size       = 256,
    float     hillMin    = 0.0f,
    float     hillMax    = 10.0f,
    short     numHills   = 1000,
    short     flattening = 2,
    int       seed        = 12345);

  /// Destruct a terrain
  virtual ~ProceduralTerrain();

  /// Returns the size of the height map in either direction.
  int GetSize() const { return mSize; }

  /// Returns the minimum height of hills
  float GetMinHillHeight() const { return mHillMin; }

  /// Returns the maximum height of hills
  float GetMaxHillHeight() const { return mHillMax; }

  /// Return the height of the terrain
  const float GetHeight() const { return mHillMax - mHillMin; }

  /// Returns the number of hills on the map
  short GetHillCount() const { return mNumHills; }

  /// Returns the flattening state of the terrain
  short GetFlattening() const { return mFlattening; }

  /// Returns the seed for terrain generation
  int GetSeed() const { return mSeed; }

  /// Determines the height of the map at the given cell.
  float GetCell(int x, int y) const;

  /// Sets the height of the map at the given cell.
  void SetCell(int x, int y, float value);

  /// Modifies the height of the map at the given cell.
  void OffsetCell(int x, int y, float value);

  /// Returns the height of the terrain in world coordinate at (x, y).
  float GetHeightAt(float x, float y) const;

  /// Returns the height of the terrain at @a pos
  float GetHeightAt(const Vector2& pos) const;

  /// Returns the height of the terrain at @a pos
  float GetHeightAt(const Vector3& pos) const;

  /// Returns the normal at (x, y) in terrain coordinate
  const Vector3 GetNormalAt(float x, float y) const;

  /// Return true if the coordinates are over the terrain.
  bool IsValidPosition(float x, float y) const;

  /// Tries to intersect the terrain and returns the intersection hit if any.
  const bool Probe(const VCNRay& ray, TerrainProbeResult& result);

  /// Prepare the object view state
  void Prepare();

  /// Indicate the object to Release it view state
  void Release();

//
// VCNNode overrides
//

  /// Displays the terrain
  virtual void Render() const override;

private:

  // Object not copyable
  ProceduralTerrain(const ProceduralTerrain&);
  ProceduralTerrain& operator = (const ProceduralTerrain&);

  /// Generates the cells of the terrain using previously set properties.
  void Generate();

  /// Compile 3D vertex information
  void Compile();

  /// Adds a hill at random position
  void AddHill();

  /// Normalizes all cells
  void NormalizeCells();

  /// Sets all cells to 0
  void ResetCells();

  /// Flattens the terrain.
  void Flatten();

  /// Allocates the memory used by the terrain.
  void AllocateTerrain();

  /// Calculate the normal using 3 points
  void CalculateNormal(const Vector3& pt1, const Vector3& pt2, const Vector3& pt3, Vector3& normal) const;

  /// Compute a vertex's normal for a specific cell.
  void ComputeVertexNormal(int x, int y, Vector3& normal) const;

  /// Returns the number of vertex generated.
  const int VertexCount() const { return mSize * mSize; }

  /// Returns the number of vertex generated.
  const int IndexCount() const { return (mSize * 2) * (mSize - 1) + (mSize - 2); }

// Data members

  static const int PATCH_COUNT = 32;

  VCNResID            mVertexCache;
  VCNResID            mLightingCache;
  VCNResID            mTextureCache;
  VCNResID            mIndexCache;

  VCNResID            mGrassTexture;
  VCNResID            mSandTexture;
  VCNResID            mSnowTexture;
  VCNResID            mGrassRockTexture;
  VCNResID            mSnowRockTexture;

  VCNResID            mMaterial;

  VCNResID            mMesh;

  struct Lighting
  {
    Lighting()
    {
      color.Set(1,1,1);
    }

    Vector3   normal;
    Vector3   color;
  };

  Vector3*            mVertexBuffer;
  Lighting*           mLightingBuffer;
  Vector2*            mTexBuffer;
  unsigned short*     mIndexBuffer;

  short               mSize;         ///< The size of the terrain. (size x size)
  float               mHillMin;      ///< Min hill height
  float               mHillMax;      ///< Max hill height
  short               mNumHills;     ///< Maximum number of hill to generate
  short               mFlattening;   ///< Are we flattening the terrain?
  int                 mSeed;         ///< Seed used to generate terrain
  float*              mCellMap;      ///< Buffer of cells
  float               mScale;

  VCNAabb             mTerrainPatch[PATCH_COUNT];
};

#endif // __TERRAIN_NODE_H__
