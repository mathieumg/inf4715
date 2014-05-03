///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Declares the declaration class. 
///

#ifndef DECLARATION_H
#define DECLARATION_H

#pragma once

#include "DeclarationElement.h"
#include "VCNUtils/RenderTypes.h"

///
/// Base vertex declaration class
///
class VCNDeclaration
{
public :

  /// Destructor
  virtual ~VCNDeclaration() {}

protected :

  /// Protected constructor
  VCNDeclaration(const VCNDeclarationElementList& list) 
    : mElementList( list )
  {
  }

private:

  const VCNDeclarationElementList mElementList;
};

#endif // DECLARATION_H
