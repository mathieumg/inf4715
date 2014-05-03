///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX concrete implementation
///

#include "Precompiled.h"
#include "PhysxCoreImpl.h"

#include "PhysxUtils.h"
#include "PhysxStream.h"

#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNResources/Cache.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Chrono.h"
#include "characterkinematic/PxControllerManager.h"
#include "characterkinematic/PxCapsuleController.h"
#include "PhysXCollisionCallback.h"

#define CONTACT_OFFSET			0.01f
//	#define CONTACT_OFFSET			0.1f
//	#define STEP_OFFSET				0.01f
#define STEP_OFFSET				0.05f
//	#define STEP_OFFSET				0.1f
//	#define STEP_OFFSET				0.2f

//	#define SLOPE_LIMIT				0.5f
//	#define SLOPE_LIMIT				0.2f
#define SLOPE_LIMIT				0.0f
//	#define INVISIBLE_WALLS_HEIGHT	6.0f
#define INVISIBLE_WALLS_HEIGHT	0.0f
//	#define MAX_JUMP_HEIGHT			4.0f
#define MAX_JUMP_HEIGHT			0.0f

static const float gScaleFactor = 1.5f;
static const float gStandingSize = 1.0f * gScaleFactor;
static const float gCrouchingSize = 0.25f * gScaleFactor;
static const float gControllerRadius = 0.3f * gScaleFactor;

///////////////////////////////////////////////////////////////////////
VCNPhysxCoreImpl::VCNPhysxCoreImpl()
  : mFoundation( 0 )
  , mPhysics( 0 )
  , mCooking( 0 )
  , mScene( 0 )
  , mCpuDispatcher( 0 )
  , mCudaContextManager( 0 )
  , mDefaultMaterial( 0 )
  , mAccumulator( 0.0f )
  , mStepSize( 1.0f / 60.0f )
{
  mDefaultFilterShader = PxDefaultSimulationFilterShader;
}


///////////////////////////////////////////////////////////////////////
VCNPhysxCoreImpl::~VCNPhysxCoreImpl()
{
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCoreImpl::Initialize()
{
  // Initialize the foundation
  //
  mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
  VCN_ASSERT_MSG( mFoundation, VCNTXT("PxCreateFoundation failed!") );

  // Now create the top-level PxPhysics object
  //
  mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale() );
  VCN_ASSERT_MSG( mPhysics, VCNTXT("PxCreatePhysics failed!") );
  
#if defined( DEBUG )
  // Connect to the debugger if available
	//
  PxVisualDebuggerExt::createConnection( mPhysics->getPvdConnectionManager(), "127.0.0.1", 5425, 10000 );
#endif

	// Initialize cooker
	//
  mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams());
  VCN_ASSERT_MSG( mCooking, VCNTXT("PxCreateCooking failed!") );

  // Create main scene
  // TODO: call customization game callback, customizeSceneDesc(sceneDesc);
  //
  PxSceneDesc sceneDesc( mPhysics->getTolerancesScale() );
  sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
  if( !sceneDesc.cpuDispatcher )
  {
    // The dispatcher is used to make the actual simulation
    mCpuDispatcher = PxDefaultCpuDispatcherCreate( 1 );
    VCN_ASSERT( mCpuDispatcher );
    sceneDesc.cpuDispatcher = mCpuDispatcher;
  } 
  if( !sceneDesc.filterShader )
  {
    sceneDesc.filterShader = mDefaultFilterShader;
  }
#ifdef PX_WINDOWS
  if( !sceneDesc.gpuDispatcher )
  {
    // create GPU dispatcher
    CudaContextManagerDesc cudaContextManagerDesc;
    mCudaContextManager = createCudaContextManager( *mFoundation, cudaContextManagerDesc, 0 );
    if ( mCudaContextManager )
    {
      sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
    }
  }
