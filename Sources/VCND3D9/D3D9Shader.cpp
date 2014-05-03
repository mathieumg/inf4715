///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D shader implementation
///

#include "Precompiled.h"

#include "D3D9.h"
#include "D3D9Mesh.h"
#include "D3D9Shader.h"
#include "D3D9ShaderConstants.h"
#include "D3D9Texture.h"
#include "D3D9Declaration.h"
#include "D3D9Cache.h"

#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCND3D9Shader, VCNShader );


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCND3D9Shader::VCND3D9Shader()
	: mShader(0)
	, mTechnique(0)
	, mVertexDeclaration(0)
{
}


///////////////////////////////////////////////////////////////////////
VCND3D9Shader::VCND3D9Shader(LPD3DXEFFECT effect)
	: mShader(effect)
	, mVertexDeclaration(0)
{
	mTechnique = mShader->GetTechniqueByName( kShaderBaseTechnique );
}


//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCND3D9Shader::~VCND3D9Shader()
{
	if( mVertexDeclaration )
	{
		delete mVertexDeclaration;
	}

	if( mShader )
	{
		mShader->Release();
	}
}


//-------------------------------------------------------------
// Make the shader active
//-------------------------------------------------------------
void VCND3D9Shader::Bind()
{
	// Select the technique
	HRESULT hr = mShader->SetTechnique( mTechnique );
	VCN_ASSERT( SUCCEEDED(hr) );

	// Set the vertex declaration for this kind of shader
	hr = GetD3DDevice()->SetVertexDeclaration( mVertexDeclaration->GetDeclaration() );
	VCN_ASSERT( SUCCEEDED(hr) );
}


//-------------------------------------------------------------
// Trigger the render of non-indexed primitives
//-------------------------------------------------------------
void VCND3D9Shader::RenderPrimitive( VCNUInt faceCount, VCNInt primitiveType /*= D3DPT_TRIANGLELIST*/ )
{
	HRESULT hr;

	// Get a pointer to the D3D device
	LPDIRECT3DDEVICE9 device = ((VCND3D9*)VCND3D9::GetInstance())->GetD3DDevice();

	// Start the technique and commit all the data
	VCNUInt numPasses;
	hr = mShader->Begin( &numPasses, 0 );
	VCN_ASSERT( SUCCEEDED(hr) );

	// Render all the passes of this technique (generally only 1).
	for( VCNUInt i=0; i<numPasses; ++i )
	{
		// Start the pass
		hr = mShader->BeginPass( i );
		VCN_ASSERT( SUCCEEDED(hr) );

		// Render a non-indexed tri-strip
		hr = device->DrawPrimitive( (D3DPRIMITIVETYPE)primitiveType, 0, faceCount );
		VCN_ASSERT( SUCCEEDED(hr) && "VCNDXShader::RenderPrimitive - DRAW FAILED!" );

		// End the pass
		hr = mShader->EndPass();
		VCN_ASSERT( SUCCEEDED(hr) );
	}

	// over!
	hr = mShader->End();
	VCN_ASSERT( SUCCEEDED(hr) );
}


