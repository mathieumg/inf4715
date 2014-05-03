///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "Component.h"

#include "VCNUtils/Utilities.h"

#include <functional>
#include <string>
#include <type_traits>

class VCNIComponent;

class VCNIListener
{
public:
	virtual ~VCNIListener() = 0;
};

class VCNIEvent
{
public:
	typedef std::shared_ptr<VCNIEvent> Handle;

	VCNIEvent(const std::string& evtName, VCNIListener* target);

	virtual ~VCNIEvent() = 0;

	const std::string& GetEventName() const
	{ return m_EventName; }

	VCNIListener* GetTarget() const
	{ return m_Target; }

private:

	/// No Copy
	VCNIEvent(const VCNIEvent& other);

	std::string m_EventName;
	VCNIListener* m_Target;
};

template<typename EventType>
class VCNEvent : public VCNIEvent
{
public:
	typedef std::function<void (const EventType&)> Callback;
	typedef EventType Event;

	template<typename ListenerType>
	VCNEvent(ListenerType* listener, void (ListenerType::*callback)(const EventType&));

	VCNEvent(const Callback& callback);

	void Fire(const EventType& evt);
private:
	Callback m_callback;
};

template<typename EventType>
template<typename ListenerType>
VCNEvent<EventType>::VCNEvent(ListenerType* listener, void (ListenerType::*callback)(const EventType&))
	: VCNIEvent( VCN::ExtractTypeName<EventType>(), listener )
{
	static_assert( std::is_base_of<VCNIListener, ListenerType>::value, "listener must be of type VCNIListener");
	/*
	This constraint is only to make sure we don't have any pointer problems. This would only be the case if 
	we were playing with multiple inheritance and calling this function with a template parameter for the listener
	at different levels (lets say VCNIComponent and PhysXComponent), in this case the values of the pointers may be
	different but will definitely be pointing to the same listener.

	Hence the behavior could be drastically different. If the values are different then the two are considered different listeners
	so each caller has to call unregister (even if they are actually the same caller), which is ok as it gives us more control. The problem
	is that it won't be homogeneous. One call to remove all listeners could remove everything for some, and not others depending on the inheritance
	hierarchy.

	That is why we enforce the use of VCNIListener, it could be anything, it just all has to be the same class.

	Although if VCNIListener is found multiple times in the hierarchy, each element inheriting
	from VCNIListener would have to unregister (if said object has indeed registered himself).
	*/
	m_callback = std::bind(callback, listener, std::placeholders::_1);
}

template<typename EventType>
VCNEvent<EventType>::VCNEvent(const Callback& callback)
	: VCNIEvent( VCN::ExtractTypeName<EventType>(), nullptr )
{
	m_callback = callback;
}

template<typename EventType>
void VCNEvent<EventType>::Fire(const Event& evt)
{
	m_callback(evt);
}



