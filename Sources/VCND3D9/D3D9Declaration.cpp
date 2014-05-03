///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Implements the declaration d 3 d class. 
///

#include "Precompiled.h"
#include "D3D9Declaration.h"

////////////////////////////////////////////////////////////
VCNDeclarationD3D9::VCNDeclarationD3D9(const VCNDeclarationElementList& list, IDirect3DVertexDeclaration9* Declaration) 
  : VCNDeclaration(list)
  , mDeclaration(Declaration)
{
}

////////////////////////////////////////////////////////////
VCNDeclarationD3D9::~VCNDeclarationD3D9()
{
  mDeclaration.p->Release();
}

/////////////////////////////////////////////////////////////
IDirect3DVertexDeclaration9* VCNDeclarationD3D9::GetDeclaration() const
{
  return mDeclaration;
}
