/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D9)

#include "D3D9RendererSpotLight.h"
#include "D3D9RendererTexture2D.h"

using namespace SampleRenderer;

D3D9RendererSpotLight::D3D9RendererSpotLight(D3D9Renderer &renderer, const RendererSpotLightDesc &desc) :
	RendererSpotLight(desc),
m_renderer(renderer)
{

}

D3D9RendererSpotLight::~D3D9RendererSpotLight(void)
{

}

void D3D9RendererSpotLight::bind(void) const
{
	D3D9Renderer::ShaderEnvironment &shaderEnv = m_renderer.getShaderEnvironment();
	convertToD3D9(shaderEnv.lightColor, m_color);
	shaderEnv.lightIntensity = m_intensity;
	convertToD3D9(shaderEnv.lightPosition,  m_position);
	convertToD3D9(shaderEnv.lightDirection, m_direction);
	shaderEnv.lightInnerRadius = m_innerRadius;
	shaderEnv.lightOuterRadius = m_outerRadius;
	shaderEnv.lightInnerCone   = m_innerCone;
	shaderEnv.lightOuterCone   = m_outerCone;
	shaderEnv.lightShadowMap   = m_shadowMap ? static_cast<D3D9RendererTexture2D*>(m_shadowMap)->m_d3dTexture : 0;
	buildProjectMatrix(shaderEnv.lightShadowMatrix, m_shadowProjection, m_shadowTransform);
}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9)
