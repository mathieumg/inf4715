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

#include "RendererConfig.h"

#if defined(RENDERER_ENABLE_OPENGL)

#include "OGLRendererMaterial.h"
#include "OGLRendererTexture2D.h"

#include <RendererMaterialDesc.h>

#include <SamplePlatform.h>

#include <stdio.h>

// for PsString.h
namespace physx
{
	namespace string
	{}
}
#include <PsString.h>
#include <PsFile.h>

using namespace SampleRenderer;

#if defined(RENDERER_ENABLE_CG)
	
static RendererMaterial::VariableType getVariableType(CGtype cgt)
{
	RendererMaterial::VariableType vt = RendererMaterial::NUM_VARIABLE_TYPES;
	switch(cgt)
	{
		case CG_FLOAT:     vt = RendererMaterial::VARIABLE_FLOAT;     break;
		case CG_FLOAT2:    vt = RendererMaterial::VARIABLE_FLOAT2;    break;
		case CG_FLOAT3:    vt = RendererMaterial::VARIABLE_FLOAT3;    break;
		case CG_FLOAT4:    vt = RendererMaterial::VARIABLE_FLOAT4;    break;
		case CG_FLOAT4x4:  vt = RendererMaterial::VARIABLE_FLOAT4x4;  break;
		case CG_INT:       vt = RendererMaterial::VARIABLE_INT;       break;
		case CG_SAMPLER2D: vt = RendererMaterial::VARIABLE_SAMPLER2D; break;
	}
	RENDERER_ASSERT(vt < RendererMaterial::NUM_VARIABLE_TYPES, "Unable to convert shader parameter type.");
	return vt;
}

static GLuint getGLBlendFunc(RendererMaterial::BlendFunc func)
{
	GLuint glfunc = 0;
	switch(func)
	{
		case RendererMaterial::BLEND_ZERO:                glfunc = GL_ZERO;                break;
		case RendererMaterial::BLEND_ONE:                 glfunc = GL_ONE;                 break;
		case RendererMaterial::BLEND_SRC_COLOR:           glfunc = GL_SRC_COLOR;           break;
		case RendererMaterial::BLEND_ONE_MINUS_SRC_COLOR: glfunc = GL_ONE_MINUS_SRC_COLOR; break;
		case RendererMaterial::BLEND_SRC_ALPHA:           glfunc = GL_SRC_ALPHA;           break;
		case RendererMaterial::BLEND_ONE_MINUS_SRC_ALPHA: glfunc = GL_ONE_MINUS_SRC_ALPHA; break;
		case RendererMaterial::BLEND_DST_ALPHA:           glfunc = GL_DST_COLOR;           break;
		case RendererMaterial::BLEND_ONE_MINUS_DST_ALPHA: glfunc = GL_ONE_MINUS_DST_ALPHA; break;
		case RendererMaterial::BLEND_DST_COLOR:           glfunc = GL_DST_COLOR;           break;
		case RendererMaterial::BLEND_ONE_MINUS_DST_COLOR: glfunc = GL_ONE_MINUS_DST_COLOR; break;
		case RendererMaterial::BLEND_SRC_ALPHA_SATURATE:  glfunc = GL_SRC_ALPHA_SATURATE;  break;
	}
	RENDERER_ASSERT(glfunc, "Unable to convert Material Blend Func.");
	return glfunc;
}

static void connectParameters(CGparameter from, CGparameter to)
{
	if(from && to) cgConnectParameter(from, to);
}

