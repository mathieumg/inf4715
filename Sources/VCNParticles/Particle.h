///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines the particle basic structure
///

#ifndef VCNPARTICLE_H
#define VCNPARTICLE_H

#pragma once

#include "VCNUtils/Vector.h"

struct VCNParticle
{
  VCNParticle();

  Vector3 pos;
  Vector3 vel;
  float   size;
  float   life;
  float   initlife;
  float   rotation, rotRate;
  bool    alive;
  bool    active;
};

#endif // VCNPARTICLE_H
