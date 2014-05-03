#include "Precompiled.h"
#include "Event.h"

VCNIListener::~VCNIListener()
{}

VCNIEvent::VCNIEvent(const std::string& evtName, VCNIListener* target)
    :   m_EventName( evtName ),
        m_Target(target)
{}

VCNIEvent::~VCNIEvent()
{}