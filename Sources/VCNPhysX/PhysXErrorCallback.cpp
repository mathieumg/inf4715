///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX default error reporter
///

#include "Precompiled.h"
#include "PhysxErrorCallback.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Types.h"

///////////////////////////////////////////////////////////////////////
VCNPhysxErrorReporter::VCNPhysxErrorReporter(void)
{
}


///////////////////////////////////////////////////////////////////////
VCNPhysxErrorReporter::~VCNPhysxErrorReporter(void)
{
}


///////////////////////////////////////////////////////////////////////
void VCNPhysxErrorReporter::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
  //VCN_ASSERT_FAIL("PhysX Error (%d):\n%s\n\n%s (%d)", code, message, file, line);
}
