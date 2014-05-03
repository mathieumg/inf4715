///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// This class contains all the facilities to read meshes from
/// files (XML or BIN) and to write meshes to files (BIN).
///

#ifndef VICUNA_MESH_LOADER
#define VICUNA_MESH_LOADER

#include "XMLLoader.h"
#include "VCNUtils/RenderTypes.h"


// Forward declarations
class VCNMesh;
class VCNSphere;
class VCNAabb;

class VCNMeshLoader : public VCNXMLLoader
{

public:

  static VCNMesh* LoadMesh( const VCNString& filename );

protected:

  // Functions relating to XML loading...
  static VCNMesh*  LoadMeshXML( const VCNString& filename );
  static VCNMesh*  LoadMeshElementXML( XMLNodePtr node );
  static VCNResID  LoadMeshElementFaceXML( XMLNodePtr elementNode );
  static VCNResID  LoadMeshElementPositionXML( XMLNodePtr elementNode, VCNSphere* bounding = NULL, VCNAabb* aabb = NULL );
  static VCNResID  LoadMeshElementLightingXML( XMLNodePtr elementNode );
  static VCNResID  LoadMeshElementTextureCoordXML( XMLNodePtr elementNode, VCNCacheType coordType );

  /// Loads a DirectX mesh file
  static VCNMesh*  LoadMeshDirectX( const VCNString& filename );

protected:
  // This utility class should not be instanced.
  VCNMeshLoader() {}
  virtual ~VCNMeshLoader() {}

};

#endif
