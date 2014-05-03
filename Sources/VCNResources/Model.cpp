///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Model.h"

#include "VCNNodes/NodeCore.h"
#include "VCNNodes/Node.h"

VCN_TYPE( VCNModel, VCNResource ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNModel::VCNModel( VCNNode* modelRoot ) : VCNResource()
,mModelRoot(modelRoot)
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNModel::~VCNModel()
{
}


//-------------------------------------------------------------
/// Creates a renderable instance of this model for the scene
/// graph.
//-------------------------------------------------------------
VCNNode* VCNModel::CreateInstance()
{
  VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();

  // Copy the tree
  VCNNode* modelInstance = nodeCore->CopyTree( mModelRoot->GetNodeID() );
  VCNString instanceName = StringBuilder() << VCNTXT("Instance_") << GetName();
  modelInstance->SetTag( instanceName );

  // Return
  return modelInstance;
}