static void connectEnvParameters(const OGLRenderer::CGEnvironment &cgEnv, CGprogram program)
{
	connectParameters(cgEnv.modelMatrix,         cgGetNamedParameter(program, "g_" "modelMatrix"));
	connectParameters(cgEnv.viewMatrix,          cgGetNamedParameter(program, "g_" "viewMatrix"));
	connectParameters(cgEnv.projMatrix,          cgGetNamedParameter(program, "g_" "projMatrix"));
	connectParameters(cgEnv.modelViewMatrix,     cgGetNamedParameter(program, "g_" "modelViewMatrix"));
	connectParameters(cgEnv.modelViewProjMatrix, cgGetNamedParameter(program, "g_" "modelViewProjMatrix"));


#if !defined(RENDERER_PS3)
	connectParameters(cgEnv.boneMatrices,         cgGetNamedParameter(program, "g_" "boneMatrices"));
#endif		
	connectParameters(cgEnv.eyePosition,         cgGetNamedParameter(program, "g_" "eyePosition"));
	connectParameters(cgEnv.eyeDirection,        cgGetNamedParameter(program, "g_" "eyeDirection"));

	connectParameters(cgEnv.fogColorAndDistance, cgGetNamedParameter(program, "g_" "fogColorAndDistance"));

	connectParameters(cgEnv.ambientColor,        cgGetNamedParameter(program, "g_" "ambientColor"));
	
	connectParameters(cgEnv.lightColor,          cgGetNamedParameter(program, "g_" "lightColor"));
	connectParameters(cgEnv.lightIntensity,      cgGetNamedParameter(program, "g_" "lightIntensity"));
	connectParameters(cgEnv.lightDirection,      cgGetNamedParameter(program, "g_" "lightDirection"));
	connectParameters(cgEnv.lightPosition,       cgGetNamedParameter(program, "g_" "lightPosition"));
	connectParameters(cgEnv.lightInnerRadius,    cgGetNamedParameter(program, "g_" "lightInnerRadius"));
	connectParameters(cgEnv.lightOuterRadius,    cgGetNamedParameter(program, "g_" "lightOuterRadius"));
	connectParameters(cgEnv.lightInnerCone,      cgGetNamedParameter(program, "g_" "lightInnerCone"));
	connectParameters(cgEnv.lightOuterCone,      cgGetNamedParameter(program, "g_" "lightOuterCone"));
}

OGLRendererMaterial::CGVariable::CGVariable(const char *name, VariableType type, PxU32 offset) :
	Variable(name, type, offset)
{
	m_vertexHandle = 0;
	memset(m_fragmentHandles, 0, sizeof(m_fragmentHandles));
}

OGLRendererMaterial::CGVariable::~CGVariable(void)
{
	
}

void OGLRendererMaterial::CGVariable::addVertexHandle(CGparameter handle)
{
	m_vertexHandle = handle;
}

void OGLRendererMaterial::CGVariable::addFragmentHandle(CGparameter handle, Pass pass)
{
	m_fragmentHandles[pass] = handle;
}

#endif

