///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Shader resource base class 
///

#ifndef VICUNA_DIRECTX_SHADER
#define VICUNA_DIRECTX_SHADER

#pragma once

#include "D3D9.h"

#include "VCNUtils/Types.h"
#include "VCNRenderer/Shader.h"

class VCNDXMesh;
class VCNDeclarationD3D9;

//-------------------------------------------------------------
/// The texture class
//-------------------------------------------------------------
class VCND3D9Shader : public VCNShader
{
	VCN_CLASS;

	friend class VCNDXShaderCore;

public:

	/// Create an empty shader
	VCND3D9Shader();

	/// Create a shader with an effect
	explicit VCND3D9Shader(LPD3DXEFFECT effect);

	/// Destructs the shader
	virtual ~VCND3D9Shader();

	/// Sets the shader as the active shader
	virtual VCNShaderParameter GetParameterByName(const char* name) const override;
	virtual void Begin() override;
	virtual void BeginPass(int pass) override;
	virtual void Bind() override;
	virtual void BindIndexCache(VCNCache* indexBuffer) override;
	virtual void BindVertexCache(VCNUInt streamSource, VCNCache* vertexBuffer, VCNUInt offsetInBytes, VCNUInt stride) override;
	virtual void CommitChanges() override;
	virtual void CreateVertexDeclaration(const VCNDeclarationElementList& list) override;
	virtual void End() override;
	virtual void EndPass() override;
	virtual void RenderIndexedPrimitive(VCNPrimitiveType primitiveType, VCNInt baseVertexIndex, VCNUInt minVertexIndex, VCNUInt numVertices, VCNUInt startIndex, VCNUInt primCount) override;
	virtual void RenderIndexedPrimitive(VCNUInt faceCount, VCNUInt vertexCount, VCNInt primitiveType = D3DPT_TRIANGLELIST) override;
	virtual void RenderPrimitive(VCNUInt faceCount, VCNInt primitiveType = D3DPT_TRIANGLELIST) override;
	virtual void SetColor(const VCNShaderParameter& param, const VCNColor& color) override;
	virtual void SetInt(const VCNShaderParameter& param, int i) override;
	virtual void SetFloat(const VCNShaderParameter& param, float f) override;
	virtual void SetMatrix(const VCNShaderParameter& param, const Matrix4& mat) override;
	virtual void SetMatrixArray(const VCNShaderParameter& param, const std::vector<Matrix4>& matrices, std::size_t count) override;
	virtual void SetTechnique(const char* name) override;
	virtual void SetTexture(const VCNShaderParameter& param, const VCNResID textureID) override;

	/// Helper function to get the device
	LPDIRECT3DDEVICE9 GetD3DDevice() const { return ((VCND3D9*)VCND3D9::GetInstance())->GetD3DDevice();  }

	/// Returns the shading effect for quick access inside DX core
	LPD3DXEFFECT GetEffect() const { return mShader; }

protected:

	// The shader itself
	LPD3DXEFFECT mShader;
	D3DXHANDLE   mTechnique;

	// The vertex declaration needed by the shader
	VCNDeclarationD3D9* mVertexDeclaration;
};

//-------------------------------------------------------------

#endif
