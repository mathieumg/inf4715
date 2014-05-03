///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Widget interface. 
///

#ifndef VCNWIDGET_H
#define VCNWIDGET_H

#pragma once

#include "VCNCore/Atom.h"
#include "VCNUtils/TemplateUtils.h"

///
/// The widget is a base class for UI elements.
///
class VCNWidget : public VCNAtom, private VCNNonCopyable
{
  VCN_CLASS;

public:

  /// Destructor
  virtual ~VCNWidget();

  /// Called every frame to update the widget state.
  virtual void Update(const float elapsedTime) = 0;

  /// Called every frame to draw the widget.
  virtual void Draw() const = 0;

protected:

  /// Default constructor
  VCNWidget();

};

#endif // VCNWIDGET_H