OGLRendererMaterial::OGLRendererMaterial(OGLRenderer &renderer, const RendererMaterialDesc &desc) :
	RendererMaterial(desc),
	m_renderer(renderer)
{
	m_glAlphaTestFunc = GL_ALWAYS;
	
	AlphaTestFunc alphaTestFunc = getAlphaTestFunc();
	switch(alphaTestFunc)
	{
		case ALPHA_TEST_ALWAYS:        m_glAlphaTestFunc = GL_ALWAYS;   break;
		case ALPHA_TEST_EQUAL:         m_glAlphaTestFunc = GL_EQUAL;    break;
		case ALPHA_TEST_NOT_EQUAL:     m_glAlphaTestFunc = GL_NOTEQUAL; break;
		case ALPHA_TEST_LESS:          m_glAlphaTestFunc = GL_LESS;     break;
		case ALPHA_TEST_LESS_EQUAL:    m_glAlphaTestFunc = GL_LEQUAL;   break;
		case ALPHA_TEST_GREATER:       m_glAlphaTestFunc = GL_GREATER;  break;
		case ALPHA_TEST_GREATER_EQUAL: m_glAlphaTestFunc = GL_GEQUAL;   break;
		default:
			RENDERER_ASSERT(0, "Unknown Alpha Test Func.");
	}
	
#if defined(RENDERER_ENABLE_CG)
	m_vertexProgram   = 0;
#if defined(RENDERER_PS3)
	m_vertexProfile   = CG_PROFILE_SCE_VP_RSX; //cgGLGetLatestProfile(CG_GL_VERTEX); // CG_PROFILE_GPU_VP FAILS SO BAD!
	m_fragmentProfile = CG_PROFILE_SCE_FP_RSX; //cgGLGetLatestProfile(CG_GL_FRAGMENT); // CG_PROFILE_GPU_VP FAILS SO BAD!
#else
	m_vertexProfile   = CG_PROFILE_ARBVP1; //cgGLGetLatestProfile(CG_GL_VERTEX); // CG_PROFILE_GPU_VP FAILS SO BAD!
	m_fragmentProfile = CG_PROFILE_ARBFP1; //cgGLGetLatestProfile(CG_GL_FRAGMENT); // CG_PROFILE_GPU_VP FAILS SO BAD!
#define NO_SUPPORT_DDX_DDY
#endif
	memset(m_fragmentPrograms, 0, sizeof(m_fragmentPrograms));
	
	CGcontext                         cgContext = m_renderer.getCGContext();
	const OGLRenderer::CGEnvironment &cgEnv     = m_renderer.getCGEnvironment();
	if(cgContext && m_vertexProfile && m_fragmentProfile)
	{
		char fullPath[1024] = "-I";
		physx::string::strcat_s(fullPath, 1024, m_renderer.getAssetDir());
		physx::string::strcat_s(fullPath, 1024, "shaders/include");
		const char *vertexEntry = "vmain";
		const char *vertexArgs[] =
		{
			fullPath,
			"-DRENDERER_VERTEX",
#ifdef RENDERER_PS3
			"-DRENDERER_PS3",
			"-melf",
#endif
			0, 0,
		};		

		cgGLSetOptimalOptions(m_vertexProfile);
		if(1)
		{
			RENDERER_PERFZONE(OGLRendererMaterial_compile_vertexProgram);
			m_vertexProgram = static_cast<CGprogram>(SampleFramework::SamplePlatform::platform()->compileProgram(cgContext, m_renderer.getAssetDir(), desc.vertexShaderPath, m_vertexProfile, 0, vertexEntry, vertexArgs));
		}
		RENDERER_ASSERT(m_vertexProgram, "Failed to compile Vertex Shader.");
		if(m_vertexProgram)
		{
			RENDERER_PERFZONE(OGLRendererMaterial_load_vertexProgram);
			connectEnvParameters(cgEnv, m_vertexProgram);
			cgGLLoadProgram(m_vertexProgram);
			loadCustomConstants(m_vertexProgram, NUM_PASSES);
		}
		else
		{
			char msg[1024];
			physx::string::sprintf_s(msg, sizeof(msg), "Could not find shader file: <%s> in path: <%sshaders/>", desc.vertexShaderPath, m_renderer.getAssetDir());
			RENDERER_OUTPUT_MESSAGE(&m_renderer, msg);
		}
		
		const char *fragmentEntry = "fmain";
		for(PxU32 i=0; i<NUM_PASSES; i++)
		{
			char passDefine[1024] = {0};
			physx::string::sprintf_s(passDefine, 1023, "-D%s", getPassName((Pass)i));
			const char *fragmentArgs[]  =
			{
				fullPath,
				"-DRENDERER_FRAGMENT",
#ifdef RENDERER_PS3
				"-DRENDERER_PS3",
				"-melf",
				"-DENABLE_VFACE",
#endif
#ifdef NO_SUPPORT_DDX_DDY
				"-DNO_SUPPORT_DDX_DDY",
#endif
				passDefine,
				0, 0,
			};
			cgGLSetOptimalOptions(m_fragmentProfile);
			CGprogram fp = 0;
			if(1)
			{
				RENDERER_PERFZONE(OGLRendererMaterial_compile_fragmentProgram);
				fp = static_cast<CGprogram>(SampleFramework::SamplePlatform::platform()->compileProgram(cgContext, m_renderer.getAssetDir(), desc.fragmentShaderPath, m_fragmentProfile, getPassName((Pass)i), fragmentEntry, fragmentArgs));
			}
			RENDERER_ASSERT(fp, "Failed to compile Fragment Shader.");
			if(fp)
			{
				RENDERER_PERFZONE(OGLRendererMaterial_load_fragmentProgram);
				connectEnvParameters(cgEnv, fp);
				cgGLLoadProgram(fp);
				m_fragmentPrograms[i] = fp;
				loadCustomConstants(fp, (Pass)i);
			}
		}
	}
#endif
}

