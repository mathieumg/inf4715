///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "Component.h"

#include "VCNCore/Core.h"
#include "VCNNodes/Node.h"
#include "VCNUtils/Utilities.h"

#include <functional>
#include <type_traits>
#include <unordered_map>


//to supporte creation of components that aren't simply created with the default constructor
//this also permits the creation of abstract components to support the facades that are in place
template<typename T>
struct ComponentTraits
{
    static T* FactoryFunction()
    {
        return new T();
    }
};

class VCNComponentCore : public VCNCore<VCNComponentCore>
{
public:
    VCNComponentCore();
    virtual ~VCNComponentCore();

    template<typename T>
    static VCNIComponent::Type RegisterComponent();

    /// Called at startup
    virtual VCNBool Initialize() override;

    /// Called when the app closes
    virtual VCNBool Uninitialize() override;

    void SetPersonaAlias(const std::string& alias)
    { m_Alias = alias; }

    const std::string& GetPersonaAlias() const
    { return m_Alias; }

    void ReloadPersonas();

    //This clears the nodes current persona and applies a new one
    void GivePersonaTo(VCNNode* node, const std::string& personaName) const;

    //This adds the components of a persona to a node without erasing the existing ones
    void AddPersonaTo(VCNNode* node, const std::string& personaName) const;

    //This keeps all the common Components of the current persona and only add the diff
    void SwitchPersonaTo(VCNNode* node, const std::string& personaName) const;

    static const VCNChar* PERSONA_DIRECTORY;
    static const VCNChar* PERSONA_FILE_EXTENSION;
    static const VCNChar* ROOT_TAG;
    static const VCNChar* NAME_ATTRIBUTE;
    static const VCNChar* ALIAS_ENABLED_ATTRIBUTE;
    static const VCNChar* FALLBACK_ALIAS_ATTRIBUTE;
    static const VCNChar* CLASS_ATTRIBUTE;
    static const VCNChar* COMPONENT_TAG;
    static const VCNChar* BASE_PNA_ATTRIBUTE;
private:
    struct ComponentDefinition
    {
        std::string m_ClassName;
        VCNIComponent::Parameters m_Params;
    };

    typedef std::vector<ComponentDefinition> ComponentDefinitions;

    struct PersonaDefinition
    {
        std::string m_BasePersona;
        ComponentDefinitions m_ComponentDefs;
    };

    typedef std::unordered_map< std::string, PersonaDefinition > PersonaDefinitions;
    typedef std::function<VCNIComponent::Ptr (const VCNIComponent::Parameters&, VCNNode*)> InstantiationFunc;
    typedef std::pair<std::string, InstantiationFunc> InstantiationPair;
    typedef std::unordered_map<InstantiationPair::first_type, InstantiationPair::second_type> InstantationMap;

    PersonaDefinitions::const_iterator GetPersonaDefinition(const std::string& name) const;
    void SignalPersonaApplied(const std::vector<VCNIComponent::Ptr>& node) const;
    bool HasComponentWithName(const VCNNode::ComponentMap& components, const std::string& name) const;

    ComponentDefinitions GetFullPersonaDefinition(const std::string& persona) const;

    std::string m_Alias;
    PersonaDefinitions m_PersonaDefinitions;

    /*
    #### Accessor to static instantiators
    Instantiation map is static method data to prevent static instantiation order problems
    that could be caused due to static registration system
    */
    static InstantationMap& GetInstantiationMap();
};

template<typename T>
VCNIComponent::Type VCNComponentCore::RegisterComponent()
{
    std::string classname = VCN::ExtractTypeName<T>();
    assert(GetInstantiationMap().find(classname) == std::end(GetInstantiationMap()));

    GetInstantiationMap().insert(
        InstantiationPair(classname,
            [](const VCNIComponent::Parameters& params, VCNNode* owner) -> VCNIComponent::Ptr
            {
                //All components must have a parameterless constructor
                VCNIComponent::Ptr value = VCNIComponent::Ptr( ComponentTraits<T>::FactoryFunction() );
                owner->AddComponent(value);
                value->Initialise(params);
                return value;
            }));

    return GetInstantiationMap().size();
}

