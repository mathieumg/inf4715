///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "MaterialLoader.h"

// Project includes
#include "MaterialLoaderConstants.h"

// Engine includes
#include "VCNResources/ResourceCore.h"
#include "VCNResources/Material.h"
#include "VCNRenderer/MaterialCore.h"

//-------------------------------------------------------------
void VCNMaterialLoader::LoadMaterial( const VCNString& filename )
{
  // Make sure we at least have a char and an extension
  VCN_ASSERT( filename.length()>7 && "Incorrect filename!" );

  MaterialList retValue;

  // Is it an XML format or a binary format?
  if( filename.rfind(kXMLMaterialExtension) != VCNString::npos )
  {
    LoadMultipleMaterialXML( filename, retValue );
  }
  else
  {
    VCN_ASSERT( false && "Unsupported material extension!" );
  }

  // Add this material to the resource manager
  for(unsigned int i = 0; i < retValue.size(); ++i)
  {
    // Make sure we aren't loading it twice
    VCN_ASSERT_MSG( !VCNResourceCore::GetInstance()->GetResource<VCNMaterial>( retValue[i]->GetName() ), 
      "Material was already created" );

    // Add it
    VCNResourceCore::GetInstance()->AddResource( retValue[i]->GetName(), retValue[i] );
  }
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadMultipleMaterialXML( const VCNString& filename , MaterialList& outMaterialsList)
{
  // Open the XML document
  XMLElementPtr pRootElem = LoadDocumentRoot( filename );
  VCN_ASSERT( pRootElem!=NULL && "Could not load XML Material!" );

  // Check if we have multiple materials in the file.
  CComBSTR rootTagName;
  pRootElem->get_tagName( &rootTagName );
  if ( rootTagName == kMultipleMaterialsNode )
  {
    XMLNodeListPtr elementNodes = 0;
    pRootElem->selectNodes( (VCNTChar*)kMaterialNode, &elementNodes );
    VCNLong elementNodeCount = 0;
    elementNodes->get_length( &elementNodeCount );
    VCN_ASSERT( elementNodeCount>0 && "No nodes found while loading XML Material!" );

    for( VCNLong i=0; i<elementNodeCount; i++ )
    {
      // Get the child node
      XMLNodePtr xmlChildNode = 0;
      elementNodes->get_item( i, &xmlChildNode );

      // Load individual materials
      outMaterialsList.push_back( LoadMaterialXML(xmlChildNode) );
    }
  }
  else
  {
    // We only have one material. Load it!
    outMaterialsList.push_back( LoadMaterialXML( pRootElem ) );
  }

  // Free up the doc memory
  ReleaseDocument();
}

//-------------------------------------------------------------
VCNMaterial*  VCNMaterialLoader::LoadMaterialXML( XMLNodePtr rootNode )
{
  // Create the new mesh to be filled!
  VCNMaterial* newMaterial = new VCNMaterial;

  XMLNodePtr node;
  VCNFloat tmpFloat;

  // Load the base properties
  LoadResourceBaseProperties( rootNode, newMaterial );

  // Load the ambient color
  if ( SUCCEEDED(rootNode->selectSingleNode( (VCNTChar*)kNodeMaterialAmbientColor, &node )) )
  {
    newMaterial->SetAmbientColor( LoadMaterialColorXML(node) );
  }

  // Load the diffuse color
  rootNode->selectSingleNode( (VCNTChar*)kNodeMaterialDiffuseColor, &node );
  if ( node )
  {
    newMaterial->SetDiffuseColor( LoadMaterialColorXML(node) );
  }

  // Load the specular color
  rootNode->selectSingleNode( (VCNTChar*)kNodeMaterialSpecularColor, &node );
  if ( node )
  {
    newMaterial->SetSpecularColor( LoadMaterialColorXML(node) );
    if ( GetAttributeFloat( node, kAttrMaterialPower, tmpFloat ) )
    {
      newMaterial->SetSpecularPower( tmpFloat );
    }
  }

  // Load in the effect node
  rootNode->selectSingleNode( (VCNTChar*)kNodeMaterialEffect, &node );
  LoadMaterialEffectNode( node, newMaterial->GetEffectParamSet() );

  return newMaterial;
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadMaterialEffectNode( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // If we don't have effects, bail.
  if( node == NULL )
    return;

  // Start by loading up the effect's type
  VCNString effectName;
  GetAttributeString( node, kAttrMaterialEffectName, effectName );
  effectParams.SetEffectID( effectName );

  // Load up every kind of effect param we have
  LoadTextureParams( node, effectParams );
  LoadCubeTextureParams( node, effectParams );
  LoadStringParams( node, effectParams );
  LoadFloatParams( node, effectParams );
  LoadColorParams( node, effectParams );
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadColorParams( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // Load up the texture params
  XMLNodeListPtr colorNodes = 0;
  node->selectNodes( (VCNTChar*)kNodeMaterialEffectColorParam, &colorNodes );
  VCNLong colorNodeCount = 0;
  colorNodes->get_length( &colorNodeCount );
  for( VCNLong i=0; i<colorNodeCount; i++ )
  {
    // Get the param node
    XMLNodePtr colorNode = 0;
    colorNodes->get_item( i, &colorNode );

    // Get the param's name
    VCNString paramName;
    GetAttributeString( colorNode, kAttrMaterialEffectParamName, paramName );

    // Get the texture's name
    VCNColor color;
    GetAttributeFloat( node, kAttrMaterialColorR, color.R );
    GetAttributeFloat( node, kAttrMaterialColorG, color.G );
    GetAttributeFloat( node, kAttrMaterialColorB, color.B );
    GetAttributeFloat( node, kAttrMaterialColorA, color.A );

    // Add the resource
    effectParams.AddColorParam( VCNStringHash(paramName.c_str()), color );
  }
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadTextureParams( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // Load up the texture params
  XMLNodeListPtr textureNodes = 0;
  node->selectNodes( (VCNTChar*)kNodeMaterialEffectTextureParam, &textureNodes );
  VCNLong textureNodeCount = 0;
  textureNodes->get_length( &textureNodeCount );
  for( VCNLong i=0; i<textureNodeCount; i++ )
  {
    // Get the param node
    XMLNodePtr textureNode = 0;
    textureNodes->get_item( i, &textureNode );

    // Get the param's name
    VCNString paramName;
    GetAttributeString( textureNode, kAttrMaterialEffectParamName, paramName );

    // Get the texture's name
    VCNString textureName;
    GetAttributeString( textureNode, kAttrMaterialEffectParamValue, textureName );

    // Load the texture
    VCNResID texID = LoadMaterialTexture( textureName );

    // Add the resource
    effectParams.AddResource( VCNStringHash(paramName.c_str()), texID );
  }
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadCubeTextureParams( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // Load up the texture params
  XMLNodeListPtr textureNodes = 0;
  node->selectNodes( (VCNTChar*)kNodeMaterialEffectTextureCubeParam, &textureNodes );
  VCNLong textureNodeCount = 0;
  textureNodes->get_length( &textureNodeCount );
  for( VCNLong i=0; i<textureNodeCount; i++ )
  {
    // Get the param node
    XMLNodePtr textureNode = 0;
    textureNodes->get_item( i, &textureNode );

    // Get the param's name
    VCNString paramName;
    GetAttributeString( textureNode, kAttrMaterialEffectParamName, paramName );

    // Get the texture's name
    VCNString textureName;
    GetAttributeString( textureNode, kAttrMaterialEffectParamValue, textureName );

    // Load the texture
    VCNResID texID = LoadMaterialCubeTexture( textureName );

    // Add the resource
    effectParams.AddResource( VCNStringHash(paramName.c_str()), texID );
  }
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadStringParams( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // Load up the texture params
  XMLNodeListPtr stringNodes = 0;
  node->selectNodes( (VCNTChar*)kNodeMaterialEffectStringParam, &stringNodes );
  VCNLong stringNodeCount = 0;
  stringNodes->get_length( &stringNodeCount );
  for( VCNLong i=0; i<stringNodeCount; i++ )
  {
    // Get the param node
    XMLNodePtr stringNode = 0;
    stringNodes->get_item( i, &stringNode );

    // Get the param's name
    VCNString paramName;
    GetAttributeString( stringNode, kAttrMaterialEffectParamName, paramName );

    // Get the texture's name
    VCNString stringValue;
    GetAttributeString( stringNode, kAttrMaterialEffectParamValue, stringValue );

    // Add the resource
    effectParams.AddString( VCNStringHash(paramName.c_str()), stringValue );
  }
}

//-------------------------------------------------------------
void VCNMaterialLoader::LoadFloatParams( XMLNodePtr node, VCNEffectParamSet& effectParams )
{
  // Load up the texture params
  XMLNodeListPtr floatNodes = 0;
  node->selectNodes( (VCNTChar*)kNodeMaterialEffectFloatParam, &floatNodes );
  VCNLong floatNodeCount = 0;
  floatNodes->get_length( &floatNodeCount );
  for( VCNLong i=0; i<floatNodeCount; i++ )
  {
    // Get the param node
    XMLNodePtr floatNode = 0;
    floatNodes->get_item( i, &floatNode );

    // Get the param's name
    VCNString paramName;
    GetAttributeString( floatNode, kAttrMaterialEffectParamName, paramName );

    // Get the texture's name
    VCNFloat floatValue;
    GetAttributeFloat( floatNode, kAttrMaterialEffectParamValue, floatValue );

    // Add the resource
    effectParams.AddFloat( VCNStringHash(paramName.c_str()), floatValue );
  }
}


//-------------------------------------------------------------
VCNColor VCNMaterialLoader::LoadMaterialColorXML( XMLNodePtr node )
{
  VCNColor color;

  if( node != NULL )
  {
    GetAttributeFloat( node, kAttrMaterialColorR, color.R );
    GetAttributeFloat( node, kAttrMaterialColorG, color.G );
    GetAttributeFloat( node, kAttrMaterialColorB, color.B );
    GetAttributeFloat( node, kAttrMaterialColorA, color.A );
  }

  return color;
}

//-------------------------------------------------------------
VCNResID VCNMaterialLoader::LoadMaterialTexture( const VCNString& filename )
{
  // Pessimistic approach
  VCNResID texID = kInvalidResID;

  // We need to actually have a filename
  if( filename != VCNTXT("") )
  {
    // Lets see if it's loaded already
    texID = VCNResourceCore::GetInstance()->GetResourceID( filename );

    // If it wasn't loaded, then lets load it!
    if( texID == kInvalidResID )
    {
      // Send a request out so that the texture be loaded
      VCNMaterialCore::GetInstance()->LoadTexture( filename );

      // Lets try fetching it again
      texID = VCNResourceCore::GetInstance()->GetResourceID( filename );
      VCN_ASSERT_MSG( texID != kInvalidResID, _T("Texture [%s] not found!"),  filename.c_str() );
    }
  }

  return texID;
}

//-------------------------------------------------------------
VCNResID VCNMaterialLoader::LoadMaterialCubeTexture( const VCNString& filename )
{
  // Pessimistic approach
  VCNResID texID = kInvalidResID;

  // We need to actually have a filename
  if( filename != VCNTXT("") )
  {
    // Lets see if it's loaded already
    texID = VCNResourceCore::GetInstance()->GetResourceID( filename );

    // If it wasn't loaded, then lets load it!
    if( texID == kInvalidResID )
    {
      // Send a request out so that the texture be loaded
      VCNMaterialCore::GetInstance()->CreateCubeTexture( filename );

      // Lets try fetching it again
      texID = VCNResourceCore::GetInstance()->GetResourceID( filename );
      VCN_ASSERT( texID != kInvalidResID && "Cube Texture not found!" );
    }
  }

  return texID;
}
