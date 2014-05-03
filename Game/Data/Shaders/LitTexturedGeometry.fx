///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Render geometry with lighting effects only.
/// 

#include "ShaderUtils.fx"
#include "ParamPool.fx"
#include "LightingFunctions.fx"
#include "Selection.fx"

///
/// Global variables
///

// Diffuse color texture
texture   gDiffuseTexture;    

// Shadow constants
float     gMaxDepth;                  // If light view is orthographic set to 1
float4x4  gLightWorldViewProjection;
texture   gShadowMapTexture;

///
/// Texture samplers
///
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

///
/// Input/Output structures
///

struct VS_IN
{
  float4 Position     : POSITION;
  float3 Normal       : NORMAL;
  float3 DiffuseColor : COLOR0;
  float2 TexCoord     : TEXCOORD0;
};

struct PS_IN
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

///
/// Vertex shader
///
PS_IN LitTexturedGeometryVS( VS_IN In )
{
  PS_IN Output;

  // Apply transforms for the out position
  Output.Position2D = mul( In.Position, gWorldViewProjection );

  // Fill the params that need values
  Output.Position = In.Position.xyz / In.Position.w;
  Output.Normal   = In.Normal;
  Output.Diffuse  = In.DiffuseColor;
  
  // Apply the vertex shader lighting calculations
  SetVertexShaderValues(Output.Position, Output.Normal);

  // Get the vector from the camera to the vertex for the specular component by
  // subtracting the world position from the camera
  Output.ViewDir = gViewPosition - Output.Position;
  
  Output.ShadowMapSamplingPos = mul(In.Position, gLightWorldViewProjection);
  Output.RealDistance = Output.ShadowMapSamplingPos.z/*/gMaxDepth*/;

  // Copy the set of UVs for diffuse texture
  Output.DiffuseUV = In.TexCoord;

  return Output;    
}

///
/// Pixel shader
///
float4 LitTexturedGeometryPS( PS_IN In ) : COLOR0
{ 
  // Find out our overall diffuse color
  float4 textureColor = tex2D( DiffuseTextureSampler, In.DiffuseUV );

  // DEBUG: Check diffuse color
  // return float4(textureColor.rgb, 1.0f);

  // DEBUG: Check light hit (additive blending)
  // return float4(0.25f, 0, 0, 1);
  
  // Call the lighting function and return the color!
  const float3 normal = normalize(In.Normal);
  const float3 viewDir = normalize(In.ViewDir);

  float3 litColorBefore = textureColor.rgb * gLightDiffuseColor;
  float4 litColor = GetLitPixelColor( In.Position, viewDir, normal, litColorBefore );

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

///
/// Technique
///
technique BaseTechnique
{
  pass Selection
  {
    VertexShader    = compile vs_3_0 SelectionVS();
    PixelShader     = compile ps_3_0 SelectionPS();

    CullMode = CW;
    AlphaTestEnable = TRUE;
    AlphaFunc       = Greater;
    AlphaRef        = 0x000080; 
  }

  pass BasePass
  {
    VertexShader    = compile vs_3_0 LitTexturedGeometryVS();
    PixelShader     = compile ps_3_0 LitTexturedGeometryPS();

    CullMode        = CCW;
    AlphaTestEnable = TRUE;
    AlphaFunc       = Greater;
    AlphaRef        = 0x000080; 
  }
}
