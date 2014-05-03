///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "GPUProfiling.h"

// Project includes
#include "RenderCore.h"

// Engine includes
#include "VCNUtils/Macros.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/StringUtils.h"

// Class constructor. Takes the necessary information and
// composes a string that will appear in PIXfW.
VCNGPUProfiling::VCNGPUProfiling(const VCNTChar* name, const int line)
{
	VCNTChar label[128];
	_sntprintf(label, STATIC_ARRAY_COUNT(label), VCNTXT("%s @ Line %d"), name, line);
	VCNRenderCore::GetInstance()->ProfileBeginEvent( label, VCNColor::FromInteger(rand() % 255, rand() % 255, rand() % 255) );
}

// Makes sure that the BeginEvent() has a matching EndEvent()
// if used via the macro in D3DUtils.h this will be called when
// the variable goes out of scope.
VCNGPUProfiling::~VCNGPUProfiling( )
{
	VCNRenderCore::GetInstance()->ProfileEndEvent();
}
