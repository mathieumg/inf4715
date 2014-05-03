///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game entry point.x Game logic starts here.
///

#include "Precompiled.h"
#include "ItemStore.h"

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
ItemStore::~ItemStore()
{
}

void ItemStore::addItem(std::string itemId, std::string readableName, std::string imageId)
{
    _items[itemId] = std::make_pair(readableName, imageId);
    ItemStoreData data;
    data.registeredId = itemId;
    data.readableName = readableName;
    data.imageId = imageId;
	Notify(data);
}
