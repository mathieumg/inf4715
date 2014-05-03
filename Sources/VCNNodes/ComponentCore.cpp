///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "ComponentCore.h"

#include "VCNUtils/PropertyTreeUtils.h"
#include "VCNUtils/Assert.h"
#include "VCNCore/System.h"

//#include <iostream>

//#include <boost/filesystem/operations.hpp>

const VCNChar* VCNComponentCore::PERSONA_DIRECTORY = "Personas";
const VCNChar* VCNComponentCore::PERSONA_FILE_EXTENSION = ".pna";
const VCNChar* VCNComponentCore::ROOT_TAG = "Persona";
const VCNChar* VCNComponentCore::NAME_ATTRIBUTE = "Name";
const VCNChar* VCNComponentCore::CLASS_ATTRIBUTE = "Class";
const VCNChar* VCNComponentCore::ALIAS_ENABLED_ATTRIBUTE = "HasAlias";
const VCNChar* VCNComponentCore::COMPONENT_TAG = "Component";
const VCNChar* VCNComponentCore::FALLBACK_ALIAS_ATTRIBUTE = "FallbackAlias";
const VCNChar* VCNComponentCore::BASE_PNA_ATTRIBUTE = "Base";

std::vector<std::string> GetPersonaFiles();

VCNComponentCore::InstantationMap& VCNComponentCore::GetInstantiationMap()
{
	static InstantationMap instantiationMap;
	return instantiationMap;
}

VCNComponentCore::VCNComponentCore()
{}

VCNComponentCore::~VCNComponentCore()
{}

VCNBool VCNComponentCore::Initialize()
{
	ReloadPersonas();

	return BaseCore::Initialize();
}

void VCNComponentCore::ReloadPersonas()
{
	m_PersonaDefinitions.clear();

	auto trees = VCNPTree::ConvertFilesToTrees(GetPersonaFiles());

	std::for_each( std::begin(trees), std::end(trees), [&](boost::property_tree::ptree& fileRoot)
	{
		using namespace boost::property_tree;

		std::string file = fileRoot.get<std::string>( VCNPTree::FILE_ATTRIBUTE );

		ptree& persona = fileRoot.get_child(VCNComponentCore::ROOT_TAG);
		VCNPTree::FlattenAttributes( persona );

		std::string personaName = persona.get<std::string>(VCNComponentCore::NAME_ATTRIBUTE);

		auto& bhvDefinition = m_PersonaDefinitions[personaName];
		bhvDefinition.m_ComponentDefs.clear();

		//Set the base persona of our definition
		bhvDefinition.m_BasePersona = persona.get_optional<std::string>(BASE_PNA_ATTRIBUTE).get_value_or("");

		bool hasAliases = persona.get_optional<bool>( VCNComponentCore::ALIAS_ENABLED_ATTRIBUTE ).get_value_or(false);
		if (hasAliases)
		{
			//swap persona root with current alias
			//this lets us define different personas
			//depending on the execution context
			auto aliasedPersona = persona.get_child_optional( m_Alias );
			if (!aliasedPersona.is_initialized())
			{
				std::string fallbackAlias = persona.get<std::string>( VCNComponentCore::FALLBACK_ALIAS_ATTRIBUTE );
				aliasedPersona = persona.get_child_optional( fallbackAlias );

				VCN_ASSERT_MSG( aliasedPersona.is_initialized(), "No alias defined in %s for %s and no fallback specified", personaName, m_Alias.c_str() );
			}

			persona = aliasedPersona.get();
		}

		std::for_each( std::begin(persona), std::end(persona), [&](const std::pair<std::string, ptree>& pair)
		{
			if (pair.first != VCNComponentCore::COMPONENT_TAG)
				return;

			using namespace boost::property_tree;

			VCNComponentCore::ComponentDefinition definition;
			definition.m_ClassName = pair.second.get<std::string>(VCNComponentCore::CLASS_ATTRIBUTE);
			bool classExists = GetInstantiationMap().find(definition.m_ClassName) != std::end(GetInstantiationMap());

			static char* troubleshooting = 
				"TROUBLE SHOOTING\n"
				"- if this pops up make sure the class name is right in the .bhv file\n"
				"- if you don't use/include your component anywhere in your code, the template will never be instantiated\n"
				"- make sure to explicitly instantiate your type at the end of your definition with:\n"
				"template class ComponentBase<PhysicsComponent>;\n";

			VCN_ASSERT_MSG( classExists, "Instantiator for %s doesn't exist. %s", definition.m_ClassName.c_str(), troubleshooting );

			definition.m_Params = pair.second;

			bhvDefinition.m_ComponentDefs.push_back(definition);
		});

	});
}

VCNBool VCNComponentCore::Uninitialize()
{
	return BaseCore::Uninitialize();
}

