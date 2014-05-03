///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Transformation core module implementation
///

#include "Precompiled.h"
#include "XformCore.h"

#include "VCNUtils/Plane.h"
#include "VCNUtils/Error.h"

// HACK: Using the observer pattern to notify transformation adds 
//        about 2-3 ms to each frame, so for now we will deactivate it 
//        and directly call the systems that need transformation information.
#define USE_OBSERVER_PATTERN 0

#if !USE_OBSERVER_PATTERN
  #include "VCNRenderer/ShaderCore.h"
#endif

VCN_TYPE( VCNXformCore, VCNCore<VCNXformCore> ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNXformCore::VCNXformCore()
  : mProjectionType( PT_PERSPECTIVE )
  , mNear( 1.0f )
  , mFar( 1000.0f )
{
  // Set the basic matrices to identity
  mWorld.SetIdentity();
  mView2D.SetIdentity();
  mView3D.SetIdentity();
  mPerspective.SetIdentity();
  mOrthogonal.SetIdentity();
  mProjection2D.SetIdentity();
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNXformCore::~VCNXformCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNXformCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  // Calculate the dependent matrices
  UpdateViewProjMatrix();
  UpdateWorldViewProjMatrix();

  // We're initialized
  return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNXformCore::Uninitialize()
{
  return BaseCore::Uninitialize();
}

//-------------------------------------------------------------
/// Set the world transformation to the identity matrix.
//-------------------------------------------------------------
void VCNXformCore::ResetWorldTransform( bool update )
{
  mWorld.SetIdentity();

  // Update dependent matrices
  if( update )
  {
    // Recalculate
    UpdateWorldViewProjMatrix();
  }
}


//-------------------------------------------------------------
/// Set the world transformation matrix.
//-------------------------------------------------------------
void VCNXformCore::SetWorldTransform(const Matrix4& world)
{
  // Take in this new world matrix
  mWorld = world;
  mModelView = world * GetViewMatrix();

  // Recalculate dependent matrices
  UpdateWorldViewProjMatrix();

  // Notify of the model-view matrix change.
#if USE_OBSERVER_PATTERN
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_MODEL, world );
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_MODEL_VIEW, mModelView) );
#else
  VCNShaderCore::GetInstance()->SetWorldTransform( mWorld );  
  VCNShaderCore::GetInstance()->SetModelViewMatrix( mModelView );
#endif
}

//-------------------------------------------------------------
/// Get the current projection matrix
//-------------------------------------------------------------
const Matrix4& VCNXformCore::GetProjectionMatrix()
{
  // Current projection type
  switch( mProjectionType )
  {
  case PT_PERSPECTIVE:
    return mPerspective;

  case PT_ORTHOGONAL:
    return mOrthogonal;

  case PT_UI:
    return mProjection2D;
  }

  VCN_ASSERT( false && "Unknown projection type!" );
  return mOrthogonal;
}

//-------------------------------------------------------------
/// Get the current view matrix
//-------------------------------------------------------------
const Matrix4& VCNXformCore::GetViewMatrix()
{
  // Current projection type
  switch( mProjectionType )
  {
  case PT_PERSPECTIVE:
  case PT_ORTHOGONAL:
    return mView3D;

  case PT_UI:
    return mView2D;
  }

  VCN_ASSERT( false && "Unknown projection type!" );
  return mView3D;
}


//-------------------------------------------------------------
/// Calculate and store a matrix combining the effects of
/// the world, view and projection matrices.
//-------------------------------------------------------------
void VCNXformCore::UpdateWorldViewProjMatrix()
{
  // Start with world and view, and add the current projection
  mWorldViewProjection = mWorld * mViewProjection;

  // Let everyone know the matrix has changed
#if USE_OBSERVER_PATTERN
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_MODEL_VIEW_PROJECTION, mWorldViewProjection) );
#else
  VCNShaderCore::GetInstance()->SetWorldViewProjMatrix( mWorldViewProjection );
#endif
}

//-------------------------------------------------------------
/// Calculate and store a matrix with view and projection
/// together.
//-------------------------------------------------------------
void VCNXformCore::UpdateViewProjMatrix()
{
  // Current view with current projection... that's it
  mViewProjection = GetViewMatrix() * GetProjectionMatrix();

  mViewFrustum.Update(mViewProjection);
}


