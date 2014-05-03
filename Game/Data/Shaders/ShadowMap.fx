///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Draws a shadow map
///

#include "ParamPool.fx"

//
// Uniform variables
//

// Maximum depth value in the scene
// NOTE: Must be set to 1.0f for orthographic projections
float gMaxDepth;

// Diffuse color texture
texture gDiffuseTexture;

//--------------------------------------------------------------------------------------
/// Texture samplers
//--------------------------------------------------------------------------------------
sampler DiffuseTextureSampler = 
  sampler_state
{
  Texture = <gDiffuseTexture>;
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
  AddressU  = CLAMP;
  AddressV  = CLAMP;
};

//
// Input / Output structures
//
struct VS_IN
{
  float4 Position   : POSITION;
  float2 TexUV      : TEXCOORD0;
};

struct VS_OUT
{
  float4 Position   : POSITION;
  float2 TexUV      : TEXCOORD0;
  float  Depth      : TEXCOORD1;
};

//
// Vertex shader
//
VS_OUT ShadowMapVertexShader(VS_IN In)
{
  VS_OUT Output = (VS_OUT)0;

  // NOTE: gWorldViewProjection stores the light WVP matrix.
  Output.Position = mul(In.Position, gWorldViewProjection);
  //Output.Position.y -= 0.005f;
  Output.TexUV    = In.TexUV;
  Output.Depth    = Output.Position.z / Output.Position.w /*/ gMaxDepth*/;

  return Output;    
}

//
// Pixel shader
//
float4 ShadowMapPixelShader(VS_OUT In) : COLOR0
{
  float4 Output = (float4)0;

  Output.r = In.Depth;
  Output.g = In.Depth * In.Depth;

  // Uses alpha testing to ignore transparent texels
  Output.a = tex2D( DiffuseTextureSampler, In.TexUV ).a;

  return Output;
}

technique BaseTechnique
{
  pass P0
  {
    VertexShader    = compile vs_2_0 ShadowMapVertexShader();
    PixelShader     = compile ps_2_0 ShadowMapPixelShader();

    ZEnable         = True;
    ZWriteEnable    = True;
    ZFunc           = Less;
    CullMode        = None;
    AlphaTestEnable = TRUE;
    AlphaFunc       = Greater;
    AlphaRef        = 0x20; 
  }
}
