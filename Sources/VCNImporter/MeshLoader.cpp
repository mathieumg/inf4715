///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "MeshLoader.h"

// Project includes
#include "MeshLoaderConstants.h"

// Engine includes
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNUtils/Aabb.h"

//-------------------------------------------------------------
VCNMesh* VCNMeshLoader::LoadMesh( const VCNString& filename )
{
  // Make sure we at least have a char and an extension
  VCN_ASSERT_MSG( filename.length() > 7, "Incorrect filename!" );

  VCNMesh* retValue = NULL;

  // Is it an XML format or a binary format?
  if( filename.rfind(kXMLMeshExtension) != VCNString::npos )
  {
    retValue = LoadMeshXML( filename );
  }
  else if( filename.rfind(kXMeshExtension) != VCNString::npos )
  {
    retValue = LoadMeshDirectX( filename );
  }
  else
  {
    VCN_ASSERT( false && "Unsupported mesh extension!" );
  }

  // return the mesh pointer!
  return retValue;
}

//-------------------------------------------------------------
VCNMesh* VCNMeshLoader::LoadMeshXML( const VCNString& filename )
{
  VCNMesh* newMesh = NULL;

  // Open the XML document
  XMLElementPtr pRootElem = LoadDocumentRoot( filename );
  VCN_ASSERT( pRootElem!=NULL && "Could not load XML Mesh!" );

  // Go through all the MeshElement nodes
  XMLNodeListPtr elementNodes = 0;
  pRootElem->selectNodes( (VCNTChar*)kNodeMeshElement, &elementNodes );
  VCNLong elementNodeListLength = 0;
  elementNodes->get_length( &elementNodeListLength );
  for( VCNLong i=0; i<elementNodeListLength; i++ )
  {
    // Get the element's node
    XMLNodePtr elementNode = 0;
    elementNodes->get_item( i, &elementNode );

    // Load the mesh element
    newMesh = LoadMeshElementXML( elementNode );
  }

  // Free up the doc memory
  ReleaseDocument();

  // Returns the last mesh we created our of this file
  return newMesh;
}

//-------------------------------------------------------------
VCNMesh* VCNMeshLoader::LoadMeshElementXML( XMLNodePtr elementNode )
{
  // Try to see if this mesh element already exists
  VCNMesh* newElement = new VCNMesh();
  VCN_ASSERT(newElement);

  // Load the base properties
  LoadResourceBaseProperties( elementNode, newElement );

  // Make sure we don't have duplicate meshes
  VCN_ASSERT( !VCNResourceCore::GetInstance()->GetResource<VCNMesh>( newElement->GetName() ) );

  // Load the position array
  VCNSphere boundingSphere;
  VCNAabb aabb;
  newElement->SetCacheID( VT_POSITION, LoadMeshElementPositionXML( elementNode, &boundingSphere, &aabb ) );
  newElement->SetBoundingSphere( boundingSphere );
  newElement->SetBoundingBox( aabb );

  // We need positions AT LEAST to have a mesh
  VCN_ASSERT( newElement->GetCacheID(VT_POSITION) != kInvalidResID && "No positions in mesh!" );

  // Load the array of face indexes (if there is one)
  newElement->SetFaceCache( LoadMeshElementFaceXML( elementNode ) );

  // Load the lighting array (normals and colors)
  newElement->SetCacheID( VT_LIGHTING, LoadMeshElementLightingXML( elementNode ) );

  // Load the diffuse texture coordinate set
  newElement->SetCacheID( VT_DIFFUSE_TEX_COORDS, LoadMeshElementTextureCoordXML( elementNode, VT_DIFFUSE_TEX_COORDS ) );

  // Load the normal map texture coordinate set
  newElement->SetCacheID( VT_NORMAL_TEX_COORDS, LoadMeshElementTextureCoordXML( elementNode, VT_NORMAL_TEX_COORDS ) );

  // We have a new resource ready, add it!
  VCNResourceCore::GetInstance()->AddResource( newElement->GetName(), newElement );

  // Done!
  return newElement;
}