#endif

	// Create the main scene
	//
  mScene = mPhysics->createScene( sceneDesc );
  VCN_ASSERT_MSG( mScene, "Failed to create the physical scene!" );

  // Create the main material
  //
  mDefaultMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
  VCN_ASSERT_MSG( mDefaultMaterial, "Failed to create physic material" );
  

  mControllerManager = PxCreateControllerManager(*mFoundation);
  VCN_ASSERT_MSG(mControllerManager, "Failed to create the controller manager");

  return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCoreImpl::Uninitialize()
{
	mDefaultMaterial->release();
	mDefaultMaterial = 0;

	mScene->release();
	mScene = 0;

	// Released in the controller manager
	//delete mPhysxController;
	//mPhysxController = 0;

	mControllerManager->release();
	mControllerManager = 0;

	mCudaContextManager->release();
	mCudaContextManager = 0;

  mCpuDispatcher->release();
  mCpuDispatcher = 0;

  mCooking->release();
  mCooking = 0;

  mPhysics->release();
  mPhysics = 0;

  mFoundation->release();
  mFoundation = 0;

  return true;
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCoreImpl::Process(const float elapsedTime)
{
  mAccumulator += elapsedTime;
  if(mAccumulator < mStepSize)
    return true;

  CHRONO_AVERAGE( "PHX" );

  mAccumulator -= mStepSize;

  // Run the physic simulation
  mScene->simulate(mStepSize);

  // Fetch simulation results
  mScene->fetchResults( true );
  
  return true;
}

///////////////////////////////////////////////////////////////////////
PxActor* const  VCNPhysxCoreImpl::CreateGroundPlane(const Vector4& plane)
{
  VCN_ASSERT( mScene );

  PxRigidStatic* planeActor = PxCreatePlane(*mPhysics, PxPlane( PxVec3(plane.x, plane.y, plane.z), plane.w ), *mDefaultMaterial);
  VCN_ASSERT_MSG( planeActor, "Failed to create plane" );
  mScene->addActor( *planeActor );

  return planeActor;
}



///////////////////////////////////////////////////////////////////////
PxActor* const VCNPhysxCoreImpl::CreateStaticMesh(const VCNResID meshID, const Matrix4& transform)
{
  VCN_ASSERT( meshID != kInvalidResID );

  VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>( meshID );
  VCN_ASSERT( mesh );

  VCNResID vertexCacheID = mesh->GetCacheID(VT_POSITION);
  VCN_ASSERT(kInvalidResID != vertexCacheID);

  VCNResID indexCacheID = mesh->GetFaceCache();
  VCN_ASSERT(kInvalidResID != indexCacheID);

  VCNCache* vertexCache = VCNResourceCore::GetInstance()->GetResource<VCNCache>(vertexCacheID);
  VCN_ASSERT( vertexCache );

  VCNCache* indexCache = VCNResourceCore::GetInstance()->GetResource<VCNCache>(indexCacheID);
  VCN_ASSERT( indexCache );

  // Get vertices
  //
  const VCNUInt numVertices = vertexCache->GetCount();
  Vector3* vertexBufferPointer = 0;
  vertexCache->Lock( (VCNByte**)&vertexBufferPointer );

  // Get indices
  //
  const VCNUInt numTriangles = mesh->GetFaceCount();
  const VCNUInt indexStriding = indexCache->GetStride();
  const VCNUShort* indiceBufferPointer = 0;
  indexCache->Lock( (VCNByte**)&indiceBufferPointer );

  // Create the mesh info
  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count           = numVertices;
  meshDesc.points.stride          = vertexCache->GetStride();
  meshDesc.points.data            = vertexBufferPointer;

  meshDesc.triangles.count        = numTriangles;
  meshDesc.triangles.stride       = indexStriding * 3; // 16 bits
  meshDesc.triangles.data         = indiceBufferPointer;
  meshDesc.flags                  = PxMeshFlag::e16_BIT_INDICES;

  VCN_ASSERT(meshDesc.isValid());

  VCNPhysxMemoryOutputStream writeBuffer;
  bool status = mCooking->cookTriangleMesh(meshDesc, writeBuffer);
  VCN_ASSERT( status );

  // We don't need the buffers anymore
  indexCache->Unlock();
  vertexCache->Unlock();

  VCNPhysxMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
  PxTriangleMesh* triangleMesh = mPhysics->createTriangleMesh( readBuffer );

  // Create the static actor
  PxRigidStatic* staticActor = mPhysics->createRigidStatic( VCNPhysxUtils::ToTransform(transform) );

  PxMeshScale meshScaling;
  meshScaling.scale.x = transform.GetXAxis().Length();
  meshScaling.scale.y = transform.GetYAxis().Length();
  meshScaling.scale.z = transform.GetZAxis().Length();

  // Create the shape
  PxShape* meshShape = staticActor->createShape( PxTriangleMeshGeometry(triangleMesh, meshScaling), *mDefaultMaterial );
  VCN_ASSERT( meshShape );

  // Add the actor to the scene
  mScene->addActor( *staticActor );
  
  // Return it
  return staticActor;
}



///////////////////////////////////////////////////////////////////////
PxActor* const VCNPhysxCoreImpl::CreateSphere(const VCNSphere& sphereDesc)
{
  PxSphereGeometry sphereGeom( sphereDesc.GetRadius() );

  PxRigidDynamic* sphereActor = PxCreateDynamic( 
    *mPhysics, 
    VCNPhysxUtils::ToTransform( sphereDesc ),
    sphereGeom,
    *mDefaultMaterial,
    2.0f /* TODO: be configurable */ );

  // Push sphere to the scene
  mScene->addActor( *sphereActor );

  return sphereActor;    
}



///////////////////////////////////////////////////////////////////////
PxActor* const VCNPhysxCoreImpl::CreateActor(const VCNNode* node)
{
  VCN_ASSERT( node );
  
  // Get node properties for mesh creation
  const bool isDynamic = node->HasProperty( VCNTXT("Dynamic") );

  // Create the physic actor
  PxRigidActor* nodeActor = 0;
  PxTransform pxXform = VCNPhysxUtils::ToTransform( node->GetWorldTransformation() );
  if ( isDynamic )
  {
	PxRigidDynamic* dyn = mPhysics->createRigidDynamic( pxXform );
	if (dyn && node->HasProperty(VCNTXT("Kinematic")))
	{
		dyn->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
	}
    nodeActor = dyn;
  }
  else
  {
    nodeActor = mPhysics->createRigidStatic( pxXform );
  }
  VCN_ASSERT( nodeActor );
  
  // Load render node meshes as shapes
  ShapeList shapes;
  CreateActorShapes(node, isDynamic, shapes);
  for (ShapeList::const_iterator it = shapes.begin(), end = shapes.end(); it != end; ++it)
  {
    PxShape* actorShape = nodeActor->createShape( *it->get(), *mDefaultMaterial );
    VCN_ASSERT( actorShape );
  }

  // Add body to dynamic actors
  if ( isDynamic )
  {
    PxRigidBodyExt::updateMassAndInertia( *static_cast<PxRigidDynamic*>(nodeActor), 1.0f );
  }

  // Add actor to scene
  mScene->addActor( *nodeActor );
  nodeActor->userData = (void*)node;

  return nodeActor;
}

PxController* const VCNPhysxCoreImpl::CreateCharacterController(const VCNNode* node)
{
	VCN_ASSERT(node);

	// Get node properties for mesh creation
	const bool isDynamic = node->HasProperty(VCNTXT("Dynamic"));

	// Create the physic actor
	PxRigidActor* nodeActor = 0;
	PxTransform pxXform = VCNPhysxUtils::ToTransform(node->GetWorldTransformation());
	if (isDynamic)
	{
		nodeActor = mPhysics->createRigidDynamic(pxXform);
        
	}
	else
	{
		nodeActor = mPhysics->createRigidStatic(pxXform);
	}

	//ControlledActorDesc desc;
	//desc.mPosition = PxExtendedVec3(0.0f, 0.0f, 0.0f);
	//desc.mContactOffset = 0.05f;
	//desc.mStepOffset = 0.01;
	//desc.mSlopeLimit = 0.5f;
	//desc.mRadius = 0.5f;
	//desc.mHeight = 2;
	////desc.material = mDefaultMaterial; //THIS IS SET INSIDE THE INIT WITH CREATEMATERIAL
	//desc.mType = PxControllerShapeType::eCAPSULE;


	PxControllerShapeType::Enum	type = PxControllerShapeType::eCAPSULE;
	const PxExtendedVec3& position = PxExtendedVec3(node->GetWorldTranslation().x, node->GetWorldTranslation().y + 1.5f, node->GetWorldTranslation().z);
	float slopeLimit = 0.5f;
	float contactOffset = 0.05f;
	float stepOffset = 0.01;
	//float invisibleWallHeight = desc.mInvisibleWallHeight;
	//float maxJumpHeight = desc.mMaxJumpHeight;
	float radius = 0.75f;
	float height = 3.85f;
	//float crouchHeight = desc.mCrouchHeight;

	PxControllerDesc* cDesc;
	PxBoxControllerDesc boxDesc;
	PxCapsuleControllerDesc capsuleDesc;

	if (type == PxControllerShapeType::eBOX)
	{
		height *= 0.5f;
		height += radius;
		//crouchHeight *= 0.5f;
		//crouchHeight += radius;
		boxDesc.halfHeight = height;
		boxDesc.halfSideExtent = radius;
		boxDesc.halfForwardExtent = radius;
		cDesc = &boxDesc;
	}
	else
	{
		PX_ASSERT(type == PxControllerShapeType::eCAPSULE);
		capsuleDesc.height = height;
		capsuleDesc.radius = radius;
		cDesc = &capsuleDesc;
	}

	//cDesc->density = desc.mProxyDensity;
	//cDesc->scaleCoeff = desc.mProxyScale;
	// Create a basic default material for now
	cDesc->material = mDefaultMaterial;
	//cDesc->material = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
	cDesc->position = position;
	cDesc->slopeLimit = slopeLimit;
	cDesc->contactOffset = contactOffset;
	cDesc->stepOffset = stepOffset;
	//cDesc->invisibleWallHeight = invisibleWallHeight;
	//cDesc->maxJumpHeight = maxJumpHeight;
	cDesc->callback = new PhysXCollisionCallback();
	//cDesc->behaviorCallback = behaviorCallback;
	//	cDesc->nonWalkableMode		= PxCCTNonWalkableMode::eFORCE_SLIDING;
	//	cDesc->volumeGrowth			= 2.0f;

	//mType = type;
	//mInitialPosition = position;
	//mStandingSize = height;
	//mCrouchingSize = crouchHeight;
	//mControllerRadius = radius;

	PxController* ctrl = static_cast<PxBoxController*>(mControllerManager->createController(*mPhysics, mScene, *cDesc));
	PX_ASSERT(ctrl);

	mPhysxController = new VCNPhysxController(ctrl);
	//PxController* ctrl = mPhysxController->init(desc, mControllerManager, mPhysics, mScene, NULL, NULL);

	//mControllerManager->createController(*mPhysics, mScene, desc);
	VCN_ASSERT(mPhysxController);
	
	// Load render node meshes as shapes
	ShapeList shapes;
	CreateActorShapes(node, isDynamic, shapes);
	for (ShapeList::const_iterator it = shapes.begin(), end = shapes.end(); it != end; ++it)
	{
		PxShape* actorShape = nodeActor->createShape(*it->get(), *mDefaultMaterial);
		VCN_ASSERT(actorShape);
	}

	// Add body to dynamic actors
	if (isDynamic)
	{
		PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(nodeActor), 1.0f);
	}

	// Add actor to scene
	mScene->addActor(*nodeActor);
	nodeActor->userData = (void*)node;

	return ctrl;
}

