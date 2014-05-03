///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game Scene Implementation
///

#include "Precompiled.h"
#include "MediaManager.h"

#include "SplashScreen.h"
#include "GameSceneConstants.h"

// Engine includes
#include "VCNCore/System.h"
#include "VCNImporter/AnimLoader.h"
#include "VCNImporter/MaterialLoader.h"
#include "VCNImporter/MeshLoader.h"
#include "VCNImporter/ModelLoader.h"
#include "VCNNodes/Node.h"
#include "VCNResources/Model.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/StringUtils.h"

VCN_TYPE( MediaManager, VCNCore<MediaManager> ) ;

extern HWND GLOBAL_WINDOW_HANDLE;

static int sNextID = 1;

////////////////////////////////////////////////////////////////////////
///
/// @fn GameScene::GameScene()
///
/// Constructor for the GameScene class.
///
/// @return GameScene
///
////////////////////////////////////////////////////////////////////////
MediaManager::MediaManager()
: mMaterialsPath(VCNTXT(".\\Materials"))
, mModelsPath(VCNTXT(".\\Models"))
, mMeshesPath(VCNTXT(".\\Meshes"))
, mAnimationsPath(VCNTXT(".\\Animations"))
{
}

////////////////////////////////////////////////////////////////////////
///
/// Destructor for the GameScene class.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
MediaManager::~MediaManager()
{  
}