OGLRendererMaterial::~OGLRendererMaterial(void)
{
#if defined(RENDERER_ENABLE_CG)
	if(m_vertexProgram)
	{
		cgDestroyProgram(m_vertexProgram);
	}
	for(PxU32 i=0; i<NUM_PASSES; i++)
	{
		CGprogram fp = m_fragmentPrograms[i];
		if(fp)
		{
			cgDestroyProgram(fp);
		}
	}
#endif
}

void OGLRendererMaterial::bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const
{
	m_renderer.setCurrentMaterial(this);
	
	if(m_glAlphaTestFunc == GL_ALWAYS)
	{
		glDisable(GL_ALPHA_TEST);
	}
	else
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(m_glAlphaTestFunc, PxClamp(getAlphaTestRef(), 0.0f, 1.0f));
	}
	
	if(getBlending())
	{
		glBlendFunc(getGLBlendFunc(getSrcBlendFunc()), getGLBlendFunc(getDstBlendFunc()));
		glEnable(GL_BLEND);
		glDepthMask(0);
	}

#if defined(RENDERER_ENABLE_CG)
	if(m_vertexProgram)
	{
		cgGLEnableProfile(m_vertexProfile);
		cgGLBindProgram(m_vertexProgram);
#if !defined(RENDERER_PS3)
		cgUpdateProgramParameters(m_vertexProgram);
#endif
	}
	if(pass < NUM_PASSES && m_fragmentPrograms[pass])
	{
		cgGLEnableProfile(m_fragmentProfile);
		cgGLBindProgram(m_fragmentPrograms[pass]);
		
		RendererMaterial::bind(pass, materialInstance, instanced);
		//this is a kludge to make the particles work. I see no way to set env's through this interface
		glEnable(GL_POINT_SPRITE_ARB);
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

#if !defined(RENDERER_PS3)		
		cgUpdateProgramParameters(m_fragmentPrograms[pass]);
#endif
	}
#endif
}

void OGLRendererMaterial::bindMeshState(bool instanced) const
{
#if defined(RENDERER_ENABLE_CG)
	if(m_vertexProgram)
	{
#if defined(RENDERER_PS3)
		cgGLBindProgram(m_vertexProgram);
#else
		cgUpdateProgramParameters(m_vertexProgram);
#endif
	}
#endif
}

void OGLRendererMaterial::unbind(void) const
{
#if defined(RENDERER_ENABLE_CG)
	glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	if(m_vertexProfile)
	{
		cgGLUnbindProgram(m_vertexProfile);
		cgGLDisableProfile(m_vertexProfile);
	}
	if(m_fragmentProfile)
	{
		cgGLUnbindProgram(m_fragmentProfile);
		cgGLDisableProfile(m_fragmentProfile);
	}
#endif

	if(getBlending())
	{
		glDisable(GL_BLEND);
		glDepthMask(1);
	}

	m_renderer.setCurrentMaterial(0);
}

#if defined(RENDERER_ENABLE_CG)
static void bindSampler2DVariable(CGparameter param, RendererTexture2D &texture)
{
	OGLRendererTexture2D &tex = *static_cast<OGLRendererTexture2D*>(&texture);
	if(param)
	{
		CGresource resource = cgGetParameterResource(param);
		RENDERER_ASSERT(resource >= CG_TEXUNIT0 && resource <= CG_TEXUNIT15, "Invalid Texture Resource Location.");
		if(resource >= CG_TEXUNIT0 && resource <= CG_TEXUNIT15)
		{
			tex.bind(resource-CG_TEXUNIT0);
		}
	}
}
#endif

