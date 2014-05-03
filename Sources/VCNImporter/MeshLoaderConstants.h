///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Constants only used while loading meshes
///

#ifndef VCNMESHLOADERCONSTANTS_H
#define VCNMESHLOADERCONSTANTS_H

#pragma once

// File extensions
const VCNTChar* kXMLMeshExtension     = VCNTXT("MSH.xml");
const VCNTChar* kXMeshExtension       = VCNTXT(".x");

// Root node
const VCNTChar* kRootMeshElement      = VCNTXT("VCNMeshes");
const VCNTChar* kNodeMeshElement      = VCNTXT("VCNMesh");

// Mesh Element node
const VCNTChar* kAttrMeshElementName  = VCNTXT("Name");


// *** FACES ***

// Face array
const VCNTChar* kNodeFaces            = VCNTXT("Faces");
const VCNTChar* kAttrFacesSize        = VCNTXT("size");

// Face array elements
const VCNTChar* kNodeFace             = VCNTXT("Face");
const VCNTChar* kAttrFaceID           = VCNTXT("id");
const VCNTChar* kAttrFace1            = VCNTXT("VertIndex.x");
const VCNTChar* kAttrFace2            = VCNTXT("VertIndex.y");
const VCNTChar* kAttrFace3            = VCNTXT("VertIndex.z");

// *** POSITIONS ***

// Position array
const VCNTChar* kNodeVertexPositions      = VCNTXT("VertsPos");
const VCNTChar* kAttrVertexPositionsSize  = VCNTXT("size");

// Position array elements
const VCNTChar* kNodeVertexPosition     = VCNTXT("VertPos");
const VCNTChar* kAttrVertexPositionID   = VCNTXT("id");
const VCNTChar* kAttrVertexPositionX    = VCNTXT("Pos.x");
const VCNTChar* kAttrVertexPositionY    = VCNTXT("Pos.y");
const VCNTChar* kAttrVertexPositionZ    = VCNTXT("Pos.z");

// *** NORMALS ***

// Normal array
const VCNTChar* kNodeVertexNormals        = VCNTXT("VertsNormals");
const VCNTChar* kAttrVertexNormalsSize    = VCNTXT("size");

// Normal array elements
const VCNTChar* kNodeVertexNormal      = VCNTXT("VertNormal");
const VCNTChar* kAttrVertexNormalID    = VCNTXT("id");
const VCNTChar* kAttrVertexNormalX     = VCNTXT("Normal.x");
const VCNTChar* kAttrVertexNormalY     = VCNTXT("Normal.y");
const VCNTChar* kAttrVertexNormalZ     = VCNTXT("Normal.z");

// *** COLORS ***

// Color array
const VCNTChar* kNodeVertexColors      = VCNTXT("VertsColors");
const VCNTChar* kAttrVertexColorsSize  = VCNTXT("size");

// Color array elements
const VCNTChar* kNodeVertexColor      = VCNTXT("VertColor");
const VCNTChar* kAttrVertexColorID    = VCNTXT("id");
const VCNTChar* kAttrVertexColorR     = VCNTXT("Color.x");
const VCNTChar* kAttrVertexColorG     = VCNTXT("Color.y");
const VCNTChar* kAttrVertexColorB     = VCNTXT("Color.z");

// *** TEXTURE COORDINATES ***

// Texture coord array
const VCNTChar* kNodeDiffuseTexCoords    = VCNTXT("DiffuseUVs");
const VCNTChar* kNodeNormalTexCoords     = VCNTXT("NormalUVs");
const VCNTChar* kAttrVertexTexCoordsSize = VCNTXT("size");

// Texture coord array elements
const VCNTChar* kNodeVertexTexCoord     = VCNTXT("VertUV");
const VCNTChar* kAttrVertexTexCoordID   = VCNTXT("id");
const VCNTChar* kAttrVertexTexCoordU    = VCNTXT("UV.x");
const VCNTChar* kAttrVertexTexCoordV    = VCNTXT("UV.y");


// Size of array elements (in multiples of floats)
const VCNUInt kFaceUShorts          = 3;
const VCNUInt kPositionFloats       = 3;
const VCNUInt kNormalFloats         = 3;
const VCNUInt kColorFloats          = 3;
const VCNUInt kTexCoordFloats       = 2;

#endif // VCNMESHLOADERCONSTANTS_H
