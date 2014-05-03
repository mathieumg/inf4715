///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef VCNDECLARATIOND3D9_H
#define VCNDECLARATIOND3D9_H

#pragma once

#include "VCNRenderer/Declaration.h"

// Forward declaration
struct IDirect3DVertexDeclaration9;

///
/// D3D9 specific vertex declaration
///
class VCNDeclarationD3D9 : public VCNDeclaration
{
public:

  /// Release resources
  virtual ~VCNDeclarationD3D9();

  /// Default constructor
  VCNDeclarationD3D9(const VCNDeclarationElementList& list, IDirect3DVertexDeclaration9* Declaration);

  /// Returns the vertex declaration
  IDirect3DVertexDeclaration9* GetDeclaration() const;

private :

// Data members

  /// D3D9 vertex declaration
  CComPtr<IDirect3DVertexDeclaration9> mDeclaration;
};

#endif // VCNDECLARATIOND3D9_H
