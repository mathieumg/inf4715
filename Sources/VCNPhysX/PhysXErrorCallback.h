///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Interface of the default PhysX error callback manager
///

#pragma once

class VCNPhysxErrorReporter : public physx::PxErrorCallback
{
public:

  VCNPhysxErrorReporter(void);
  ~VCNPhysxErrorReporter(void);

  virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;

};

