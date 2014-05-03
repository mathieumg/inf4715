///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief 
///

#ifndef TERRAIN_H
#define TERRAIN_H

#pragma once

#include "VCNNodes/Node.h"

class VCNRenderNode;

class Terrain : public VCNNode
{
  VCN_CLASS;

public:

  /// Default constructor
  Terrain(VCNNodeID nodeID);

  /// Destructor
  virtual ~Terrain();

// Accessors

  /// Returns the width of the terrain
  const int GetWidth() const;

  /// Returns the maximum elevation of the terrain from 0
  const int GetHeight() const;

  /// Returns the depth of the terrain
  const int GetDepth() const;

  /// Returns the rendering node mesh ID
  const VCNResID GetMeshID() const;

// Interface

  /// Returns the elevation at a given position
  const float GetHeightAt(float x, float y) const;

  /// Returns the position for (x,y) -> (x,h+offset,y)
  const Vector3 GetPositionAt(float x, float y, float heightOffset = 0.0f) const;

  /// Returns the normal at a given position
  const Vector3 GetNormalAt(float x, float y) const;

private:

  /// Generates a terrain map of each cells
  void GenerateTerrainMap();

// Data members

  // Precomputed information for each discrete point on the terrain
  struct TerrainPoint
  {
    TerrainPoint()
      : height(0)
      , normal(0.0f, 1.0f, 0.0f)
      , sourceCount(0)
    {
    }

    VCNFloat  height;
    Vector3   normal;
    size_t    sourceCount;
  };

  typedef std::vector<TerrainPoint> TerrainRow;
  typedef std::vector<TerrainRow>   TerrainMap;

// Terrain map info

  TerrainMap      mTerrainMap;
  int             mixMin;
  int             mixMax;
  int             miyMin;
  int             miyMax;
  int             mWidth;
  int             mHeight; 

// Rendering info

  VCNRenderNode*  mTerrainNode;
};

//
// INLINES
//

inline const int Terrain::GetWidth() const
{
  return mWidth;
}


inline const int Terrain::GetHeight() const
{
  // TODO: Compute the max height
  return 256;
}


inline const int Terrain::GetDepth() const
{
  return mHeight;
}


///////////////////////////////////////////////////////////////////////


#endif // TERRAIN_H