///////////////////////////////////////////////////////////////////////
VCNBool MediaManager::Initialize()
{
  // Load game/scene data
  ShowLoading( VCNTXT("Loading scene...") );
  LoadScene();

  return BaseCore::Initialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool MediaManager::Uninitialize()
{
  return BaseCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool MediaManager::Process(const float elapsedTime)
{
  return BaseCore::Process( elapsedTime );
}

////////////////////////////////////////////////////////////////////////
///
/// Function that will load all the materials, models, meshes and objects instances
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void MediaManager::LoadScene()
{
  LoadMaterials();
  LoadMeshes();
  LoadModels();
  LoadAnimations();
}


////////////////////////////////////////////////////////////////////////
///
/// Function that will load all the materials found in the Materials folder
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void MediaManager::LoadMaterials()
{
  VCNSystem::GetInstance()->FindFilesRecursively( mMaterialsPath.c_str(), VCNTXT("*.MAT.xml"), [](const VCNTChar* path)
  {
    VCNString materialFileName = path;
    VCNString loadingStr = VCNTXT("Loading materials [") + materialFileName + VCNTXT("]...");
    ShowLoading( loadingStr );

    VCNMaterialLoader::LoadMaterial( materialFileName );
  });
}


////////////////////////////////////////////////////////////////////////
///
/// Function that will load all the animations found in the Animations folder
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void MediaManager::LoadAnimations()
{
  VCNSystem::GetInstance()->FindFilesRecursively( mAnimationsPath.c_str(), VCNTXT("*.ANM.xml"), [&](const VCNTChar* path)
  {
    VCNString animationFileName = path;
    VCNString loadingStr = VCNTXT("Loading animations [") + animationFileName + VCNTXT("]...");
    ShowLoading( loadingStr );

    VCNAnimLoader::LoadAnim( animationFileName );
  });
}

////////////////////////////////////////////////////////////////////////
///
/// Function that will load all the meshes found in the Meshes folder
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void MediaManager::LoadMeshes()
{
  auto meshLoadCB = [](const VCNTChar* path)
  {
    const VCNString meshesFileName = path;
    VCNString loadingStr = VCNTXT("Loading meshes [") + meshesFileName + VCNTXT("]...");
    ShowLoading( loadingStr );

    VCNMeshLoader::LoadMesh( meshesFileName );
  };

  VCNSystem::GetInstance()->FindFilesRecursively( mMeshesPath.c_str(), VCNTXT("*.MSH.xml"), meshLoadCB);
  VCNSystem::GetInstance()->FindFilesRecursively( mMeshesPath.c_str(), VCNTXT("*.x"), meshLoadCB);
}

////////////////////////////////////////////////////////////////////////
///
/// Function that will load all the models found in the Models folder
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void MediaManager::LoadModels()
{
  auto loaderCB = [&](const VCNTChar* path)
  {
    // Returns the full path to a file in one of the Model folders
    const VCNString modelFullPath      = path;
    const VCNString modelShortPath     = PathFindFileName( path );
    
    // Get extension
    const size_t extentionPos          = modelShortPath.find_first_of( '.' );
    const VCNString modelName          = modelShortPath.substr( 0, extentionPos );
    
    const VCNString loadingStr = VCNTXT("Loading models [") + modelName + VCNTXT("]...");
    ShowLoading( loadingStr );

    VCNModel* newModel = VCNModelLoader::LoadModel( modelFullPath );
    mModelsMap[modelName] = newModel;
  };

  VCNSystem::GetInstance()->FindFilesRecursively( mModelsPath.c_str(), VCNTXT("*.x"), loaderCB );
  VCNSystem::GetInstance()->FindFilesRecursively( mModelsPath.c_str(), VCNTXT("*.MDL.xml"), loaderCB );
}

////////////////////////////////////////////////////////////////////////
///
/// Function that will create an instance of a certain model
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
VCNNode* MediaManager::CreateInstance( const VCNString& InstanceBaseModelName)
{
  VCNModel* model = mModelsMap[InstanceBaseModelName];

  VCN_ASSERT_MSG( model, VCNTXT("Requested model [%s] doesn't exists"), InstanceBaseModelName.c_str() );

  return model->CreateInstance();
}


///////////////////////////////////////////////////////////////////////
///
/// Function that will delete an instance of a certain model by its nodeId
///
/// @param nodeID  [IN] delete a node
///
void MediaManager::DeleteInstance( const VCNNodeID& nodeID)
{
  VCNNodeCore::GetInstance()->DestroyNode(nodeID);
}


////////////////////////////////////////////////////////////////////////
///
/// Returns the number of base models that can be instantiated
///
////////////////////////////////////////////////////////////////////////
const int MediaManager::GetModelCount() const
{
  return mModelsMap.size();
}

////////////////////////////////////////////////////////////////////////
///
/// Returns all base model names
///
////////////////////////////////////////////////////////////////////////
const std::vector<VCNString> MediaManager::GetModelNames() const
{
  std::vector<VCNString> keyVector;
  
  for (std::map<VCNString, VCNModel*>::const_iterator i = mModelsMap.begin(); i != mModelsMap.end(); ++i)
  {
    VCNString key = i->first;
    keyVector.push_back( key );
  }

  return keyVector;
}

////////////////////////////////////////////////////////////////////////
///
/// Returns the scale transform that needs to be applied to an instance of a base model
///
////////////////////////////////////////////////////////////////////////
const Vector3 MediaManager::GetModelScaleTransform(const VCNString& InstanceBaseModelName) const
{
  Vector3 scale(1,1,1);
  std::map<VCNString, Vector3>::const_iterator fItr = mModelsScaleTransformsMap.find( InstanceBaseModelName );
  if ( fItr != mModelsScaleTransformsMap.end() )
  {
    scale = fItr->second;
    if ( scale.IsNull() )
      scale = Vector3(1, 1, 1);
  }
  return scale;
}

////////////////////////////////////////////////////////////////////////
///
/// Returns the rotation transform that needs to be applied to an instance of a base model
///
////////////////////////////////////////////////////////////////////////
const VCNString MediaManager::GetModelProperty(const VCNString& InstanceBaseModelName) const
{
  auto fItr = mModelsPropertyMap.find( InstanceBaseModelName );
  if ( fItr != mModelsPropertyMap.end() )
  {
    return fItr->second;
  }

  return VCNTXT( "" );
}

////////////////////////////////////////////////////////////////////////
///
/// Creates a new xml node to be added to the file.
///
////////////////////////////////////////////////////////////////////////
XMLElementPtr MediaManager::CreateNewNode( XMLDocPtr xmlDoc, VCNNode* instanceToSave )
{
  // Create the new node
  XMLElementPtr newNode = NULL;
  xmlDoc->createElement(_bstr_t("Instance"), &newNode);

  // Get all the values needed
  std::string instanceOf = VCN_W2A(instanceToSave->GetTag());

  VCNFloat instancePositionX = instanceToSave->GetLocalTranslation().x;
  VCNFloat instancePositionY = instanceToSave->GetLocalTranslation().y;
  VCNFloat instancePositionZ = instanceToSave->GetLocalTranslation().z;

  VCNFloat instanceScaleX = instanceToSave->GetLocalScale().x;
  VCNFloat instanceScaleY = instanceToSave->GetLocalScale().y;
  VCNFloat instanceScaleZ = instanceToSave->GetLocalScale().z;

  VCNFloat instancePitch, instanceYaw, instanceRoll;

  Matrix4 instanceRotationMatrix = instanceToSave->GetLocalRotation();
  VCNQuat instanceQuaternions;
  instanceQuaternions.SetFromMatrix(instanceRotationMatrix);
  instanceQuaternions.GetEulers(&instancePitch, &instanceYaw , &instanceRoll);

  // Set all the attributes to be saved
  std::stringstream ss;
  ss << instanceOf << "_" << sNextID;
  newNode->setAttribute(_bstr_t("name"), _variant_t(ss.str().c_str()));
  newNode->setAttribute(_bstr_t("of"), _variant_t(instanceOf.c_str()));

  newNode->setAttribute(_bstr_t("posx"), _variant_t(instancePositionX));
  newNode->setAttribute(_bstr_t("posy"), _variant_t(instancePositionY));
  newNode->setAttribute(_bstr_t("posz"), _variant_t(instancePositionZ));

  newNode->setAttribute(_bstr_t("pitch"), _variant_t(instancePitch));
  newNode->setAttribute(_bstr_t("yaw"), _variant_t(instanceYaw));
  newNode->setAttribute(_bstr_t("roll"), _variant_t(instanceRoll));

  newNode->setAttribute(_bstr_t("sclx"), _variant_t(instanceScaleX));
  newNode->setAttribute(_bstr_t("scly"), _variant_t(instanceScaleY));
  newNode->setAttribute(_bstr_t("sclz"), _variant_t(instanceScaleZ));

  // Check if the object is dynamic
  if(instanceToSave->HasProperty(VCNTXT("Dynamic")))
  {
    // Create the new property node
    XMLElementPtr newProperty = NULL;
    xmlDoc->createElement(_bstr_t("Property"), &newProperty);

    // Set the value of the dynamic property
    newProperty->setAttribute(_bstr_t("name"), _variant_t("Dynamic"));
    
    // Add the property to the new node ()
    newNode->appendChild(newProperty, NULL);
  }

  // Check if the object is animated
  if(instanceToSave->HasProperty(VCNTXT("Animated")))
  {
    // Create the new property node
    XMLElementPtr newProperty = NULL;
    xmlDoc->createElement(_bstr_t("Property"), &newProperty);

    // Set the value of the dynamic property
    newProperty->setAttribute(_bstr_t("name"), _variant_t("Animated"));
    
    // Add the property to the new node ()
    newNode->appendChild(newProperty, NULL);
  }

  // Return the new node
  return newNode;
}

//////////////////////////////////////////////////////////////////////////
const VCNBool MediaManager::HasPropertyNode(const VCNString& propName, XMLNodePtr node, XMLNodePtr& retPropNode) const
{
  // Save the Dynamic property
  // <Property name="Dynamic"></Property>
  XMLNodePtr pPropNode = 0, pNextPropNode = 0;
  XMLNodeListPtr pPropertiyNodes = 0;
  node->get_childNodes(&pPropertiyNodes);
  if (pPropertiyNodes)
  {
    // We have some properties
    pPropertiyNodes->get_item(0, &pPropNode);
    while (pPropNode)
    {
      CComBSTR nodeName;
      pPropNode->get_nodeName(&nodeName);

      if ( nodeName == _T("Property") )
      {
        VCNString propValue;
        GetAttributeString(pPropNode, VCNTXT("name"), propValue);

        if ( propValue == propName.c_str() )
        {
          retPropNode = pPropNode;
          return true;
        }
      }

      pPropNode->get_nextSibling(&pNextPropNode);
      pPropNode = pNextPropNode;
    }
  }

  return false;
}
