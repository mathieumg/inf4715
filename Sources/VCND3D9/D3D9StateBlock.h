///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines D3D9 state block
///

#pragma once

#include "D3D9ObjectWrapper.h"

#include "VCNRenderer/StateBlock.h"

class VCND3D9StateBlock : public VCND3D9ObjectWrapper<VCNStateBlock>
{
  VCN_CLASS;

public:

  VCND3D9StateBlock();

  virtual ~VCND3D9StateBlock();

  /// Starts recording rendering states
  virtual void BeginRecord() override;

  /// Ends recording state
  virtual void EndRecord() override;

  /// Apply the state block
  virtual void Apply() override;

protected:

private:

  CComPtr<IDirect3DStateBlock9> mStateBlock;

};