void OGLRendererMaterial::bindVariable(Pass pass, const Variable &variable, const void *data) const
{
#if defined(RENDERER_ENABLE_CG)
	CGVariable &var = *(CGVariable*)&variable;
	switch(var.getType())
	{
		case VARIABLE_FLOAT:
		{
			float f = *(const float*)data;
			if(var.m_vertexHandle)          cgGLSetParameter1f(var.m_vertexHandle,          f);
			if(var.m_fragmentHandles[pass]) cgGLSetParameter1f(var.m_fragmentHandles[pass], f);
			break;
		}
		case VARIABLE_FLOAT2:
		{
			const float *f = (const float*)data;
			if(var.m_vertexHandle)          cgGLSetParameter2fv(var.m_vertexHandle,          f);
			if(var.m_fragmentHandles[pass]) cgGLSetParameter2fv(var.m_fragmentHandles[pass], f);
			break;
		}
		case VARIABLE_FLOAT3:
		{
			const float *f = (const float*)data;
			if(var.m_vertexHandle)          cgGLSetParameter3fv(var.m_vertexHandle,          f);
			if(var.m_fragmentHandles[pass]) cgGLSetParameter3fv(var.m_fragmentHandles[pass], f);
			break;
		}
		case VARIABLE_FLOAT4:
		{
			const float *f = (const float*)data;
			if(var.m_vertexHandle)          cgGLSetParameter4fv(var.m_vertexHandle,          f);
			if(var.m_fragmentHandles[pass]) cgGLSetParameter4fv(var.m_fragmentHandles[pass], f);
			break;
		}
		case VARIABLE_SAMPLER2D:
			data = *(void**)data;
			RENDERER_ASSERT(data, "NULL Sampler.");
			if(data)
			{
				bindSampler2DVariable(var.m_vertexHandle,          *(RendererTexture2D*)data);
				bindSampler2DVariable(var.m_fragmentHandles[pass], *(RendererTexture2D*)data);
			}
			break;
	}
#endif
}

#if defined(RENDERER_ENABLE_CG)
void OGLRendererMaterial::loadCustomConstants(CGprogram program, Pass pass)
{
	for(CGparameter param = cgGetFirstParameter(program, CG_GLOBAL); param; param=cgGetNextParameter(param))
	{
		const char  *name   = cgGetParameterName(param);
		CGtype       cgtype = cgGetParameterType(param);
		VariableType type   = cgtype != CG_STRUCT && cgtype != CG_ARRAY ? getVariableType(cgtype) : NUM_VARIABLE_TYPES;
		if(type < NUM_VARIABLE_TYPES && cgIsParameter(param) && cgIsParameterReferenced(param) && strncmp(name, "g_", 2))
		{
			CGVariable *var = 0;
			// find existing variable...
			PxU32 numVariables = (PxU32)m_variables.size();
			for(PxU32 i=0; i<numVariables; i++)
			{
				if(!strcmp(m_variables[i]->getName(), name))
				{
					var = static_cast<CGVariable*>(m_variables[i]);
					break;
				}
			}
			// check to see if the variable is of the same type.
			if(var)
			{
				RENDERER_ASSERT(var->getType() == type, "Variable changes type!");
			}
			// if the variable was not found, add it.
			if(!var)
			{
				var = new CGVariable(name, type, m_variableBufferSize);
				m_variables.push_back(var);
				m_variableBufferSize += var->getDataSize();
			}
			if(pass < NUM_PASSES)
			{
				var->addFragmentHandle(param, pass);
			}
			else
			{
				var->addVertexHandle(param);
			}
		}
	}
}
#endif

#endif // #if defined(RENDERER_ENABLE_OPENGL)
