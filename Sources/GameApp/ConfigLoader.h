///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef VICUNA_CONFIG_LOADER
#define VICUNA_CONFIG_LOADER

#pragma once

#include "VCNImporter/XMLLoader.h"

class ConfigLoader : public VCNXMLLoader
{
public:

  static VCNBool LoadConfig( const VCNString& filename );

protected:

  static VCNBool LoadConstants( const VCNString& filename );

  // This utility class should not be instanced.
  ConfigLoader();
  ~ConfigLoader();
};

#endif
