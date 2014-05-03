///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Resource.h"

#include "ResourceCore.h"

VCN_TYPE( VCNResource, VCNAtom );

///////////////////////////////////////////////////////////////////////
VCNResource::VCNResource() 
  : mResID( kInvalidResID )
  , mResVersion( 1 )
  , mResName( VCNTXT("") )
{
}


///////////////////////////////////////////////////////////////////////
VCNResource::~VCNResource()
{
}


///////////////////////////////////////////////////////////////////////
void VCNResource::Release()
{
  VCNResourceCore::GetInstance()->DeleteResource( mResID, true );
}