///////////////////////////////////////////////////////////////////////
void VCNPhysxCoreImpl::CreateActorShapes(const VCNNode* node, const bool isDynamic, ShapeList& shapes)
{
  // Extract mesh information
  if ( node->Is<VCNRenderNode>() && node->Cast<VCNRenderNode>()->GetMeshID() != kInvalidResID)
  {
    const Vector3& nodeScale = node->GetWorldScale();
    
    const VCNRenderNode* renderNode = node->Cast<VCNRenderNode>();
    const VCNResID meshID = renderNode->GetMeshID();
    VCN_ASSERT(kInvalidResID != meshID);

    VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>(meshID);

    VCNResID vertexCacheID = mesh->GetCacheID(VT_POSITION);
    VCN_ASSERT(kInvalidResID != vertexCacheID);

    VCNCache* vertexCache = VCNResourceCore::GetInstance()->GetResource<VCNCache>(vertexCacheID);
    
    const VCNFloat* vertexBufferPointer = 0;
    vertexCache->Lock( (VCNByte**)&vertexBufferPointer );

    const VCNUInt numVertices = vertexCache->GetCount();
    std::vector<VCNFloat> verticePhysX(3 * numVertices);
    VCNFloat* ptr = &verticePhysX[0];
    const VCNFloat* cptr = vertexBufferPointer;
    for (unsigned int i = 0; i < numVertices; ++i)
    {
      *ptr++ = *cptr++ * nodeScale.x;
      *ptr++ = *cptr++ * nodeScale.y;
      *ptr++ = *cptr++ * nodeScale.z;
    }

    vertexCache->Unlock();

	const bool forceHollow = node->HasProperty(VCNTXT("ForceHollow"));

    /*******************************/
    /*        DYNAMIC MESH         */
    /*******************************/
    if ( isDynamic && !forceHollow )
    {
      // Create descriptor for convex mesh
      PxConvexMeshDesc convexDesc;
      convexDesc.points.count   = numVertices;
      convexDesc.points.stride  = sizeof( VCNFloat ) * 3;
      convexDesc.points.data    = &verticePhysX[0];
      convexDesc.flags          = PxConvexFlag::eCOMPUTE_CONVEX;

      VCN_ASSERT(convexDesc.isValid());

      VCNPhysxMemoryOutputStream buf;
      const bool cookingResult = mCooking->cookConvexMesh(convexDesc, buf);
      VCN_ASSERT( cookingResult );
      
      VCNPhysxMemoryInputData input(buf.getData(), buf.getSize());
      PxConvexMesh* convexMesh = mPhysics->createConvexMesh(input);
      VCN_ASSERT( convexMesh );
      
      std::shared_ptr<PxConvexMeshGeometry> shapeGeom( new PxConvexMeshGeometry(convexMesh) );
      VCN_ASSERT( shapeGeom->isValid() );

      shapes.push_back( shapeGeom );
    }
    /*******************************/
    /*         STATIC MESH         */
    /*******************************/
    else
    {
			VCNResID indexCacheID = mesh->GetFaceCache();
			VCN_ASSERT(kInvalidResID != indexCacheID);

      VCNCache* indexCache = VCNResourceCore::GetInstance()->GetResource<VCNCache>(indexCacheID);
      const VCNUInt numTriangles = indexCache->GetCount() / 3;
      const VCNUInt indexStriding = indexCache->GetStride();
      const VCNUShort* indexBufferPointer = 0;

      indexCache->Lock( (VCNByte**)&indexBufferPointer );

      // Build the triangle mesh.
      PxTriangleMeshDesc meshDesc;
      meshDesc.points.count           = numVertices;
      meshDesc.points.stride          = sizeof( VCNFloat ) * 3;
      meshDesc.points.data            = &verticePhysX[0];

      meshDesc.triangles.count        = numTriangles;
      meshDesc.triangles.stride       = indexStriding * 3;
      meshDesc.triangles.data         = indexBufferPointer;
      meshDesc.flags                  = PxMeshFlag::e16_BIT_INDICES;

      VCN_ASSERT( meshDesc.isValid() );

      VCNPhysxMemoryOutputStream writeBuffer;
      const bool status = mCooking->cookTriangleMesh(meshDesc, writeBuffer);
      VCN_ASSERT( status );

      indexCache->Unlock();
      
      VCNPhysxMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
      PxTriangleMesh* triangleMesh = mPhysics->createTriangleMesh(readBuffer);
      VCN_ASSERT( triangleMesh );

      std::shared_ptr<PxTriangleMeshGeometry> meshShapeDesc( new PxTriangleMeshGeometry(triangleMesh) );
      VCN_ASSERT( meshShapeDesc->isValid() );

      shapes.push_back( meshShapeDesc );
    }
  }

  std::vector<VCNNodeID> children;
  node->GetChildrenArray(children);

  // Process children first
  VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
  for( VCNUInt i=0; i<children.size(); i++ )
  {
    // Get the node where we wish the make the insertion
    VCNNodeID childID = children[i];
    VCNNode* childNode = nodeCore->GetNode(childID);

    if( childNode )
    {
      CreateActorShapes( childNode, isDynamic, shapes );
    }
  }
}

void VCNPhysxCoreImpl::RemoveActor( PxActor* actor )
{
	mScene->removeActor(*actor);
}
