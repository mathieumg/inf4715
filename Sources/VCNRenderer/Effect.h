///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna Effect base class.
///

#ifndef VCNEFFECT_H
#define VCNEFFECT_H

#pragma once

#include "VCNCore/Atom.h"

class VCNEffectParamSet;
class VCNMesh;
class VCNSphere;

class VCNEffect : public VCNAtom
{
  VCN_CLASS;

public:

  /// Destructor
  virtual ~VCNEffect() = 0;

  /// Prepare the effect before being used
  virtual void Prepare() = 0;

  /// Renders a mesh using the effect.
  virtual void RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams) = 0;

protected:

  /// Default constructor
  VCNEffect();

};

#endif // VCNEFFECT_H