void VCNComponentCore::GivePersonaTo(VCNNode* node, const std::string& personaName) const
{
	auto copy = node->GetComponents();
	node->RemoveAllComponents();    //once the function returns

	AddPersonaTo(node, personaName);
}

void VCNComponentCore::AddPersonaTo(VCNNode* node, const std::string& personaName) const
{
	ComponentDefinitions componentDefs = GetFullPersonaDefinition(personaName);
	if (componentDefs.empty())
		return;

	std::vector<VCNIComponent::Ptr> createdComponents;

	auto& instantiationMap = GetInstantiationMap();

	std::for_each(std::begin(componentDefs), std::end(componentDefs),
		[&](const ComponentDefinition& definition)
	{
		//this creates and initialises the component and adds it to the VCNNode
		VCNIComponent::Ptr built = instantiationMap[definition.m_ClassName](definition.m_Params, node);
		createdComponents.push_back(built);
	});

	SignalPersonaApplied(createdComponents);
}

void VCNComponentCore::SwitchPersonaTo(VCNNode* node, const std::string& personaName) const
{
	ComponentDefinitions componentDefs = GetFullPersonaDefinition(personaName);
	if (componentDefs.empty())
		return;

	//remove all components not found in new definition
	std::vector<VCNIComponent::Type> toErase;
	const auto& components = node->GetComponents();
	for(auto it = components.begin(); it != components.end(); ++it)
	{
		std::string componentName = it->second->GetName();
		auto existsInNewItr = std::find_if( componentDefs.begin(), componentDefs.end(),
			[&componentName](ComponentDefinition& def)
		{
			return def.m_ClassName == componentName;
		});

		//if we didn't find a component of the same type in new persona
		//remove that son of a bitch
		if (existsInNewItr == componentDefs.end())
			toErase.push_back(it->first);
	}

	std::for_each( toErase.begin(), toErase.end(), [&node](VCNIComponent::Type type)
	{  
		node->RemoveComponent(type);
	});


	std::vector<VCNIComponent::Ptr> createdComponents;

	auto& instantiationMap = GetInstantiationMap();

	//create only the new ones
	std::for_each(std::begin(componentDefs), std::end(componentDefs),
		[&](const ComponentDefinition& componentDef)
	{
		if (HasComponentWithName(node->GetComponents(), componentDef.m_ClassName))
			return; //dont create component of same type

		//this creates and initialises the component and adds it to the VCNNode
		VCNIComponent::Ptr built = instantiationMap[componentDef.m_ClassName](componentDef.m_Params, node);
		createdComponents.push_back(built);
	});

	SignalPersonaApplied(createdComponents);
}

VCNComponentCore::PersonaDefinitions::const_iterator VCNComponentCore::GetPersonaDefinition(const std::string& name) const
{
	auto def = m_PersonaDefinitions.find(name);
	VCN_ASSERT_MSG( def != std::end(m_PersonaDefinitions), "Persona %s doesn't exist", name.c_str());
	return def;
}

void VCNComponentCore::SignalPersonaApplied(const std::vector<VCNIComponent::Ptr>& created) const
{
	//Tell all components that every thing is done
	std::for_each(created.begin(), created.end(), [](VCNIComponent::Ptr comp)
	{
		comp->OnPersonaApplied();
	});
}

VCNComponentCore::ComponentDefinitions VCNComponentCore::GetFullPersonaDefinition(const std::string& persona) const
{
	auto def = GetPersonaDefinition(persona);
	VCN_ASSERT_MSG( def != m_PersonaDefinitions.end(), "Error finding persona: %s", persona.c_str() );
	if (def == m_PersonaDefinitions.end())
		return ComponentDefinitions();

	ComponentDefinitions definitions = def->second.m_ComponentDefs;

	if (!def->second.m_BasePersona.empty())
	{
		auto baseComponents = GetFullPersonaDefinition(def->second.m_BasePersona);
		definitions.insert(definitions.end(), baseComponents.begin(), baseComponents.end());
	}

	return definitions;
}

bool VCNComponentCore::HasComponentWithName(const VCNNode::ComponentMap& components, const std::string& name) const
{
	auto itr = std::find_if(components.begin(), components.end(),
		[&name](const VCNNode::ComponentPair& pair)
	{
		return pair.second->GetName() == name;
	});

	return itr != components.end();
}

//// HELPER METHODS

std::vector<std::string> GetPersonaFiles()
{
	std::vector<std::string> found_files;

	auto loaderCB = [&found_files](const VCNTChar* path)
	{
		found_files.push_back( StringUtils::WStringToString(path) );
	};

	const VCNString directory = StringBuilder() << VCNTXT(".\\") << VCN_A2W(VCNComponentCore::PERSONA_DIRECTORY);
	VCNSystem::GetInstance()->FindFilesRecursively( directory.c_str(), VCNTXT("*.pna"), loaderCB );

	return found_files;
}
