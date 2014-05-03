///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Animation resource base class.
///

#ifndef VICUNA_ANIMATION
#define VICUNA_ANIMATION

#include "VCNResources/Resource.h"

class VCNAnimJoint;

//-------------------------------------------------------------
// The animation class
//-------------------------------------------------------------
class VCNAnim : public VCNResource
{
  VCN_CLASS;

public:

  VCNAnim();
  virtual ~VCNAnim();

  // Accessors
  inline void SetTargetName( const VCNString& name );
  inline const VCNString& GetTargetName() const;
  const VCNResID GetTargetID() const;

  // Add a joint
  inline void AddJoint( const VCNResID jointID );

  // Joint accessors
  inline const VCNUInt GetJointCount() const;
  inline const VCNResID GetJointID( VCNUInt index ) const;
  VCNAnimJoint* GetJoint( VCNUInt index ) const;

protected:
  // To what resource name does this animation apply to
  VCNString        mTargetName;

  // All the joints of the animation
  VCNResourceVector    mJoints;
};



//-------------------------------------------------------------
inline void VCNAnim::SetTargetName( const VCNString& name )
{
  mTargetName = name;
}

//-------------------------------------------------------------
inline const VCNString& VCNAnim::GetTargetName() const
{
  return mTargetName;
}

//-------------------------------------------------------------
inline void VCNAnim::AddJoint( const VCNResID jointID )
{
  mJoints.push_back( jointID );
}

//-------------------------------------------------------------
inline const VCNUInt VCNAnim::GetJointCount() const
{
  return (VCNUInt)mJoints.size();
}

//-------------------------------------------------------------
inline const VCNResID VCNAnim::GetJointID( VCNUInt index ) const
{
  return mJoints[index];
}


#endif
