///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Animation Joint resource base class.
///

#ifndef VICUNA_ANIMATION_JOINT
#define VICUNA_ANIMATION_JOINT

#include "VCNResources/Resource.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/Types.h"

class VCNNode;
class VCNAnimationController;

//-------------------------------------------------------------
// The animation joint class
//-------------------------------------------------------------
class VCNAnimJoint : public VCNResource
{
  VCN_CLASS;

public:

  VCNAnimJoint();
  virtual ~VCNAnimJoint();

  // Accessors
  void SetTargetName( const VCNString& name );
  const VCNString& GetTargetName() const;
  VCNNode* FindTargetNode(VCNNode* rootNode) const;
  const VCNFloat GetDuration() const;

  // Add new frames
  void    AddPosFrame( VCNFloat time, const Vector3& posFrame );
  void    AddRotFrame( VCNFloat time, const VCNQuat& rotFrame );
  void    AddScaleFrame( VCNFloat time, const Vector3& rotFrame );

  // Return the bounding frames for a certain time
  VCNFloat GetBoundingPositions( const VCNFloat time, Vector3& lowerBound, Vector3& upperBound );
  VCNBool  GetPositionAtTime( VCNFloat time, Vector3& resultVector );
  VCNFloat GetBoundingRotations( const VCNFloat time, VCNQuat& lowerBound, VCNQuat& upperBound );
  VCNBool  GetRotationAtTime( VCNFloat time, Matrix4& resultMatrix );
  VCNFloat GetBoundingScales( const VCNFloat time, Vector3& lowerBound, Vector3& upperBound );
  VCNBool  GetScaleAtTime( VCNFloat time, Vector3& resultMatrix );

protected:

  // Make things easier on ourselves
  typedef std::less<VCNFloat> LessFloatFunctor;
  typedef std::map<VCNFloat,Vector3,LessFloatFunctor> PosFrameMap;
  typedef std::map<VCNFloat,VCNQuat,LessFloatFunctor> RotFrameMap;
  typedef std::map<VCNFloat,Vector3,LessFloatFunctor> ScaleFrameMap;

  // The name of the element it applies to
  VCNString  mTargetName;

  // The length of the animation
  VCNFloat  mDuration;

  // Contains all our frames, sorted by time
  PosFrameMap    mPosFrames;
  RotFrameMap    mRotFrames;
  ScaleFrameMap  mScaleFrames;
};



//-------------------------------------------------------------
inline void VCNAnimJoint::SetTargetName( const VCNString& name )
{
  mTargetName = name;
}

//-------------------------------------------------------------
inline const VCNString& VCNAnimJoint::GetTargetName() const
{
  return mTargetName;
}

//-------------------------------------------------------------
inline const VCNFloat VCNAnimJoint::GetDuration() const
{
  return mDuration;
}

#endif
