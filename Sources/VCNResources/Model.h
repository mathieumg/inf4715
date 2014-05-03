///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Model resource base class.
///

#ifndef VICUNA_MODEL
#define VICUNA_MODEL

#pragma once

#include "VCNResources/Resource.h"

class VCNNode;

//-------------------------------------------------------------
// The model class
//-------------------------------------------------------------
class VCNModel : public VCNResource
{
  VCN_CLASS;

public:

  VCNModel( VCNNode* modelRoot );
  virtual ~VCNModel();

  // Creates an instance of this model
  VCNNode* CreateInstance();

protected:

  // The root node for the entire model
  VCNNode* mModelRoot;
};

#endif
