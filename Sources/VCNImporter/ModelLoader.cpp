///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna model loader implementation.
///        Supports:
///              - .MDL.xml (Custom Vicuna Model files)
///              - .x       (DirectX files)
///

#include "Precompiled.h"
#include "ModelLoader.h"

// Project includes
#include "ModelLoaderConstants.h"

// Engine includes
#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/Model.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/StringUtils.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNNodes/ComponentFactory.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/DirectionalLight.h"
#include "VCNRenderer/PointLight.h"
#include "VCNRenderer/SpotLight.h"


///////////////////////////////////////////////////////////////////////
///
/// Loads a model from a given file. First we check that we support the
/// file format, and then we load it.
/// 
/// @param filename  [IN] file name to load
///
/// @return the instantiated model
///
VCNModel* VCNModelLoader::LoadModel(const VCNString& filename)
{
  // Make sure we at least have a char and an extension
  VCN_ASSERT( filename.length()>7 && "Incorrect filename!" );

  VCNModel* newModel = NULL;

  // Is it an XML format or a binary format?
  if( filename.rfind(kXMLModelExtension) != VCNString::npos )
  {
    newModel = LoadModelXML( filename );
  }
  else if( filename.rfind(kXModelExtension) != VCNString::npos )
  {
    newModel = LoadModelDirectX( filename );
  }
  else
  {
    VCN_ASSERT( false && "Unsupported model extension!" );
  }

  // return the mesh pointer!
  return newModel;
}


///////////////////////////////////////////////////////////////////////
VCNModel* VCNModelLoader::LoadModelXML( const VCNString& filename )
{
  VCNModel* newModel = NULL;

  // Open the XML document
  XMLElementPtr pRootElem = LoadDocumentRoot( filename );
  VCN_ASSERT( pRootElem!=NULL && "Could not load XML Model!" );

  // Go through all the VCNModel nodes
  XMLNodeListPtr elementNodes = 0;
  pRootElem->selectNodes( (VCNTChar*)kModelResourceNode, &elementNodes );
  VCNLong elementNodeCount = 0;
  elementNodes->get_length( &elementNodeCount );
  for( VCNLong i=0; i<elementNodeCount; i++ )
  {
    // Get the element's node
    XMLNodePtr elementNode = 0;
    elementNodes->get_item( i, &elementNode );

    // Get a list of children
    XMLNodeListPtr xmlChildNodes = 0;
    elementNode->get_childNodes( &xmlChildNodes );
    VCNLong childNodeLength = 0;
    xmlChildNodes->get_length( &childNodeLength );

    // Create the root node for our model
    VCNNode* rootNode = 0;

    if ( childNodeLength > 1 )
    {
      // Create helper if more than one node
      rootNode = VCNNodeCore::GetInstance()->CreateNode<VCNNode>();
    }

    // Go through all the children attached to the root
    for( VCNLong i=0; i<childNodeLength; i++ )
    {
      // Get the child node
      XMLNodePtr xmlChildNode = 0;
      xmlChildNodes->get_item( i, &xmlChildNode );

      // Load the model up with all of this mesh's content
      VCNNode* childNode = LoadModelElementXML( xmlChildNode );
      if( childNode )
      {
        if ( childNodeLength == 1 )
        {
          rootNode = childNode;
        }
        else
        {
          rootNode->AttachChild( childNode->GetNodeID() );
        }
      }
    }

    // Create the new mesh to be filled!
    newModel = new VCNModel( rootNode );

    // Load the base properties
    LoadResourceBaseProperties( elementNode, newModel );

    // Make sure we aren't loading it twice
    VCN_ASSERT( !VCNResourceCore::GetInstance()->GetResource<VCNModel>( newModel->GetName() ) );

    // Add this model to the resource manager
    VCNResourceCore::GetInstance()->AddResource( newModel->GetName(), newModel );
  }

  // Free up the doc memory
  ReleaseDocument();

  // Return the last model we created
  return newModel;
}


