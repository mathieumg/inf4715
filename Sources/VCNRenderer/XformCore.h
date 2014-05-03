///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Xform Core
/// Core module for transform management.
///

#ifndef VICUNA_XFORM_CORE
#define VICUNA_XFORM_CORE

#pragma once

#include "VCNCore/Core.h"
#include "VCNUtils/Frustum.h"
#include "VCNUtils/Observer.h"
#include "VCNUtils/RenderTypes.h"

class VCNPlane;

//
// Message used to notify when a transformation has been updated
//
struct VCNMsgTransformChanged
{
  enum TransformChange
  {
    TC_MODEL,
    TC_VIEW,
    TC_PROJECTION,
    TC_MODEL_VIEW,
    TC_MODEL_VIEW_PROJECTION,
    TC_NORMAL_MATRIX,
    TC_VIEW_POSITION
  } type;
  
  const Vector3& vec3;
  const Matrix4& mat4;

  VCNMsgTransformChanged(TransformChange transformType, const Vector3& transform)
    : type( transformType )
    , vec3( transform )
    , mat4( *(Matrix4*)0 )
  {
  }

  VCNMsgTransformChanged(TransformChange transformType, const Matrix4& transform)
    : type( transformType )
    , vec3( *(Vector3*)0 )
    , mat4( transform )
  {
  }
};

//
// Transformation manager
//
class VCNXformCore 
  : public VCNCore<VCNXformCore>
  , public patterns::Subject<VCNMsgTransformChanged>
{
  VCN_CLASS;

public:
  VCNXformCore(void);
  virtual ~VCNXformCore(void);

  // Core module overloads
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;

  // World matrix
  void ResetWorldTransform( bool update=true );
  void SetWorldTransform( const Matrix4& world );

  // Perspective matrices
  void SetProjectionMatrix(const Matrix4& projection);
  VCNBool SetPerspectiveMatrix( const float FOV, const float aspect );
  VCNBool SetOrthogonalMatrix( const float width, const float height );
  VCNBool SetUIProjectionMatrix( const float width, const float height );

  // Functions to recalculate composed matrices
  void UpdateWorldViewProjMatrix();
  void UpdateViewProjMatrix();

  // Change the projection type
  void SetProjectionType( VCNProjectionType type );
  void SetProjectionType( VCNProjectionType type, const Matrix4& projection );

  /// Returns the current project type.
  const VCNProjectionType GetProjectionType() const;

  void SetClippingPlanes( const float near, const float _far );

  VCNBool GetFrustrumPlanes( VCNPlane p[] );
  VCNBool SetViewLookAt( const Matrix4& view );
  VCNBool SetViewLookAt( const Vector3& camPos, const Vector3& camPoint, const Vector3& camUp );

  // Utility function to get the screen space coordinate of a 3D position.
  // Return false if the point is out of screen.
  VCNBool WorldToScreen(const Vector3& position, Vector2& screenPosition);

  // Helper functions to return current matrices
  const Matrix4& GetWorld() const { return mWorld; }
  const Matrix4& GetViewMatrix();
  const Matrix4& GetProjectionMatrix();
  const Vector3& GetViewPosition() const { return mViewPos; }
  const Frustum& GetViewFrustum() const { return mViewFrustum; }

  const float GetNear() const { return mNear; }
  const float GetFar() const { return mFar; }

protected:

  VCNBool UpdateView3D();

  // Type of projection matrix used
  VCNProjectionType mProjectionType;

  // Near and far planes (affect matrices)
  float mNear;
  float mFar;
   
  // Basic matrices
  Matrix4 mWorld;         // World transform
  Matrix4 mView2D;        // 2D View transform
  Matrix4 mView3D;        // 3D View transform
  Matrix4 mPerspective;   // Perspective matrix
  Matrix4 mOrthogonal;    // Orthogonal matrix
  Matrix4 mProjection2D;  // Orthogonal matrix
  Matrix4 mModelView;
  Matrix4 mNormalMatrix;  // (View^-1)T
  Vector3 mViewPos;

  // Composite matrices
  Matrix4 mViewProjection;
  Matrix4 mWorldViewProjection;

  Frustum mViewFrustum;
};

//
// INLINES
//

///////////////////////////////////////////////////////////////////////
inline const VCNProjectionType VCNXformCore::GetProjectionType() const
{
  return mProjectionType;
}

#endif
