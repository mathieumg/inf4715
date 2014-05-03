///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Anim.h"

// Project includes
#include "AnimJoint.h"

// Engine includes
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNAnim, VCNResource ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNAnim::VCNAnim() : VCNResource()
,mTargetName( VCNTXT("") )
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNAnim::~VCNAnim()
{
}

//-------------------------------------------------------------
/// Converts the name to an ID, it's not fast, but it's handy.
//-------------------------------------------------------------
const VCNResID VCNAnim::GetTargetID() const
{
  return VCNResourceCore::GetInstance()->GetResourceID( mTargetName );
}

//-------------------------------------------------------------
/// Returns a pointer to the joint directly (takes care of
/// fetching and casting the resource for you).
//-------------------------------------------------------------
VCNAnimJoint* VCNAnim::GetJoint( VCNUInt index ) const
{
  return VCNResourceCore::GetInstance()->GetResource<VCNAnimJoint>( mJoints[index] );
}
