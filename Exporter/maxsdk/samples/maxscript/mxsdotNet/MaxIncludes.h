//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// AUTHOR: Larry.Minton / David Cunningham - created Oct 2, 2006
//***********

#pragma once

#pragma managed(push, off)
#include "stdafx.h"
#include <maxscript/maxscript.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/compiler/parser.h>
#include <maxscript/foundation/functions.h>
#include <maxscript/compiler/thunks.h>
#include <maxscript/foundation/hashtable.h>
#include "macrorec.h"
#pragma managed(pop)

#pragma managed(push, on)
namespace MXS_dotNet	{

	// Note that this functionality should probably be added to CStr/WStr at some point
	struct MNETStr
	{
		static MSTR ToMSTR(System::String^);
		static MSTR ToMSTR(System::Exception^, bool InnerException = true);
	};

	/* -------------------- event_delegate_pair  ------------------- */
	// 

	public ref class event_delegate_pair
	{
	public:
		System::String^ m_pEventName;
		System::Delegate^ m_pEventDelegate;
		event_delegate_pair(System::String^ pEventName, System::Delegate^ pEventDelegate)
		{
			m_pEventName = pEventName;
			m_pEventDelegate = pEventDelegate;
		}
	};
}

#pragma managed(pop)