//-------------------------------------------------------------
VCNResID VCNMeshLoader::LoadMeshElementFaceXML( XMLNodePtr elementNode )
{
  // Fetch the node we need from the element node
  XMLNodePtr node = 0;
  elementNode->selectSingleNode( (VCNTChar*)kNodeFaces, &node );

  // It might very well be that we aren't using indexes
  if( node == NULL )
    return kInvalidResID;

  // Get the expected size of the array
  VCNUInt size = 0;
  GetAttributeUInt( node, kAttrFacesSize, size );

  // If we don't have any, leave.
  if( size == 0 )
    return kInvalidResID;

  // Create an array to contain all of this (3 indexes per face)
  VCNUInt stride = kFaceUShorts * kCacheStrides[VT_INDEX];
  VCNUInt numBytes = size * stride;
  VCNByte* buffer = new VCNByte[numBytes];

  // Create some tools...
  VCNUShort* ptrFaces = (VCNUShort*)buffer;
  VCNInt safety = 0;

  // Read the XML and fill the array!
  XMLNodeListPtr faces = 0;
  node->selectNodes( (VCNTChar*)kNodeFace, &faces );
  VCNLong facesLength = 0;
  faces->get_length( &facesLength );
  VCN_ASSERT( facesLength == size && "FILE IS CORRUPTED!" );
  for( VCNLong i=0; i<facesLength; i++ )
  {
    // Get the element's node
    XMLNodePtr faceNode = 0;
    faces->get_item( i, &faceNode );

    // Read the X
    GetAttributeUShort( faceNode, kAttrFace1, *ptrFaces );
    ptrFaces++;

    // Read the Y
    GetAttributeUShort( faceNode, kAttrFace2, *ptrFaces );
    ptrFaces++;

    // Read the Z
    GetAttributeUShort( faceNode, kAttrFace3, *ptrFaces );
    ptrFaces++;

    // Verify the safety to make sure we're reading in the right order
    GetAttributeInt( faceNode, kAttrVertexPositionID, safety );
    VCN_ASSERT( safety==i && "VERTEX AREN'T READ IN ORDER!" );
  }

  // Now give the information to the cache manager 
  // (he'll take care of making this data API specific)
  VCNResID cacheID = VCNRenderCore::GetInstance()->CreateCache( VT_INDEX, buffer, numBytes );

  // Clear the buffer
  delete [] buffer;

  // Return the cache ID
  return cacheID;
}

