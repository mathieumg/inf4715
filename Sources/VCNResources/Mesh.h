///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Mesh Resource base class.
///

#ifndef VICUNA_MESH
#define VICUNA_MESH

#pragma once

#include "VCNResources/Resource.h"
#include "VCNUtils/Matrix.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/Sphere.h"
#include "VCNUtils/Aabb.h"

class VCNNode;

//-------------------------------------------------------------
// The actual mesh class
//-------------------------------------------------------------
class VCNMesh : public VCNResource
{
	VCN_CLASS;

public:

	VCNMesh();
	virtual ~VCNMesh();

	// CacheID accessors
	void SetCacheID( VCNCacheType type, VCNResID cacheID );
	VCNResID GetCacheID( VCNCacheType type ) const;
	VCNUInt GetVertexCount() const;

	// Face index cache accessors
	void SetFaceCache( VCNResID cacheID );
	VCNResID GetFaceCache() const;

	// Material accessors
	void SetMaterialID( const VCNResID materialID );
	const VCNResID GetMaterialID() const;

	// Face count accessors
	VCNUInt GetFaceCount() const;
	void SetFaceCount(const VCNUInt faceCount) { mFaceCount = faceCount; }

	// Gets/Sets the primitive type (list, strip, etc)
	void SetPrimitiveType(VCNPrimitiveType primitiveType) { mPrimitiveType = primitiveType; }
	const VCNPrimitiveType GetPrimitiveType() const { return mPrimitiveType; }

	// Calculate the bounding sphere
	const VCNSphere& GetBoundingSphere() const;
	void SetBoundingSphere( const VCNSphere& sphere );

	const VCNAabb& GetBoundingBox() const;
	void SetBoundingBox( const VCNAabb& box );

	// Checks if the mesh is indexed
	const VCNBool IsIndexed() const;

	std::size_t GetBoneCount() const { return m_BoneOffsets.size(); }

	const std::vector<Matrix4>& GetBoneOffsets() const { return m_BoneOffsets; }

	void SetBoneTransforms(const std::vector<Matrix4>* value) { m_BoneTransforms = value; }
	const std::vector<Matrix4>* GetBoneTransforms() const { return m_BoneTransforms; }

	void SetBoneOffsets(const std::vector<Matrix4>& value) { m_BoneOffsets = value; }
	void SetBoneOffsets(std::vector<Matrix4>&& value) { m_BoneOffsets = std::move(value); }

	void SetBoneInfluenceCount(std::size_t count) { m_BoneInfluenceCount = count; }
	std::size_t GetBoneInfluenceCount() const { return m_BoneInfluenceCount; }

	std::size_t GetPaletteSize() const { return m_MatrixPaletteIndexes.size(); }

	const std::vector<std::size_t>& GetMatrixPaletteIndexes() const { return m_MatrixPaletteIndexes; }
	void SetMatrixPaletteIndexes(const std::vector<std::size_t>& paletteIndexes) { m_MatrixPaletteIndexes = paletteIndexes; }

protected:

	typedef std::vector<Matrix4>* TransformList;

	// The bounding sphere for this mesh
	VCNSphere                   mBoundingSphere;

	// The AABB of the mesh
	VCNAabb						mBoundingAabb;
										          
	// ID for index cache
	VCNResID                    mFaceCacheID;
										          
	// IDs for ever kind of cache
	VCNResID                    mCacheIDs[VT_NUM_VERTEX_TYPES];
										          
	// Mesh can specify they own material (can be 0)
	VCNResID                    mMaterialID;

	// Defines the primitive mesh type (triangle list, strip, etc.)
	VCNPrimitiveType            mPrimitiveType;

	// Overrides the face count of the mesh
	VCNUInt                     mFaceCount;

	// Skinning
	std::size_t                 m_BoneInfluenceCount;
	const std::vector<Matrix4>* m_BoneTransforms;
	std::vector<Matrix4>        m_BoneOffsets;
	std::vector<std::size_t>    m_MatrixPaletteIndexes;
};

#endif
