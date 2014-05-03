///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "ShaderUtils.fx"
#include "ParamPool.fx"
#include "LightingFunctions.fx"
#include "SkinningFunctions.fx"
#include "Selection.fx"

//--------------------------------------------------------------------------------------
/// Global variables
//--------------------------------------------------------------------------------------

// Textures
texture  gDiffuseTexture;            // Diffuse color texture
texture  gShadowMapTexture;

// Shadow constants
float    gMaxDepth;                  // If light view is orthographic set to 1
float4x4 gLightWorldViewProjection;

// Skinning
uniform int gCurNumBones = 2;

//--------------------------------------------------------------------------------------
/// Texture samplers
//--------------------------------------------------------------------------------------
sampler DiffuseTextureSampler = sampler_state
{
    Texture = <gDiffuseTexture>;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};

sampler ShadowMapTextureSampler = sampler_state
{
    Texture = <gShadowMapTexture>;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_INPUT
{
  float4 vPosition        : POSITION;
  float3 vNormal          : NORMAL;
  float3 vDiffuseColor    : COLOR0;
  float2 vTexCoord        : TEXCOORD0;
  float4 vBlendWeights    : BLENDWEIGHT;
  float4 vBlendIndices    : BLENDINDICES;
};

struct PS_INPUT
{
  float4 Position2D           : POSITION;
  float3 Position             : TEXCOORD0; // World position
  float3 Normal               : TEXCOORD1; // World normal orientation
  float3 Diffuse              : TEXCOORD2; // Material diffuse color
  float2 DiffuseUV            : TEXCOORD3; // Texture UV
  float3 ViewDir              : TEXCOORD4; // Camera view direction
  float4 ShadowMapSamplingPos : TEXCOORD5;
  float4 RealDistance         : TEXCOORD6;
};

//--------------------------------------------------------------------------------------
PS_INPUT LitTexturedSkinnedVS( VS_INPUT In )
{
  PS_INPUT output;

  if (gCurNumBones > 0)
  {
    ApplySkinning(In.vPosition, In.vNormal, In.vBlendWeights, In.vBlendIndices, gCurNumBones);
  }
    
  // Apply transforms for the out position
  output.Position2D = mul( In.vPosition, gWorldViewProjection );

  // Fill the params that need values
  output.Position = In.vPosition.xyz / In.vPosition.w;
  output.Normal   = In.vNormal;
  output.Diffuse  = In.vDiffuseColor;
  
  // Apply the vertex shader lighting calculations
  SetVertexShaderValues(output.Position, output.Normal);

  // Get the vector from the camera to the vertex for the specular component by
  // subtracting the world position from the camera
  output.ViewDir = gViewPosition - output.Position;
  
  output.ShadowMapSamplingPos = mul(In.vPosition, gLightWorldViewProjection);
  output.RealDistance = output.ShadowMapSamplingPos.z/*/gMaxDepth*/;

  // Copy the set of UVs for diffuse texture
  output.DiffuseUV = In.vTexCoord;

  return output;    
}

//--------------------------------------------------------------------------------------
float4 LitTexturedSkinnedPS( PS_INPUT In ) : COLOR0
{  
  // Call the lighting function and return the color!
  const float3 normal = normalize(In.Normal);
  const float3 viewDir = normalize(In.ViewDir);
    
  // Find out our overall diffuse color
  float4 textureColor = tex2D( DiffuseTextureSampler, In.DiffuseUV );
  float4 litColor = GetLitPixelColor( In.Position, viewDir, normal, textureColor.rgb );

  // Apply shadow effect
  if( gLightTypeDirectional )
  {
    // Transform the surface into light space and project
    // NB: Could be done in the vertex shader, but doing it here keeps the "light
    // shader" abstraction and doesn't limit # of shadowed lights.
    float4 surf_tex = In.ShadowMapSamplingPos;
    surf_tex = surf_tex / surf_tex.w;

    // Rescale viewport to be [0,1] (texture coordinate space)
    float2 shadow_tex = surf_tex.xy * float2(0.5, -0.5) + 0.5;

    float4 moments = BlurX6(ShadowMapTextureSampler, shadow_tex, gLightQuadAttenuation);

    // Rescale light distance and check if we're in shadow
    // NOTE: gLightConstantAttenuation is used as a bias and not as a constant attenuation.
    float rescaled_dist_to_light = In.RealDistance.x - gLightConstantAttenuation;
    float lit_factor = (rescaled_dist_to_light <= moments.x);

    // Variance shadow mapping
    // NOTE: gLightLinearAttenuation is used as a bias and not as a linear attenuation.
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = min(max(E_x2 - Ex_2, 0.0) + gLightLinearAttenuation, 1.0);
    float m_d = (moments.x - rescaled_dist_to_light);
    float p = variance / (variance + m_d * m_d);

    // Adjust the light color based on the shadow attenuation
    litColor *= max(lit_factor, max(p, 0.25f));
  }

  return float4(litColor.rgb, textureColor.a) * gFadeAmount;
}

//--------------------------------------------------------------------------------------
/// Techniques available to the Vicuna Renderer
//--------------------------------------------------------------------------------------
technique BaseTechnique
{ 
  pass Selection
  {
    VertexShader    = compile vs_3_0 SelectionVS();
    PixelShader     = compile ps_3_0 SelectionPS();

    CullMode = CCW;
    AlphaTestEnable = TRUE;
    AlphaFunc       = Greater;
    AlphaRef        = 0x000080; 
  }

  pass BasePass
  {
    VertexShader = compile vs_3_0 LitTexturedSkinnedVS();
    PixelShader  = compile ps_3_0 LitTexturedSkinnedPS();

    CullMode = CCW;
    AlphaTestEnable = TRUE;
    AlphaFunc = Greater;
    AlphaRef = 0x000080; 
  }
}
