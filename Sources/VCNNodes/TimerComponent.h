///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "ComponentBase.h"

class VCNTimerComponent :   public VCNComponentBase<VCNTimerComponent>,
                            public VCNIUpdatable
{
public:
    VCNTimerComponent();
    VCNTimerComponent(float timeInSeconds, const std::function<void(VCNNode*)>& onElapsed);

    virtual void Initialise(const Parameters& params) override;
    virtual void Update(VCNFloat dt) override;

private:
    float m_Time;
    std::function<void(VCNNode*)> m_Func;
};

template class VCNComponentBase<VCNTimerComponent>;
