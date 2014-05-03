///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "Component.h"
#include "ComponentCore.h"

#include "VCNUtils/Utilities.h"

#include <type_traits>



/*
In order to use the component system, all components must inherit directly from VCNComponentBase
and pass their own type as the template parameter.

Also all components must make sure to explicitly instantiate themselves to make sure they are
handle by VCNComponentCore

ex:
class MyComponent : public VCNComponentBase<MyComponent>
{
######### override abstract methods here #########
};

//DONT FORGET THIS: (this makes the compiler explicitly instantiate the template)
template class VCNComponentBase<MyComponent>;

*/

/* HEADS UP!
Curiously recurring template pattern, put final concrete type as argument
*/
template<typename T>
class VCNComponentBase : public VCNIComponent
{
public:
    virtual ~VCNComponentBase() = 0;

    virtual Type GetType() const override;
    virtual const std::string& GetName() const override;
    virtual VCNIComponent::BehaviorFlags GetBehaviorFlags() const override;
    
    static const VCNIComponent::BehaviorFlags BHV_FLAGS;
    static const Type TYPE;
    static const std::string NAME;

};

template<typename T>
VCNIComponent::BehaviorFlags VCNComponentBase<T>::GetBehaviorFlags() const
{
    return VCNComponentBase<T>::BHV_FLAGS;
}

template<typename T>
VCNIComponent::Type VCNComponentBase<T>::GetType() const
{
	return VCNComponentBase<T>::TYPE;
}

template<typename T>
const std::string& VCNComponentBase<T>::GetName() const
{
	return VCNComponentBase<T>::NAME;
}

template<typename T>
const VCNIComponent::BehaviorFlags VCNComponentBase<T>::BHV_FLAGS = VCNIComponent::GetFlagsForType<T>();

template<typename T>
const VCNIComponent::Type VCNComponentBase<T>::TYPE = VCNComponentCore::RegisterComponent<T>();

template<typename T>
const std::string VCNComponentBase<T>::NAME = VCN::ExtractTypeName<T>();

template<typename T>
VCNComponentBase<T>::~VCNComponentBase()
{}

