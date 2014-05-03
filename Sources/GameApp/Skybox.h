///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Skybox node interface
///

#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#pragma once

// Engine includes
#include "VCNNodes/Node.h"
#include "VCNUtils/Sphere.h"

///
/// This class serves a a skybox node. Only one skybox should be added
/// to the scene graph. This skybox supports a day/night transition cycle.
///
class Skybox : public VCNNode
{
public:

  explicit Skybox( VCNNodeID nodeID );
  virtual ~Skybox(void);

private:

  /// Updates the day/night skybox transition.
  virtual void Process(const float elapsedTime) override;

  /// Renders the skybox.
  virtual void Render() const override;

  /// Prepare the skybox box meshes and load textures.
  void Prepare();

  /// Release all acquires resources.
  void Release();

  /// Generates a new material for a specified face.
  const VCNResID CreateFaceMaterial( const VCNResID textureID );

// Data members

  enum {
    Back,
    Front,
    Left,
    Right,
    Top,
    Bottom,
    FaceCount,

    FaceVertexCount = 4
  };

  /// Defines face resources mesh IDs.
  VCNResID  mFaceMeshes[FaceCount];
  
  /// Defines material resources
  VCNResID  mFaceMaterials[FaceCount];

  /// Defines texture resources.
  VCNResID  mFaceTextures[FaceCount];

  /// Defines faces vertex buffers
  Vector3   mVertexBuffers[FaceCount][FaceVertexCount];

  /// Defines texture buffers
  Vector2   mTexBuffers[FaceCount][FaceVertexCount];

  /// Sphere used to render the meshes.
  VCNSphere mSkyboxSphere;
};

#endif // __SKYBOX_H__
