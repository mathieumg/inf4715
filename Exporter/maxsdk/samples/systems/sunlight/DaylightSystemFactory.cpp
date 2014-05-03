//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/

#include "DaylightSystemFactory.h"
#include "DaylightSystemFactory2.h"

// Interface instance and descriptor
DaylightSystemFactory* DaylightSystemFactory::sTheDaylightSystemFactory = nullptr;

INode* DaylightSystemFactory::Create(IDaylightSystem*& pDaylight)
{
	IDaylightSystem2* daylight2 = NULL;
	INode* newNode = DaylightSystemFactory2::GetInstance().Create(daylight2, NULL, NULL);
	if (newNode)
	{
		pDaylight = dynamic_cast<IDaylightSystem*>(daylight2->GetInterface(DAYLIGHT_SYSTEM_INTERFACE));
	}
	return newNode;
}

void DaylightSystemFactory::RegisterInstance()
{
	if(nullptr == sTheDaylightSystemFactory)
	{
		sTheDaylightSystemFactory = new DaylightSystemFactory
		(
			DAYLIGHTSYSTEM_FACTORY_INTERFACE,
			_T("DaylightSystemFactory"), // interface name used by maxscript - don't localize it!
			0, 
			NULL, 
			FP_CORE,

			p_end
		);
	}
}

void DaylightSystemFactory::Destroy()
{
	delete sTheDaylightSystemFactory;
	sTheDaylightSystemFactory = nullptr;
}
