///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines abstract rendering state blocks
///

#ifndef VCNSTATEBLOCK_H
#define VCNSTATEBLOCK_H

#pragma once

#include "VCNResources/Resource.h"

class VCNStateBlock : public VCNResource
{
  VCN_CLASS;

public:

  virtual ~VCNStateBlock();

  /// Starts recording rendering states
  virtual void BeginRecord() = 0;

  /// Ends recording state
  virtual void EndRecord() = 0;

  /// Apply the state block
  virtual void Apply() = 0;

protected:

  VCNStateBlock();
};

#endif // VCNSTATEBLOCK_H
