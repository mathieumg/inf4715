///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Sun light node interface
///

#ifndef SUN_H
#define SUN_H

// Engine includes
#include "VCNRenderer/DirectionalLight.h"

class Sun : public VCNDirectionalLight
{
public:

  /// Construct the sun
  explicit Sun(const VCNNodeID nodeID);

  /// Destruct the sun
  virtual ~Sun();

  /// Returns the view matrix of the directional light
  virtual Matrix4 GetViewMatrix() const override;

  /// Returns the light project matrix, by default it is the same as the camera view
  virtual Matrix4 GetProjectionMatrix() const override;

private:
  
  /// Create a tweaking bar for the sun parameters
  void CreateToolbar();

  /// Release the tweak bar.
  void ReleaseToolbar();

  /// Sets the sun direction components
  static void TW_CALL SetFLDir(const void *value, void *clientData);

  /// Reads sun direction for the tweak bar view
  static void TW_CALL GetFLDir(void *value, void *clientData);

// Data members

  TwBar*                mBar;
  Vector3               mPosition;
  VCNFloat              mDistance;
  VCNFloat              mFrustomWidth;
  VCNFloat              mFrustomHeight;
  VCNFloat              mFrustomNear;
  VCNFloat              mFrustomFar;
};

#endif // SUN_H
