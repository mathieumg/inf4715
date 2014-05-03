///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game entry point.x Game logic starts here.
///

#include <algorithm>
#include "Precompiled.h"
#include "Inventory.h"

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
Inventory::~Inventory()
{
}

void Inventory::addItem(std::string itemId)
{
	_items.push_back(itemId);
	InventoryData data;
	Notify(data);
}

void Inventory::removeItem(std::string itemId)
{
	auto item = std::find(_items.begin(), _items.end(), itemId);

	if(item != _items.end())
	{
		InventoryData data;
		data.removedId = *item;
		_items.erase(item);
		Notify(data);
	}
}

void Inventory::listItems(std::vector<std::string>& items)
{
	items = _items;
}

