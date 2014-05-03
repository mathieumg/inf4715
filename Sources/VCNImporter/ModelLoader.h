///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief This class contains all the facilities to read models
///        from files (XML or .x).
///

#ifndef VCNMODELLOADER_H
#define VCNMODELLOADER_H

#pragma once

#include "XMLLoader.h"

// Forward declarations
class VCNModel;
class VCNNode;

///
/// Model loader utility class
///
class VCNModelLoader : public VCNXMLLoader
{

public:

  /// Loads a supported model file
  static VCNModel* LoadModel(const VCNString& filename);

protected:

  // Loads a Vicuna XML model file
  static VCNModel* LoadModelXML(const VCNString& filename);
  static VCNModel* LoadModelDirectX(const VCNString& filename);

  /// Loads a node in the model.
  static VCNNode* LoadModelElementXML( XMLNodePtr elementNode );

protected:

  // This utility class should not be instanced.
  VCNModelLoader() {}
  virtual ~VCNModelLoader() {}

};

#endif // VCNMODELLOADER_H
