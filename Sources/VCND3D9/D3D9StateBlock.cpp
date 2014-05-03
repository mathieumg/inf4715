///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements D3D9 state block
///

#include "Precompiled.h"
#include "D3D9StateBlock.h"

VCN_TYPE( VCND3D9StateBlock, VCNStateBlock )

///////////////////////////////////////////////////////////////////////
VCND3D9StateBlock::VCND3D9StateBlock()
{
}



///////////////////////////////////////////////////////////////////////
VCND3D9StateBlock::~VCND3D9StateBlock()
{
}



///////////////////////////////////////////////////////////////////////
void VCND3D9StateBlock::BeginRecord()
{
  GetDevice()->BeginStateBlock();
}



///////////////////////////////////////////////////////////////////////
void VCND3D9StateBlock::EndRecord()
{
  GetDevice()->EndStateBlock( &mStateBlock );
}



///////////////////////////////////////////////////////////////////////
void VCND3D9StateBlock::Apply()
{
  mStateBlock->Apply();
}
