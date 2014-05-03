///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "D3D9Converter.h"

#include "D3D9.h"
#include "D3D9Animator.h"
#include "D3D9Texture.h"
#include "D3D9Types.h"

#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNResources/Material.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/Model.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/StringUtils.h"

#include <d3dx9.h>
#include <d3dx9anim.h>
#include <limits>
#include <vector>

inline bool ContainsFlag(DWORD flagSet, DWORD flag)
{
	return (flagSet & flag) == flag;
}

inline bool ContainsPositionInformation(DWORD fvf)
{
	return (D3DFVF_POSITION_MASK & fvf) != 0;
}

inline bool ContainsBlending(DWORD fvf)
{
	return ContainsFlag(fvf, D3DFVF_XYZB1) || ContainsFlag(fvf, D3DFVF_XYZB2) ||
		ContainsFlag(fvf, D3DFVF_XYZB3) || ContainsFlag(fvf, D3DFVF_XYZB4) || ContainsFlag(fvf, D3DFVF_XYZB5);
}

inline DWORD GetPositionStride(DWORD fvf)
{
	DWORD onlyPos = D3DFVF_POSITION_MASK & fvf;
	return D3DXGetFVFVertexSize(onlyPos);
}

VCNModel* D3DConverter::ConvertD3DModel(const D3DModel& d3dmodel, LPDIRECT3DDEVICE9 device)
{
	VCNNode* root = 0;
	
	if ( d3dmodel.m_MeshContainers.size() > 1 ) 
	{
		root = VCNNodeCore::GetInstance()->CreateNode<VCNNode>();
	}

	for (auto it = d3dmodel.m_MeshContainers.begin(),end = d3dmodel.m_MeshContainers.end(); it != end; ++it)
	{
		LPD3DXMESHCONTAINER meshContainer = *it;
		std::wstring name = StringBuilder() << d3dmodel.m_Name << "_Mesh_" << meshContainer->Name;
		VCNNode* nodepart = ConvertMesh(name, meshContainer, d3dmodel.m_FrameRoot, d3dmodel.m_AnimController, device);

		if ( d3dmodel.m_MeshContainers.size() > 1 ) 
		{
			root->AttachChild( nodepart->GetNodeID() );
		}
		else
		{
			root = nodepart;
		}
	}

	VCNModel* model = new VCNModel( root );
	model->SetName( d3dmodel.m_Name );
	model->SetVersion( 1.0f );
	VCNResourceCore::GetInstance()->AddResource( d3dmodel.m_Name, model );

	return model;
}

