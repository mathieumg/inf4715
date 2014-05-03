///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Basic Lit and Textured Effect.
///

#ifndef VCND3D9_LITTEXSKINNEDFX_H
#define VCND3D9_LITTEXSKINNEDFX_H

#pragma once

#include "D3D9_LightingFX.h"
#include "VCND3D9/D3D9Effect.h"

#include "VCNUtils/Matrix.h"
#include "VCNUtils/SmartTypes.h"
#include "D3D9_SelectionFX.h"

// Forward declarations
class VCNLight;

class VCND3D9_LitTexturedSkinnedFX : public VCND3D9Effect
{
public:

	/// Constructs the effect
	VCND3D9_LitTexturedSkinnedFX();

	/// Destruct the effect
	virtual ~VCND3D9_LitTexturedSkinnedFX();

	/// Sets the shadow map that will be used to lit objects
	void SetShadowMapTexture(LPDIRECT3DTEXTURE9 texture);

	/// Prepare the effect frame information
	virtual void Prepare() override;

	/// Prepare render streams
	virtual VCNBool PrepareRender(const class VCNMesh* mesh, class VCND3D9Shader* shader) override;

	/// Render the current mesh with this effect
	virtual void RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams) override;

protected:

	/// Compute lighting for the current mesh
	void PerformLightingPasses(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams);

private:

	// Shader
	VCNShaderHandle         mShader;

	// Effect global parameters
	VCND3D9_LightingFX      mLightingFunctions;
    VCND3D9_SelectionFX     mSelectionFunctions;

	VCNShaderParameter      mMaterialAmbientColor;
	VCNShaderParameter      mMaterialDiffuseColor;
	VCNShaderParameter      mMaterialSpecularColor;
	VCNShaderParameter      mMaterialSpecularPower;
	VCNShaderParameter      mMaterialDiffuseTexture;

	// Skinning handles
	VCNShaderParameter      m_MatrixArray;
	VCNShaderParameter      m_NumBones;

	// Parameters for shadowing
	VCNShaderParameter      mMaxDepth;
	VCNShaderParameter      mLightMVP;
	VCNShaderParameter      mShadowMapTexture;

	// Lights lighting the current mesh
	std::vector<VCNLight*>  mLights;

	// Shadow mapping info
	Matrix4                 mLightViewProjectionMatrix;
};

#endif // VCND3D9_LITTEXSKINNEDFX_H
