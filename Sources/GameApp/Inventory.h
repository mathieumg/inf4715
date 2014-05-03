///
/// Copyright (C) 2014 - All Rights Reserved
/// All rights reserved. Mathieu M-Gosselin
///
/// @brief Item inventory
///

#ifndef INVENTORY_H
#define INVENTORY_H

#pragma once

#include "VCNUtils/Observer.h"

using namespace patterns;

struct InventoryData {
	std::string removedId;
};

///
/// This class serve as the main game instance. Almost all entry points 
/// are directed to this class.
///
class Inventory : public Subject<InventoryData>
{

public:
	
  /// Default constructor
  Inventory()
  {
  }

  /// Default destructor
  virtual ~Inventory();

  /// Add an item.
  void addItem(std::string itemId);

  /// Remove an item.
  void removeItem(std::string itemId);

  /// List items.
  void listItems(std::vector<std::string>& items);
  
protected:

private:

 std::vector<std::string> _items;
};

#endif // INVENTORY_H