//-------------------------------------------------------------
VCNResID VCNMeshLoader::LoadMeshElementPositionXML( XMLNodePtr elementNode, VCNSphere* bounding, VCNAabb* aabb /*= NULL*/  )
{
  // Fetch the node we need from the element node
  XMLNodePtr node = 0;
  elementNode->selectSingleNode( (VCNTChar*)kNodeVertexPositions, &node );
  VCN_ASSERT( node != NULL && "No positions in mesh!" );

  // Get the expected size of the array
  VCNUInt size = 0;
  GetAttributeUInt( node, kAttrVertexPositionsSize, size );

  // If we don't have any, leave.
  if( size == 0 )
    return kInvalidResID;

  // Create an array to contain all of this (3 floats per position)
  VCNUInt stride = size * kPositionFloats;
  VCNFloat* buffer = new VCNFloat[ stride ];

  // Create some tools...
  VCNFloat* ptrFloat = buffer;
  VCNInt safety = 0;

  // Keep track of the min and max
  VCNFloat minX, maxX;
  VCNFloat minY, maxY;
  VCNFloat minZ, maxZ;
  minX = minY = minZ = kMaxFloat;
  maxX = maxY = maxZ = kMinFloat;

  // Read the XML and fill the array!
  XMLNodeListPtr positions = 0;
  node->selectNodes( (VCNTChar*)kNodeVertexPosition, &positions );
  VCN_ASSERT( positions != 0 && "FILE IS CORRUPTED!" );
  VCNLong positionsLength = 0;
  positions->get_length( &positionsLength );
  VCN_ASSERT( positionsLength == size && "FILE IS CORRUPTED!" );
  for( VCNLong i=0; i<positionsLength; i++ )
  {
    // Get the element's node
    XMLNodePtr positionNode = 0;
    positions->get_item( i, &positionNode );

    // Read the X
    GetAttributeFloat( positionNode, kAttrVertexPositionX, *ptrFloat );
    if( *ptrFloat < minX )
      minX = *ptrFloat;
    if( *ptrFloat > maxX )
      maxX = *ptrFloat;
    ptrFloat++;

    // Read the Y
    GetAttributeFloat( positionNode, kAttrVertexPositionY, *ptrFloat );
    if( *ptrFloat < minY )
      minY = *ptrFloat;
    if( *ptrFloat > maxY )
      maxY = *ptrFloat;
    ptrFloat++;

    // Read the Z
    GetAttributeFloat( positionNode, kAttrVertexPositionZ, *ptrFloat );
    if( *ptrFloat < minZ )
      minZ = *ptrFloat;
    if( *ptrFloat > maxZ )
      maxZ = *ptrFloat;
    ptrFloat++;

    // Verify the safety to make sure we're reading in the right order
    GetAttributeInt( positionNode, kAttrVertexPositionID, safety );
    VCN_ASSERT( safety==i && "VERTEX AREN'T READ IN ORDER!" );
  }

  // Now give the information to the cache manager 
  // (he'll take care of making this data API specific)
  VCNResID cacheID = VCNRenderCore::GetInstance()->CreateCache( VT_POSITION, buffer, stride*sizeof(VCNFloat) );

  // Clear the buffer
  delete [] buffer;

  Vector3 minVect ( minX, minY, minZ );
  Vector3 maxVect ( maxX, maxY, maxZ );
  Vector3 diagonal = (maxVect - minVect) / 2.0f;
  // If he wants us to fill the AABB, we'll do it for him
  if( bounding )
  {
    VCNSphere tmpSphere( diagonal.Length(), minVect + diagonal );
    *bounding = tmpSphere;
  }
  if (aabb)
  {
	  VCNAabb tempAabb(minVect, maxVect);
	  *aabb = tempAabb;
  }

  // Return the cache ID
  return cacheID;
}