//-------------------------------------------------------------
// Trigger the render of indexed primitives
//-------------------------------------------------------------
void VCND3D9Shader::RenderIndexedPrimitive( VCNUInt faceCount, VCNUInt vertexCount, VCNInt primitiveType /*= D3DPT_TRIANGLELIST*/  )
{
	HRESULT hr = S_FALSE;

	// Get a pointer to the D3D device
	LPDIRECT3DDEVICE9 device = GetD3DDevice();

	// Start the technique and commit all the data
	VCNUInt numPasses;
	hr = mShader->Begin( &numPasses, 0 );
	VCN_ASSERT( SUCCEEDED(hr) );

	// Render all the passes of this technique (generally only 1).
	for( VCNUInt i=0; i<numPasses; ++i )
	{
		// Start the pass
		hr = mShader->BeginPass( i );
		VCN_ASSERT( SUCCEEDED(hr) );

		// Render indexed primitives
		hr = device->DrawIndexedPrimitive( (D3DPRIMITIVETYPE)primitiveType, 0, 0, vertexCount, 0, faceCount );
		VCN_ASSERT( hr != D3DERR_INVALIDCALL && "VCNDXShader::RenderIndexedPrimitive - D3DERR_INVALIDCALL!" );
		VCN_ASSERT( SUCCEEDED(hr) && "VCNDXShader::RenderIndexedPrimitive - DRAW FAILED!" );

		// End the pass
		hr = mShader->EndPass();
		VCN_ASSERT( SUCCEEDED(hr) );
	}

	hr = mShader->End();
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::RenderIndexedPrimitive(
	VCNPrimitiveType primitiveType, VCNInt baseVertexIndex, 
	VCNUInt minVertexIndex, VCNUInt numVertices, VCNUInt startIndex, VCNUInt primCount)
{
	HRESULT hr = S_FALSE;

	// Get a pointer to the D3D device
	LPDIRECT3DDEVICE9 device = GetD3DDevice();

	// Render primitives
	hr = device->DrawIndexedPrimitive( 
		(D3DPRIMITIVETYPE)primitiveType, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount);
	VCN_ASSERT( hr != D3DERR_INVALIDCALL && "VCNDXShader::RenderIndexedPrimitive - D3DERR_INVALIDCALL!" );
	VCN_ASSERT( SUCCEEDED(hr) && "VCNDXShader::RenderIndexedPrimitive - DRAW FAILED!" );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::CreateVertexDeclaration(const VCNDeclarationElementList& list)
{
	VCN_ASSERT( mVertexDeclaration == NULL );

	mVertexDeclaration = static_cast<VCNDeclarationD3D9*>(VCNRenderCore::GetInstance()->CreateVertexDeclaration(list));
}


///////////////////////////////////////////////////////////////////////
VCNShaderParameter VCND3D9Shader::GetParameterByName(const char* name) const
{
	VCN_ASSERT( mShader );

	return reinterpret_cast<VCNShaderParameter>( mShader->GetParameterByName( NULL, name ) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetColor(const VCNShaderParameter& param, const VCNColor& color)
{
	HRESULT hr = mShader->SetFloatArray( reinterpret_cast<D3DXHANDLE>(param), color.c, 4 );
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetFloat(const VCNShaderParameter& param, float f)
{
	HRESULT hr = mShader->SetFloat( reinterpret_cast<D3DXHANDLE>(param), f );
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetMatrix(const VCNShaderParameter& param, const Matrix4& mat)
{
	HRESULT hr = mShader->SetFloatArray( reinterpret_cast<D3DXHANDLE>(param), mat.m, 16 );
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetTexture(const VCNShaderParameter& param, const VCNResID textureID)
{
	// Go get the texture resource
	VCND3D9Texture* pTexRes = VCNResourceCore::GetInstance()->GetResource<VCND3D9Texture>( textureID );
	HRESULT hr = mShader->SetTexture( reinterpret_cast<D3DXHANDLE>(param), pTexRes->GetPointer() );
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::Begin()
{
	UINT passes;
	HRESULT hr = mShader->Begin(&passes, NULL);
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::BeginPass(int pass)
{
	HRESULT hr = mShader->BeginPass(pass);
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::EndPass()
{
	HRESULT hr = mShader->EndPass();
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::End()
{
	HRESULT hr = mShader->End();
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetTechnique(const char* name)
{
	mTechnique = mShader->GetTechniqueByName( name );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::BindIndexCache(VCNCache* indexBuffer)
{
	GetD3DDevice()->SetIndices( indexBuffer->Cast<VCND3D9Cache>()->GetIndexBuffer() );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::BindVertexCache(VCNUInt streamSource, VCNCache* vertexBuffer, VCNUInt offsetInBytes, VCNUInt stride)
{
	GetD3DDevice()->SetStreamSource( streamSource, vertexBuffer->Cast<VCND3D9Cache>()->GetVertexBuffer(), offsetInBytes, stride );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::CommitChanges()
{
	HRESULT hr = mShader->CommitChanges();
	VCN_ASSERT( SUCCEEDED(hr) );
}

///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetInt(const VCNShaderParameter& param, int value)
{
	HRESULT hr = mShader->SetInt( reinterpret_cast<D3DXHANDLE>(param), value );
	VCN_ASSERT( SUCCEEDED(hr) );
}

///////////////////////////////////////////////////////////////////////
void VCND3D9Shader::SetMatrixArray(const VCNShaderParameter& param, const std::vector<Matrix4>& mats, std::size_t count)
{
	VCN_ASSERT( mats.size() >= count );
	HRESULT hr = mShader->SetMatrixArray( reinterpret_cast<D3DXHANDLE>(param), (const D3DXMATRIX*)&mats[0].m, count );
	VCN_ASSERT( SUCCEEDED(hr) );
}
