///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "D3D9ModelLoader.h"

#include "D3D9.h"
#include "D3D9Allocator.h"
#include "D3D9Converter.h"
#include "D3D9Help.h"
#include "D3D9Types.h"

#include "VCNRenderer/XformCore.h"
#include "VCNResources/Model.h"
#include "VCNResources/ResourceCore.h"

#include <d3dx9.h>
#include <d3dx9math.h>

//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame, LPD3DXFRAME frameRoot )
{
	HRESULT hr;

	if( pFrame->pMeshContainer != NULL )
	{
		// call setup routine
		hr = ( ( MultiAnimMC* )pFrame->pMeshContainer )->SetupBonePtrs( frameRoot );
		if( FAILED( hr ) )
			return hr;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		hr = SetupBoneMatrixPointers( pFrame->pFrameSibling, frameRoot );
		if( FAILED( hr ) )
			return hr;
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		hr = SetupBoneMatrixPointers( pFrame->pFrameFirstChild, frameRoot );
		if( FAILED( hr ) )
			return hr;
	}

	return S_OK;
}

VCNModel* D3DModelLoader::Load(const std::wstring& modelName)
{
	D3DModel model;
	auto begin = modelName.find_first_of(L'\\');
	if (begin == std::wstring::npos)
		begin = 0;
	else
		begin++;

	model.m_Name = modelName.substr(begin, modelName.size() - 2 - begin);

	VCND3D9* instance = VCNRenderCore::GetInstance()->Cast<VCND3D9>();
	IDirect3DDevice9* device = instance->GetD3DDevice();

	D3DAllocator alloc(&model);
	HRESULT result = D3DXLoadMeshHierarchyFromX( modelName.c_str(), D3DXMESH_SYSTEMMEM, device,
		&alloc, NULL, &model.m_FrameRoot, &model.m_AnimController );

	assert(SUCCEEDED(result));

	result = SetupBoneMatrixPointers( model.m_FrameRoot, model.m_FrameRoot );
	assert(SUCCEEDED(result));

	D3DXVECTOR3 center;
	float radius;
	D3DXFrameCalculateBoundingSphere( model.m_FrameRoot, &center, &radius );
	model.m_BoundingSphere = VCNSphere(radius, Vector3(center.x, center.y, center.z));

	return D3DConverter::ConvertD3DModel( model, device );
}