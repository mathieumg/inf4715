///
/// Copyright (C) 2014 - All Rights Reserved
/// All rights reserved. Mathieu M-Gosselin
///
/// @brief Item inventory
///

#ifndef ITEMSTORE_H
#define ITEMSTORE_H

#pragma once

#include "VCNUtils/Observer.h"
#include <map>

using namespace patterns;

struct ItemStoreData {
	std::string registeredId;
    std::string readableName;
    std::string imageId;
};


class ItemStore : public Subject<ItemStoreData>
{

public:
	
  /// Default constructor
    ItemStore()
  {
  }

  /// Default destructor
  virtual ~ItemStore();

  /// Add an item.
  void addItem(std::string itemId, std::string readableName, std::string imageId);
  
protected:

private:

   std::map<std::string, std::pair<std::string, std::string>> _items;
};

#endif // ITEMSTORE_H
