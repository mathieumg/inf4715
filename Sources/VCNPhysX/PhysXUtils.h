///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///

#ifndef VCNPHYSXUTILS_H
#define VCNPHYSXUTILS_H

#pragma once

#include "VCNUtils/Matrix.h"

// Forward declarations
class VCNSphere;

namespace VCNPhysxUtils
{
  /// Convert a matrix to a PhysX transform
  PxTransform ToTransform(const Matrix4& mat);

  /// Convert a sphere transform to a PhysX transform
  PxTransform ToTransform(const VCNSphere& sphere);
}

#endif // VCNPHYSXUTILS_H
