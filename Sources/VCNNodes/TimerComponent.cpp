///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "TimerComponent.h"

VCNTimerComponent::VCNTimerComponent()
{}

VCNTimerComponent::VCNTimerComponent(float timeInSeconds, const std::function<void(VCNNode*)>& onElapsed)
	: m_Time(timeInSeconds)
	, m_Func(onElapsed)
{}

void VCNTimerComponent::Initialise(const Parameters& params)
{}

void VCNTimerComponent::Update(VCNFloat dt)
{
	m_Time -= dt;
	if (m_Time > 0)
		return;

	GetOwner()->GetEvents().InvokeLater(m_Func);
	GetOwner()->RemoveComponent<VCNTimerComponent>();
}



