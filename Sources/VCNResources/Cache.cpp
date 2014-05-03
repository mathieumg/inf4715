///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "Cache.h"


//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
VCNCache::VCNCache() : VCNResource()
, mCacheType( VT_NUM_VERTEX_TYPES )
, mCacheFlags( CF_NONE )
, mBufferHandle( NULL )
, mBufferSize( 0 )
, mCount( 0 )
, mStride( 0 )
, mFilled(false)
{
}

//-------------------------------------------------------------
// Destructor
//-------------------------------------------------------------
VCNCache::~VCNCache( void )
{
}