///////////////////////////////////////////////////////////////////////
void VCNXformCore::SetProjectionMatrix(const Matrix4& projection)
{
  // Set current projection matrix
  mProjectionType = PT_PERSPECTIVE;
  mPerspective = projection;
  UpdateViewProjMatrix();
  UpdateWorldViewProjMatrix();
}


//-------------------------------------------------------------
/// Calculate the perspective projection matrix, and set it.
//-------------------------------------------------------------
VCNBool VCNXformCore::SetPerspectiveMatrix( const float FOV, const float aspect )
{
  mPerspective.SetPerspective(FOV, aspect, mNear, mFar);

  return true;
}

//-------------------------------------------------------------
/// Calculate the orthogonal projection matrix, and set it.
//-------------------------------------------------------------
VCNBool VCNXformCore::SetOrthogonalMatrix( const float width, const float height )
{
  mOrthogonal.SetOrthogonality( width, height, mNear, mFar );

  return true;
}

//-------------------------------------------------------------
/// Changes the current projection type and updates matrices.
//-------------------------------------------------------------
void VCNXformCore::SetProjectionType( VCNProjectionType type )
{
  // Don't switch uselessly
  if( mProjectionType != type )
  {
    mProjectionType = type;
    UpdateViewProjMatrix();
    UpdateWorldViewProjMatrix();
  }
}

///////////////////////////////////////////////////////////////////////
void VCNXformCore::SetProjectionType(VCNProjectionType type, const Matrix4& projection)
{
  mProjectionType = type;
  
  // Current projection type
  switch( type )
  {
  case PT_PERSPECTIVE:
    mPerspective = projection;
    break;

  case PT_ORTHOGONAL:
    mOrthogonal = projection;
    break;

  case PT_UI:
    mProjection2D = projection;
    break;

  default:
    VCN_ASSERT( false && "Unknown projection type!" );
    break;
  }

  UpdateViewProjMatrix();
  UpdateWorldViewProjMatrix();
}

//-------------------------------------------------------------
/// Takes the camera data and builds the view matrix from it.
//-------------------------------------------------------------
VCNBool VCNXformCore::SetViewLookAt( const Vector3 &camPos, const Vector3 &camLookat, const Vector3 &camUp ) 
{
  mViewPos = camPos;
  mView3D.SetLookAt(camPos, camLookat, camUp);

  // build final matrix and set for device
  return UpdateView3D();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNXformCore::SetViewLookAt(const Matrix4& view)
{
  mView3D = view;
  mViewPos = view.GetInverse().GetTranslation();

  // build final matrix and set for device
  return UpdateView3D();
}

//-------------------------------------------------------------
/// Takes the camera data and builds the view matrix from it.
//-------------------------------------------------------------
VCNBool VCNXformCore::UpdateView3D()
{
  mNormalMatrix = mView3D;
  mNormalMatrix.SetInverseTranspose();

  // Recalculate the folks that depend about the view matrix
  UpdateViewProjMatrix();
  UpdateWorldViewProjMatrix();

  // Let everyone know the matrix has changed
#if USE_OBSERVER_PATTERN
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_VIEW, mView3D) );
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_NORMAL_MATRIX, mNormalMatrix) );
#else
  VCNShaderCore::GetInstance()->SetViewMatrix( mView3D );
  VCNShaderCore::GetInstance()->SetNormalMatrix( mNormalMatrix );
#endif
  
  // Let everyone know our viewing standpoint has changed
#if USE_OBSERVER_PATTERN
  Notify( VCNMsgTransformChanged(VCNMsgTransformChanged::TC_VIEW_POSITION, mViewPos) );
#else
  VCNShaderCore::GetInstance()->SetViewPosition( mViewPos );
#endif

  return true;
}

//-------------------------------------------------------------
/// Lets set the view2D and projection2D matrices for UI usage.
//-------------------------------------------------------------
VCNBool VCNXformCore::SetUIProjectionMatrix( const float width, const float height )
{
  // Orthogonal projection matrix
  mProjection2D.SetIdentity();
  mProjection2D._11 =  2.0f / width;
  mProjection2D._22 =  2.0f / height;
  mProjection2D._33 =  1.0f / ( mFar - mNear );
  mProjection2D._43 = -mNear * ( 1.0f / (mFar-mNear) );
  mProjection2D._44 =  1.0f;

  // 2D view matrix
  mView2D.SetIdentity();
  mView2D._22 = -1.0f;
  mView2D._41 = -((int)width) + width * 0.5f;
  mView2D._42 = height - (height  * 0.5f);
  mView2D._43 = mNear + 0.1f;

  return true;
}