///////////////////////////////////////////////////////////////////////
VCNNode* VCNModelLoader::LoadModelElementXML( XMLNodePtr elementNode )
{
  
  // Get the node type
  VCNString nodeType;
  VCNBool success = GetAttributeString(elementNode, kAttrModelNodeType, nodeType);
  VCN_ASSERT( success && "ERROR LOADING NODE FROM MODEL ELEMENT!" );

  // Get the node's name
  VCNString nodeName;
  GetAttributeString(elementNode, kAttrModelNodeName, nodeName);

  // Create the node
  VCNNode* newNode = NULL;
  if( nodeType == VCNTXT("Render") )
  {
    // Create the new node
    newNode = VCNNodeCore::GetInstance()->CreateNode<VCNRenderNode>();

    // Render nodes have a mesh...
    VCNString meshName;
    GetAttributeString(elementNode, kAttrModelNodeResource, meshName);
    VCNResID meshID = VCNResourceCore::GetInstance()->GetResourceID(meshName);
    VCN_ASSERT_MSG( meshID != kInvalidResID, _T("Mesh from Model is not loaded [%s]!"), meshName.c_str() );
    ((VCNRenderNode*)newNode)->SetMeshID( meshID );

    // Check if the loaded mesh (i.e .x mesh files) proposes a default material first
    VCNResID materialID = kInvalidResID;
    VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>( meshID );
    if ( mesh && mesh->GetMaterialID() != kInvalidResID )
    {
      materialID = mesh->GetMaterialID();
    }
    else
    {
      // Take the material proposed by the model itself
      VCNString materialName;
      GetAttributeString(elementNode, kAttrModelNodeMaterial, materialName);
      materialID = VCNResourceCore::GetInstance()->GetResourceID(materialName);
      
      VCN_ASSERT_MSG( materialID != kInvalidResID, _T("Material [%s] from Model is not loaded!"), materialName.c_str() );
    }

    static_cast<VCNRenderNode*>(newNode)->SetMaterialID( materialID );
  }
  else if( nodeType == VCNTXT("Light") )
  {
	  VCNString lightType;
	  GetAttributeString(elementNode, VCNTXT("LightType"), lightType);

	  VCNColor diffuse;
	  GetAttributeFloat( elementNode, VCNTXT("diffuseR"), diffuse.r );
	  GetAttributeFloat( elementNode, VCNTXT("diffuseG"), diffuse.g );
	  GetAttributeFloat( elementNode, VCNTXT("diffuseB"), diffuse.b );
	  GetAttributeFloat( elementNode, VCNTXT("diffuseA"), diffuse.a );

	  VCNColor specular;
	  GetAttributeFloat( elementNode, VCNTXT("specularR"), specular.r );
	  GetAttributeFloat( elementNode, VCNTXT("specularG"), specular.g );
	  GetAttributeFloat( elementNode, VCNTXT("specularB"), specular.b );
	  GetAttributeFloat( elementNode, VCNTXT("specularA"), specular.a );

	  bool isOn = false;
	  GetAttributeBool( elementNode, VCNTXT("IsOn"), isOn);

	  VCNLight* lightPtr = NULL;
	  if (lightType == VCNTXT("PointLight"))
	  {
		  float range = 0.0f;
		  float maxRange = 0.0f;
		  float constantAttenuation = 0.0f;
		  float linearAttenuation = 0.0f;
		  float quadAttenuation = 0.0f;
		  GetAttributeFloat( elementNode, VCNTXT("Range"), range );
		  GetAttributeFloat( elementNode, VCNTXT("MaxRange"), maxRange );
		  GetAttributeFloat( elementNode, VCNTXT("ConstantAttenuation"), constantAttenuation );
		  GetAttributeFloat( elementNode, VCNTXT("LinearAttenuation"), linearAttenuation );
		  GetAttributeFloat( elementNode, VCNTXT("QuadAttenuation"), quadAttenuation );

		  VCNPointLight* light = VCNLightingCore::GetInstance()->CreateLight( LT_OMNI, nodeName )->Cast<VCNPointLight>();

		  light->SetRange(range);
		  light->SetMaxRange(maxRange);
		  light->SetConstantAttenuation(constantAttenuation);
		  light->SetLinearAttenuation(linearAttenuation);
		  light->SetQuadAttenuation(quadAttenuation);



		  lightPtr = light;
	  }
	  else if (lightType == VCNTXT("DirectionalLight"))
	  {
		  Vector3 direction;
		  GetAttributeFloat( elementNode, VCNTXT("directionX"), direction.x );
		  GetAttributeFloat( elementNode, VCNTXT("directionY"), direction.y );
		  GetAttributeFloat( elementNode, VCNTXT("directionZ"), direction.z );

		  VCNDirectionalLight* light = VCNLightingCore::GetInstance()->CreateLight( LT_DIRECTIONAL, nodeName )->Cast<VCNDirectionalLight>();
		  
		  light->SetDirection(direction);

		  lightPtr = light;
	  }
	  else if (lightType == VCNTXT("SpotLight"))
	  {
		  float range = 0.0f;
		  float maxRange = 0.0f;
		  float constantAttenuation = 0.0f;
		  float linearAttenuation = 0.0f;
		  float quadAttenuation = 0.0f;
		  float phi = 0.0f;
		  float theta = 0.0f;
		  Vector3 direction;
		  GetAttributeFloat( elementNode, VCNTXT("Range"), range );
		  GetAttributeFloat( elementNode, VCNTXT("MaxRange"), maxRange );
		  GetAttributeFloat( elementNode, VCNTXT("ConstantAttenuation"), constantAttenuation );
		  GetAttributeFloat( elementNode, VCNTXT("LinearAttenuation"), linearAttenuation );
		  GetAttributeFloat( elementNode, VCNTXT("QuadAttenuation"), quadAttenuation );
		  GetAttributeFloat( elementNode, VCNTXT("Phi"), phi );
		  GetAttributeFloat( elementNode, VCNTXT("Theta"), theta );
		  GetAttributeFloat( elementNode, VCNTXT("directionX"), direction.x );
		  GetAttributeFloat( elementNode, VCNTXT("directionY"), direction.y );
		  GetAttributeFloat( elementNode, VCNTXT("directionZ"), direction.z );

		  VCNSpotLight* light = VCNLightingCore::GetInstance()->CreateLight( LT_SPOT, nodeName )->Cast<VCNSpotLight>();

		  light->SetFalloff(range);
		  light->SetMaxRange(maxRange);
		  light->SetConstantAttenuation(constantAttenuation);
		  light->SetLinearAttenuation(linearAttenuation);
		  light->SetQuadAttenuation(quadAttenuation);
		  light->SetPhi(phi);
		  light->SetTheta(theta);
		  light->SetDirection(direction);

		  lightPtr = light;
	  }

	  if (lightPtr)
	  {
		  if (!isOn) { lightPtr->TurnOff(); }
		  lightPtr->SetDiffuse(diffuse);
		  lightPtr->SetSpecular(specular);

		  newNode = lightPtr;
	  }
	  
  }
  else if( nodeType == VCNTXT("Helper") )
  {
    // Create the new node
    newNode = VCNNodeCore::GetInstance()->CreateNode<VCNNode>();
  }

  VCN_ASSERT_MSG( newNode, "ERROR! UNKNOWN NODE TYPE!" );

  
  newNode->SetTag( nodeName );
  newNode->AddProperty(VCNTXT("BaseName"), new VCNString(nodeName));
  VCNNodeCore::GetInstance()->PushName( nodeName, newNode->GetNodeID() );

  // Lets get the translation
  Vector3 position;
  GetAttributeFloat( elementNode, kAttrModelNodePosX, position.x );
  GetAttributeFloat( elementNode, kAttrModelNodePosY, position.y );
  GetAttributeFloat( elementNode, kAttrModelNodePosZ, position.z );
  newNode->SetTranslation( position );

  // Lets get the rotation
  VCNQuat quat;
  GetAttributeFloat( elementNode, kAttrModelNodeRotX, quat.x );
  GetAttributeFloat( elementNode, kAttrModelNodeRotY, quat.y );
  GetAttributeFloat( elementNode, kAttrModelNodeRotZ, quat.z );
  GetAttributeFloat( elementNode, kAttrModelNodeRotW, quat.w );
  Matrix4 rotation;
  quat.GetMatrix( rotation );
  newNode->SetRotation( rotation );

  // Lets get the scale
  Vector3 scale;
  GetAttributeFloat( elementNode, kAttrModelNodeScaleX, scale.x );
  GetAttributeFloat( elementNode, kAttrModelNodeScaleY, scale.y );
  GetAttributeFloat( elementNode, kAttrModelNodeScaleZ, scale.z );
  newNode->SetScale( scale );

  // Get a list of children
  XMLNodeListPtr xmlChildNodes;
  elementNode->get_childNodes( &xmlChildNodes );
  VCNLong childNodesLength = 0;
  xmlChildNodes->get_length( &childNodesLength );

  // Go through all the children attached to the root
  for( VCNLong i=0; i<childNodesLength; i++ )
  {
    // Get the child node
    XMLNodePtr xmlChildNode = 0;
    xmlChildNodes->get_item( i, &xmlChildNode );

	// Get the xml node type
	VCNString xmlNodeType;
	VCNBool successNodeName = GetNodeTagName(xmlChildNode, xmlNodeType);

	if (successNodeName && xmlNodeType == L"Components")
	{
		// Load all components
		XMLNodeListPtr xmlComponentChildNodes;
		xmlChildNode->get_childNodes( &xmlComponentChildNodes );
		VCNLong childComponentNodesLength = 0;
		xmlComponentChildNodes->get_length( &childComponentNodesLength );
		for( VCNLong i=0; i<childComponentNodesLength; i++ )
		{
			XMLNodePtr xmlComponentNode = 0;
			xmlComponentChildNodes->get_item( i, &xmlComponentNode );

			// The tag name is the name of the component and the attributes are attributes in the xml
			VCNString xmlComponentNodeName;
			VCNBool successComponentNodeName = GetNodeTagName(xmlComponentNode, xmlComponentNodeName);
			if (successComponentNodeName)
			{
				VCNIComponent* newComponent = ComponentFactory::CreateNewComponent(xmlComponentNodeName);
				if (newComponent != NULL)
				{
					newNode->AddComponent(newComponent);
					VCNIComponent::Parameters params;
					XMLNodeMapPtr componentsAttributes;
					HRESULT res = xmlComponentNode->get_attributes(&componentsAttributes);
					if (res == S_OK)
					{
						long componentValuesCount;
						componentsAttributes->get_length(&componentValuesCount);
						for (long cIdx = 0; cIdx < componentValuesCount; cIdx++)
						{
							XMLNodePtr componentAttributeData;
							if (componentsAttributes->get_item(cIdx, &componentAttributeData) == S_OK)
							{
								VCNString componentAttributeName;
								VCNBool successComponentAttributeName = GetNodeName(componentAttributeData, componentAttributeName);
								if (successComponentAttributeName)
								{
									BSTR componentAttributeText;
									if (componentAttributeData->get_text(&componentAttributeText) == S_OK)
									{
										// We now have all that we need to add the values to the new component
										newComponent->SetAttribute(componentAttributeName, componentAttributeText);
									}
								}
							}
						}
					}
				}
			}
		}
		int bob = 0;
	}
	else
	{
		// Load the model up with all of this mesh's content
		VCNNode* childNode = LoadModelElementXML( xmlChildNode );
		if( childNode )
		{
			newNode->AttachChild( childNode->GetNodeID() );
			childNode->SetParent( newNode->GetNodeID() );
		}
	 }
  }

  return newNode;
}


///////////////////////////////////////////////////////////////////////
VCNModel* VCNModelLoader::LoadModelDirectX(const VCNString& filename)
{
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();
  return renderer->CreateModel( filename );
}
