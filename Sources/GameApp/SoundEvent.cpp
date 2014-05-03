
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Sound Event Implementation
///

#include "Precompiled.h"
#include "SoundEvent.h"

#include "Config.h"

#include "VCNAudio/SoundEmitter2D.h"
#include "VCNAudio/SoundEmitter3D.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn SoundEvent::SoundEvent(VCN2DSoundEmitter* soundEmitter, bool repeatable)
///
/// Constructor for the SoundEvent class.
///
/// @param[im] soundEmitter : the 2D sound to be associated with this event.
/// @param[im] repeatable : whether the sound needs to play in a loop. 
///                         Default value is true.
///
/// @return SoundEvent
///
////////////////////////////////////////////////////////////////////////
SoundEvent::SoundEvent(VCN2DSoundEmitter* soundEmitter, bool repeatable)
	: m2DSoundEmitter(soundEmitter)
	, m3DSoundEmitter(NULL)
{
	mRepeatable = repeatable;
	mActivated = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn SoundEvent::SoundEvent(VCN3DSoundEmitter* soundEmitter, bool repeatable)
///
/// Constructor for the SoundEvent class.
///
/// @param[im] soundEmitter : the 3D sound to be associated with this event.
/// @param[im] repeatable : whether the sound needs to play in a loop. 
///                         Default value is true.
///
/// @return SoundEvent
///
////////////////////////////////////////////////////////////////////////
SoundEvent::SoundEvent(VCN3DSoundEmitter* soundEmitter, bool repeatable)
	: m2DSoundEmitter(NULL)
	, m3DSoundEmitter(soundEmitter)
{
	mRepeatable = repeatable;
	mActivated = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn SoundEvent::~SoundEvent()
///
/// Destructor for the SoundEvent class.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
SoundEvent::~SoundEvent()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn SoundEvent::CallActions()
///
/// This event's execution.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void SoundEvent::CallActions()
{
	if(mRepeatable || !mActivated)
	{     
		// Play the sounds
		if (m2DSoundEmitter != NULL)
		{
			m2DSoundEmitter->Play();
		}
		else
		{
			m3DSoundEmitter->Play();
		}

		mActivated = true;
	}
}
