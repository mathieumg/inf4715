///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"

#include "Config.h"
#include "ConfigLoader.h"
#include "ConfigLoaderConstants.h"

#include "VCNUtils/Utilities.h"
#include "VCNUtils/Assert.h"

//////////////////////////////////////////////////////////////////////////
///  Load the configuration file
///
///  @param [in]      filename   name of the XML file to load
///
///  This function returns true if successful
//////////////////////////////////////////////////////////////////////////
VCNBool ConfigLoader::LoadConfig(const VCNString& filename)
{
	if ( LoadConstants(filename) )
	{
#if !defined( FINAL )
		// Try to load a custom user config file
		// It will overrides value loaded previously
		const VCNString userConfigFile = filename + VCNTXT(".user");
		if ( VCN::FileExists(userConfigFile) )
		{
			if ( !LoadConstants(userConfigFile) )
				return false;
		}
#endif // FINAL

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
///  Load constants values
///
///  @param [in]      filename   name of the XML file to load
///
///  This function returns true if successful
//////////////////////////////////////////////////////////////////////////
VCNBool ConfigLoader::LoadConstants( const VCNString& filename )
{
	// Open the XML document
	XMLElementPtr pRootElem = LoadDocumentRoot( filename );
	VCN_ASSERT_MSG( pRootElem != NULL, TEXT("Could not load XML config file!") );

	// Select the Constant node
	XMLNodePtr constantsNode;
	pRootElem->selectSingleNode( TEXT("Constants"), &constantsNode );
	VCN_ASSERT_MSG( constantsNode != NULL, TEXT("Could not load Constants node.") );

	// Select all constants
	XMLNodeListPtr constantNodes;
	constantsNode->selectNodes( TEXT("Constant"), &constantNodes );
	VCN_ASSERT_MSG( constantNodes != NULL, TEXT("Could not select constant nodes.") );

	// Get the config singleton to stock constants into memory
	Config& cfg = Config::GetInstance();

	// Load each constant node into memory
	long constantCount;
	constantNodes->get_length(&constantCount);
	for (long i = 0; i < constantCount; ++i)
	{
		// Get the constant node
		XMLNodePtr node;
		constantNodes->get_item( i, &node );

		VCNString nameAttrValue, typeAttrValue, valueAttrValue;
		if( GetAttributeString(node, TEXT("name"), nameAttrValue) && 
			GetAttributeString(node, TEXT("type"), typeAttrValue) &&
			GetAttributeString(node, TEXT("value"), valueAttrValue))
		{
			cfg.PushConstant(nameAttrValue, typeAttrValue, valueAttrValue);
		}
	}

	return true;
}