//-------------------------------------------------------------
/// A lighting array is composed of normals and colors
//-------------------------------------------------------------
VCNResID VCNMeshLoader::LoadMeshElementLightingXML( XMLNodePtr elementNode )
{
  // Fetch the normals from the element node
  XMLNodePtr normals = 0;
  elementNode->selectSingleNode( (VCNTChar*)kNodeVertexNormals, &normals );
  bool hasNormals = (normals != NULL);

  // Fetch the colors from the element node
  XMLNodePtr colors = 0;
  elementNode->selectSingleNode( (VCNTChar*)kNodeVertexColors, &colors );
  bool hasColors = (colors != NULL);

  // Get the expected size of the normals
  VCNUInt normalSize = 0;
  if( hasNormals )
  {
    GetAttributeUInt( normals, kAttrVertexNormalsSize, normalSize );
    if( normalSize == 0 )
      hasNormals = false;
  }

  // Get the expected size of the colors
  VCNUInt colorSize = 0;
  if( hasColors )
  {
    GetAttributeUInt( colors, kAttrVertexColorsSize, colorSize );
    if( colorSize == 0 )
      hasColors = false;
  }

  // If we have neither, then no lighting information at all
  if( !hasColors && !hasNormals )
    return kInvalidResID;

  // If we have both, they MUST be of same size!
  if( hasColors && hasNormals && (normalSize != colorSize) )
  {
    VCN_ASSERT_FAIL( "LIGHTING REJECTED!" );
    return kInvalidResID;
  }

  // Now just retain one of the sizes
  VCNLong size = (VCNLong)(hasNormals?normalSize:colorSize);
  
  // Create an array to contain all of this (6 floats per vertex)
  VCNUInt stride = size * (kNormalFloats+kColorFloats);
  VCNFloat* buffer = new VCNFloat[ stride ];

  // Create some tools...
  VCNFloat* ptrFloat = buffer;
  VCNInt safety = 0;

  // Pick out the nodes of every normal (if we have them)
  XMLNodeListPtr normalElements;
  if( hasNormals )
  {
    normalElements = 0;
    normals->selectNodes( (VCNTChar*)kNodeVertexNormal, &normalElements );
    VCNLong normalElementsLength = 0;
    normalElements->get_length( &normalElementsLength );
    VCN_ASSERT( normalElementsLength == size && "FILE IS CORRUPTED!" );
  }

  // Pick out the nodes of every color (if we have them)
  XMLNodeListPtr colorsElements;
  if( hasColors )
  {
    colorsElements = 0;
    colors->selectNodes( (VCNTChar*)kNodeVertexColor, &colorsElements );
    VCNLong colorElementsLength = 0;
    normalElements->get_length( &colorElementsLength );
    VCN_ASSERT( colorElementsLength == size && "FILE IS CORRUPTED!" );
  }

  // Now read it in!
  for( VCNLong i=0; i<size; i++ )
  {
    // Normals
    if( hasNormals )
    {
      // Get the element's node
      XMLNodePtr normalNode = 0;
      normalElements->get_item( i, &normalNode );

      // Read the X
      GetAttributeFloat( normalNode, kAttrVertexNormalX, *ptrFloat );
      ptrFloat++;

      // Read the Y
      GetAttributeFloat( normalNode, kAttrVertexNormalY, *ptrFloat );
      ptrFloat++;

      // Read the Z
      GetAttributeFloat( normalNode, kAttrVertexNormalZ, *ptrFloat );
      ptrFloat++;

      // Verify the safety to make sure we're reading in the right order
      GetAttributeInt( normalNode, kAttrVertexNormalID, safety );
      VCN_ASSERT( safety==i && "VERTEX AREN'T READ IN ORDER!" );
    }
    else
    {
      // Put three zeros instead
      *ptrFloat = 0.0f;
      ptrFloat++;
      *ptrFloat = 0.0f;
      ptrFloat++;
      *ptrFloat = 0.0f;
      ptrFloat++;
    }

    // Then colors
    if( hasColors )
    {
      // Get the element's node
      XMLNodePtr colorNode = 0;
      colorsElements->get_item( i, &colorNode );

      // Read the X
      GetAttributeFloat( colorNode, kAttrVertexColorR, *ptrFloat );
      ptrFloat++;

      // Read the Y
      GetAttributeFloat( colorNode, kAttrVertexColorG, *ptrFloat );
      ptrFloat++;

      // Read the Z
      GetAttributeFloat( colorNode, kAttrVertexColorB, *ptrFloat );
      ptrFloat++;

      // Verify the safety to make sure we're reading in the right order
      GetAttributeInt( colorNode, kAttrVertexColorID, safety );
      VCN_ASSERT( safety==i && "VERTEX AREN'T READ IN ORDER!" );
    }
    else
    {
      // Put three ones instead (white)
      *ptrFloat = 1.0f;
      ptrFloat++;
      *ptrFloat = 1.0f;
      ptrFloat++;
      *ptrFloat = 1.0f;
      ptrFloat++;
    }
  }

  // Now give the information to the cache manager 
  // (he'll take care of making this data API specific)
  VCNResID cacheID = VCNRenderCore::GetInstance()->CreateCache( VT_LIGHTING, buffer, stride*sizeof(VCNFloat) );

  // Clear the buffer
  delete [] buffer;

  // Return the cache ID
  return cacheID;
}


