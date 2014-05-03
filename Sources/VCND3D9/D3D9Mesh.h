///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief DirectX mesh wrapper interface
///

#ifndef VCNDXMESH_H
#define VCNDXMESH_H

#pragma once

#include "VCNResources/Mesh.h"

class VCNDXMesh : public VCNMesh
{
public:

  /// Default ctor
  VCNDXMesh(void);

  /// Default dtor
  virtual ~VCNDXMesh(void);

  /// Load the DirectX mesh from a file on disk
  const VCNBool LoadFromFile( const VCNString& filename );

private:

  /// Compute the bounding sphere based on mesh data
  const VCNBool ComputeBoundingSphere(ID3DXMesh* mesh);
};

#endif // VCNDXMESH_H