//-------------------------------------------------------------
/// Set clipping planes and take care of other values that need
/// to be changed then. 
//-------------------------------------------------------------
void VCNXformCore::SetClippingPlanes( const float _near, const float _far ) 
{
  // Take down the values
  mNear = _near;
  mFar  = _far;

  // Insure some coherence
  VCN_ASSERT( mNear >= 0.0f );
  VCN_ASSERT( mFar >= 1.0f );
  VCN_ASSERT( mNear <= mFar );

  // Update the orthogonal projection
  mOrthogonal._33 = 1.0f / (mFar-mNear);
  mOrthogonal._43 = mNear / (mNear-mFar);

  // Update the perspective projection
  mPerspective._33 = mOrthogonal._33 * mFar;
  mPerspective._43 = -mPerspective._33 * mNear;
}


//-------------------------------------------------------------
/// Retrieve active frustum planes, normals pointing outwards.
/// (pass in array of 6 planes).
//-------------------------------------------------------------
VCNBool VCNXformCore::GetFrustrumPlanes( VCNPlane p[] )
{
  // left plane
  p[0].m_vcN.x = -(mViewProjection._14 + mViewProjection._11);
  p[0].m_vcN.y = -(mViewProjection._24 + mViewProjection._21);
  p[0].m_vcN.z = -(mViewProjection._34 + mViewProjection._31);
  p[0].m_fD    = -(mViewProjection._44 + mViewProjection._41);

  // right plane
  p[1].m_vcN.x = -(mViewProjection._14 - mViewProjection._11);
  p[1].m_vcN.y = -(mViewProjection._24 - mViewProjection._21);
  p[1].m_vcN.z = -(mViewProjection._34 - mViewProjection._31);
  p[1].m_fD    = -(mViewProjection._44 - mViewProjection._41);

  // top plane
  p[2].m_vcN.x = -(mViewProjection._14 - mViewProjection._12);
  p[2].m_vcN.y = -(mViewProjection._24 - mViewProjection._22);
  p[2].m_vcN.z = -(mViewProjection._34 - mViewProjection._32);
  p[2].m_fD    = -(mViewProjection._44 - mViewProjection._42);

  // bottom plane
  p[3].m_vcN.x = -(mViewProjection._14 + mViewProjection._12);
  p[3].m_vcN.y = -(mViewProjection._24 + mViewProjection._22);
  p[3].m_vcN.z = -(mViewProjection._34 + mViewProjection._32);
  p[3].m_fD    = -(mViewProjection._44 + mViewProjection._42);

  // near plane
  p[4].m_vcN.x = -mViewProjection._13;
  p[4].m_vcN.y = -mViewProjection._23;
  p[4].m_vcN.z = -mViewProjection._33;
  p[4].m_fD    = -mViewProjection._43;

  // far plane
  p[5].m_vcN.x = -(mViewProjection._14 - mViewProjection._13);
  p[5].m_vcN.y = -(mViewProjection._24 - mViewProjection._23);
  p[5].m_vcN.z = -(mViewProjection._34 - mViewProjection._33);
  p[5].m_fD    = -(mViewProjection._44 - mViewProjection._43);

  // normalize frustum normals
  for( int i=0; i<6; i++ )
  {
    float fL = p[i].m_vcN.Length();
    p[i].m_vcN /= fL;
    p[i].m_fD  /= fL;
  }

  return VCN_OK;
}

VCNBool VCNXformCore::WorldToScreen( const Vector3& position, Vector2& screenPosition )
{
  Vector4D<float> homogeneousPosition = Vector4D<float>(position.x, position.y, position.z, 1);
  homogeneousPosition = homogeneousPosition * mViewProjection;
  
  homogeneousPosition.x = homogeneousPosition.x / homogeneousPosition.w;
  homogeneousPosition.y = homogeneousPosition.y / homogeneousPosition.w;
  homogeneousPosition.z = homogeneousPosition.z / homogeneousPosition.w;

  screenPosition.x = homogeneousPosition.x;
  screenPosition.y = homogeneousPosition.y;

  return (screenPosition.x > -1 && screenPosition.x < 1) && (screenPosition.y > -1 && screenPosition.y < 1);
}