//-------------------------------------------------------------
VCNResID VCNMeshLoader::LoadMeshElementTextureCoordXML( XMLNodePtr elementNode, VCNCacheType coordType )
{
  XMLNodePtr node = NULL;

  // Fetch the node we need from the element node
  switch( coordType )
  {
  case VT_DIFFUSE_TEX_COORDS:
    elementNode->selectSingleNode( (VCNTChar*)kNodeDiffuseTexCoords, &node );
    break;
  case VT_NORMAL_TEX_COORDS:
    elementNode->selectSingleNode( (VCNTChar*)kNodeNormalTexCoords, &node );
    break;

  default:
    VCN_ASSERT( false && "Trying to load unrecognized coord type!" );
  }

  // If we didn't find it, we don't have it
  if( node == NULL )
    return kInvalidResID;

  // Get the expected size of the array
  VCNUInt size = 0;
  GetAttributeUInt( node, kAttrVertexTexCoordsSize, size );

  // If we don't have any, leave.
  if( size == 0 )
    return kInvalidResID;

  // Create an array to contain all of this (2 floats per position)
  VCNUInt stride = size * kTexCoordFloats;
  VCNFloat* buffer = new VCNFloat[ stride ];

  // Create some tools...
  VCNFloat* ptrFloat = buffer;
  VCNInt safety = 0;

  // Read the XML and fill the array!
  XMLNodeListPtr textureCoords = 0;
  node->selectNodes( (VCNTChar*)kNodeVertexTexCoord, &textureCoords );
  VCNLong textureCoordsLength = 0;
  textureCoords->get_length( &textureCoordsLength );
  VCN_ASSERT( textureCoordsLength == size && "FILE IS CORRUPTED!" );
  for( VCNLong i=0; i<textureCoordsLength; i++ )
  {
    // Get the first one
    XMLNodePtr textureCoordNode = 0;
    textureCoords->get_item( i, &textureCoordNode );

    // Read the U
    GetAttributeFloat( textureCoordNode, kAttrVertexTexCoordU, *ptrFloat );
    ptrFloat++;

    // Read the V
    GetAttributeFloat( textureCoordNode, kAttrVertexTexCoordV, *ptrFloat );
    ptrFloat++;

    // Verify the safety to make sure we're reading in the right order
    GetAttributeInt( textureCoordNode, kAttrVertexTexCoordID, safety );
    VCN_ASSERT( safety==i && "VERTEX AREN'T READ IN ORDER!" );
  }

  // Now give the information to the cache manager 
  // (he'll take care of making this data API specific)
  VCNResID cacheID = VCNRenderCore::GetInstance()->CreateCache( coordType, buffer, stride*sizeof(VCNFloat) );

  // Clear the buffer
  delete [] buffer;

  // Return the cache ID
  return cacheID;
}

//-------------------------------------------------------------
VCNMesh* VCNMeshLoader::LoadMeshDirectX( const VCNString& filename )
{
  VCNString meshName = filename;
  VCNString::size_type pos = meshName.find_first_of('\\');
  meshName.replace(0, pos + 1, VCNTXT(""));
  meshName.replace(meshName.length() - VCNString(VCNTXT(".x")).length(), meshName.length(), VCNTXT(""));

  // Make sure we don't have duplicate meshes
  VCN_ASSERT( !VCNResourceCore::GetInstance()->GetResource<VCNMesh>( meshName ) );

  VCNMesh* newMesh = VCNRenderCore::GetInstance()->CreateMesh( filename );
  
  newMesh->SetName( meshName );
  newMesh->SetVersion( 1.0 );

  // Now lets put it in the resource core to get our ID
  VCNResourceCore::GetInstance()->AddResource( meshName, newMesh );

  return newMesh;
}