VCNNode* D3DConverter::ConvertMesh(const std::wstring& name, LPD3DXMESHCONTAINER baseMeshContainer,
	D3DXFRAME* frameRoot, ID3DXAnimationController* animController, LPDIRECT3DDEVICE9 device)
{
	MultiAnimMC* meshContainer = static_cast<MultiAnimMC*>(baseMeshContainer);
	ID3DXMesh* systemMesh = meshContainer->MeshData.pMesh;

	// Load vertex caches
	//
	DWORD meshFVF = systemMesh->GetFVF();
	size_t vertexCount = systemMesh->GetNumVertices();
	const DWORD stride = D3DXGetFVFVertexSize( meshFVF );
	const DWORD normalStride = D3DXGetFVFVertexSize( D3DFVF_NORMAL );
	const DWORD diffuseStride = D3DXGetFVFVertexSize( D3DFVF_DIFFUSE );
	const DWORD textureStride = D3DXGetFVFVertexSize( D3DFVF_TEX1 );

	std::vector<VCNFloat> vtPositionBuffer( vertexCount * kCacheStrides[VT_POSITION] );
	std::vector<VCNFloat> vtBlendWeights( vertexCount * kCacheStrides[VT_BLENDWEIGHTS] ); //TODO Verify the size of this shit
	std::vector<DWORD> vtBlendIndices( vertexCount * kCacheStrides[VT_BLENDINDICES] ); //TODO Verify the size of this shit

	std::vector<VCNFloat> vtNormalBuffer( vertexCount * kCacheStrides[VT_LIGHTING] );
	std::vector<VCNFloat> vtTextureBuffer( vertexCount * kCacheStrides[VT_DIFFUSE_TEX_COORDS] );

	VCNFloat* vtPositionBuf = &vtPositionBuffer[0];
	VCNFloat* vtBlendWeightBuf = &vtBlendWeights[0];
	DWORD* vtBlendIndicesBuf = &vtBlendIndices[0];

	VCNFloat* vtNormalBuf = &vtNormalBuffer[0];
	VCNFloat* vtTextureBuf = &vtTextureBuffer[0];

	BYTE* vbptr = NULL;
	BYTE* vblineptr = NULL;
	systemMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&vblineptr);

	DWORD positionBlendAndIndicesStride = GetPositionStride(meshFVF);

	for(VCNUInt i = 0; i < vertexCount; ++i)
	{
		vbptr = vblineptr;

		if ( ContainsPositionInformation(meshFVF) )
		{
			// Read position
			float* posData = (float*)vbptr;
			*vtPositionBuf = posData[0]; vtPositionBuf++;
			*vtPositionBuf = posData[1]; vtPositionBuf++;
			*vtPositionBuf = posData[2]; vtPositionBuf++;

			if (ContainsBlending(meshFVF))
			{
				// Get blend weights
				size_t blendCount = (positionBlendAndIndicesStride / 4) - 3 - 1; // -3 to remove xyz, -1 to remove indices which come after
				for(size_t i = 0; i < blendCount; ++i)
				{
					*vtBlendWeightBuf = posData[3 + i]; vtBlendWeightBuf++;
				}
				vtBlendWeightBuf += 4 - blendCount; //each item is an array of 4 floats

				// Get blend indices
				// TODO SKIN Check the format we have to send this data as.
				if ( ContainsFlag(meshFVF, D3DFVF_LASTBETA_UBYTE4) )
				{
					*vtBlendIndicesBuf = ((DWORD*)vbptr)[3 + blendCount]; vtBlendIndicesBuf++;
				}
			}

			vbptr += positionBlendAndIndicesStride;
		}
		else
		{
			VCN_ASSERT_FAIL( VCNTXT("Mesh FVF not supported (no vertex position) [FVF = %d, stride = %d]"), meshFVF, stride );
		}

		// Read normal
		if ( ContainsFlag(meshFVF, D3DFVF_NORMAL) )
		{
			D3DXVECTOR3* normal = (D3DXVECTOR3*)(vbptr);
			*vtNormalBuf = normal->x; vtNormalBuf++;
			*vtNormalBuf = normal->y; vtNormalBuf++;
			*vtNormalBuf = normal->z; vtNormalBuf++;

			// Set default diffuse color
			std::fill(vtNormalBuf, vtNormalBuf+3, 1.0f); vtNormalBuf += 3;

			vbptr += normalStride;
		}
		else
		{
			VCN_ASSERT_FAIL( VCNTXT("Mesh FVF not supported (no normals) [FVF = %d, stride = %d]"), meshFVF, stride );
		}

		if ( ContainsFlag(meshFVF, D3DFVF_DIFFUSE) ) vbptr += diffuseStride;

		// Read texcoords
		// the check with D3DFVF_TEX0 is pretty useless as it's always true... the flag value is 0...
		if ( ContainsFlag(meshFVF, D3DFVF_TEX0) || ContainsFlag(meshFVF, D3DFVF_TEX1) )
		{
			float* texCoords = (float*)(vbptr);
			*vtTextureBuf = texCoords[0]; vtTextureBuf++;
			*vtTextureBuf = texCoords[1]; vtTextureBuf++;

			vbptr += textureStride;
		}
		else
		{
			VCN_ASSERT_FAIL( VCNTXT("Mesh FVF not supported (no texture coordinates) [FVF = %d, stride = %d]"), meshFVF, stride );
		}

		vblineptr += stride;
	}
	systemMesh->UnlockVertexBuffer();

	VCND3D9* renderer = VCNRenderCore::GetInstance()->Cast<VCND3D9>();

	// Generate cache resources that will be bind to Vicuna's meshes
	VCNResID positionCache = renderer->CreateCache(VT_POSITION, &vtPositionBuffer[0], vertexCount * kCacheStrides[VT_POSITION]);
	VCNResID lightingCache = renderer->CreateCache(VT_LIGHTING, &vtNormalBuffer[0], vertexCount * kCacheStrides[VT_LIGHTING]);
	VCNResID textureCache = renderer->CreateCache(VT_DIFFUSE_TEX_COORDS, &vtTextureBuffer[0], vertexCount * kCacheStrides[VT_DIFFUSE_TEX_COORDS]);

	VCNResID blendWeightCache = renderer->CreateCache(VT_BLENDWEIGHTS, &vtBlendWeights[0], vertexCount * kCacheStrides[VT_BLENDWEIGHTS]);
	VCNResID blendIndiceCache = renderer->CreateCache(VT_BLENDINDICES, &vtBlendIndices[0], vertexCount * kCacheStrides[VT_BLENDINDICES]);

	// Get model faces
	//
	VCNUShort* ibptr = 0;
	std::vector<VCNUShort> indices( systemMesh->GetNumFaces() * 3 );
	systemMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&ibptr);
	for(VCNUInt i = 0; i < systemMesh->GetNumFaces(); i++)
	{
		indices[(i * 3) + 0] = *(ibptr++);
		indices[(i * 3) + 1] = *(ibptr++);
		indices[(i * 3) + 2] = *(ibptr++);
	}
	systemMesh->UnlockIndexBuffer();

	// Load materials
	//
	std::vector<VCNResID> materialIDS;
	D3DXMATERIAL* d3dxMaterials = meshContainer->pMaterials;
	for (DWORD i = 0; i < meshContainer->NumMaterials; ++i)
	{
		VCNResID materialID = kInvalidResID;

		// Create the texture if it exists - it may not
		if ( d3dxMaterials[i].pTextureFilename )
		{
			VCNResID textureID = kInvalidResID;

			VCNString texturePath = VCNTXT("Textures/");
			texturePath += VCN_A2W(d3dxMaterials[i].pTextureFilename);

			// Check if the texture is already loaded
			VCND3D9Texture* resTexture = VCNResourceCore::GetInstance()->GetResource<VCND3D9Texture>(texturePath);
			if ( !resTexture )
			{
				textureID = VCNMaterialCore::GetInstance()->CreateTexture(texturePath);
				VCN_ASSERT_MSG( textureID != kInvalidResID, VCNTXT("Can't load texture %s"), texturePath.c_str() );
			}
			else
			{
				textureID = resTexture->GetResourceID();
			}

			VCNMaterial* material = new VCNMaterial();
			const VCNString materialName = StringBuilder() << name << VCNTXT("_material_") << i;
			material->SetName( materialName );
			VCNColor ambient = VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Ambient);
			ambient.a = 1.0f;
			ambient += VCNColor(0.5f, 0.5f, 0.5f, 0);

			material->SetAmbientColor( ambient );
			material->SetDiffuseColor( VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Diffuse) );
			material->SetSpecularColor( VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Specular) );
			material->SetSpecularPower( d3dxMaterials[i].MatD3D.Power );

			VCNEffectParamSet& params = material->GetEffectParamSet();
			params.SetEffectID( eidSkinned );
			params.AddResource( VCNTXT("DiffuseTexture"), textureID );

			// Add material as a resource.
			materialID = VCNResourceCore::GetInstance()->AddResource( material->GetName(), material );
		}

		materialIDS.push_back( materialID );
	}

	// Get the model attribute table with which we will instantiate has many mesh.
	//
	DWORD attribTableSize;
	std::vector<D3DXATTRIBUTERANGE> attribTable;
	HRESULT hr = systemMesh->GetAttributeTable( 0, &attribTableSize );
	if ( FAILED(hr) )
		return 0;
	attribTable.resize( attribTableSize );
	hr = systemMesh->GetAttributeTable( &attribTable[0], &attribTableSize );
	if ( FAILED(hr) )
		return 0;

	// Set the root node
	VCNNode* rootNode = attribTableSize > 1 ?  VCNNodeCore::GetInstance()->CreateNode<VCNNode>() : 
		VCNNodeCore::GetInstance()->CreateNode<VCNRenderNode>();

	rootNode->SetTag( StringBuilder() << name << VCNTXT("_Root") );

	// For each attribute, we get the material texture
	for (DWORD i = 0; i < attribTableSize; ++i)
	{
		VCNRenderNode* partNode = attribTableSize == 1 ? 
			safe_pointer_cast<VCNRenderNode*>( rootNode ) : 
		VCNNodeCore::GetInstance()->CreateNode<VCNRenderNode>();

		const VCNString partNodeName = StringBuilder() << name << VCNTXT("_Part_") << i;
		partNode->SetTag( partNodeName );

		VCNMesh* partMesh = new VCNMesh();
		partMesh->SetCacheID(VT_POSITION, positionCache); //SKIN do this for blend weights and blend indices
		partMesh->SetCacheID(VT_LIGHTING, lightingCache);
		partMesh->SetCacheID(VT_DIFFUSE_TEX_COORDS, textureCache);
		partMesh->SetCacheID(VT_BLENDWEIGHTS, blendWeightCache);
		partMesh->SetCacheID(VT_BLENDINDICES, blendIndiceCache);
		partMesh->SetPrimitiveType(PT_TRIANGLELIST);

		partMesh->SetBoneInfluenceCount( meshContainer->m_dwMaxNumFaceInfls );
		size_t numBones = meshContainer->pSkinInfo == nullptr ? 0 : meshContainer->pSkinInfo->GetNumBones();
		if (numBones > 0)
		{
			auto offsets = std::vector<Matrix4>(numBones);
			std::transform( std::begin(meshContainer->m_amxBoneOffsets), std::end(meshContainer->m_amxBoneOffsets), std::begin(offsets), [](const D3DXMATRIX& mat)
			{
				return Matrix4( (VCNFloat*)mat.m );
			});

			partMesh->SetBoneOffsets( std::move(offsets) );

			LPD3DXBONECOMBINATION boneCombination = reinterpret_cast<LPD3DXBONECOMBINATION>( 
				meshContainer->m_pBufBoneCombos->GetBufferPointer() );

			size_t numPaletteEntries = meshContainer->m_dwNumPaletteEntries;

			std::vector<size_t> matriceIndexes;

			for(size_t paletteIndex = 0; paletteIndex < numPaletteEntries; ++paletteIndex)
			{
				size_t matIndex = boneCombination[i].BoneId[paletteIndex];
				if ( matIndex == std::numeric_limits<size_t>::max())
					continue;

				matriceIndexes.push_back(matIndex);
			}

			partMesh->SetMatrixPaletteIndexes(matriceIndexes);
		}

		const DWORD partFaceCount = attribTable[i].FaceCount;
		const void* partFaceBufferStart = &indices[attribTable[i].FaceStart * 3];
		const VCNResID indexCacheID = renderer->CreateCache(VT_INDEX, partFaceBufferStart, partFaceCount * 3 * kCacheStrides[VT_INDEX]);
		partMesh->SetFaceCount( attribTable[i].FaceCount );
		partMesh->SetFaceCache( indexCacheID );

		// Compute bounding sphere
		float radius;
		D3DXVECTOR3 center;
		D3DXComputeBoundingSphere( (D3DXVECTOR3*)(&vtPositionBuffer[0] + attribTable[i].VertexStart * 3), 
			attribTable[i].VertexCount, stride, &center, &radius );
		VCNSphere modelBoundSphere( radius, V2V<Vector3>(center) );
		partMesh->SetBoundingSphere( modelBoundSphere );

		// Add mesh resource
		const VCNString partMeshName = StringBuilder() << name << VCNTXT("_part_") << i;
		const VCNResID partMeshID = VCNResourceCore::GetInstance()->AddResource( partMeshName, partMesh );

		// Set model part node attributes
		partNode->SetMeshID( partMeshID );
		if (animController && numBones > 0)
		{
			partNode->AddComponent( new VCND3DAnimator(partMeshID, animController, frameRoot, meshContainer->m_apmxBonePointers) );
		}

		size_t index = attribTable[i].AttribId;
		index = index >= materialIDS.size() ? materialIDS.size() - 1 : index;

		partNode->SetMaterialID( materialIDS[index] );

		// Add children to root
		if ( attribTableSize > 1 )
		{
			rootNode->AttachChild( partNode->GetNodeID() );
		}
	}

	return rootNode;
}
