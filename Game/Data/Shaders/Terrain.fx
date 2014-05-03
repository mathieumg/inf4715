///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Renders the terrain multiple textures
/// 

#include "ParamPool.fx"
#include "ShaderUtils.fx"
#include "LightingFunctions.fx"

//
// Global variables
//

// Textures
texture gGrassTexture;
texture gSandTexture;
texture gSnowTexture;
texture gGrassRockTexture;
texture gSnowRockTexture;

// Texture blending parameters
float gSnowHeight;
float gSandHeight;
float gRockCosAngle;
float gHeightTransition;
float gCosAngleTransition;

// Shadow constants
float     gMaxDepth;
float4x4  gLightWorldViewProjection;
texture   gShadowMapTexture;

//
// Texture samplers
//
sampler GrassTextureSampler = sampler_state
{
  Texture = <gGrassTexture>;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

sampler SandTextureSampler = sampler_state
{
  Texture = <gSandTexture>;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

sampler SnowTextureSampler = sampler_state
{
  Texture = <gSnowTexture>;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

sampler GrassRockTextureSampler = sampler_state
{
  Texture = <gGrassRockTexture>;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

sampler SnowRockTextureSampler = sampler_state
{
  Texture = <gSnowRockTexture>;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

sampler ShadowMapTextureSampler = sampler_state
{
  Texture = <gShadowMapTexture>;
  AddressU  = CLAMP;
  AddressV  = CLAMP;
  MagFilter = LINEAR;
  MinFilter = LINEAR;
  MipFilter = LINEAR;
};

//
// Input/output structures
// 

struct VS_IN
{
  float4 Position      : POSITION;
  float3 Normal        : NORMAL;
  float3 DiffuseColor  : COLOR0;
  float2 TexCoord      : TEXCOORD0;
};

struct PS_IN
{
  float4 oPosition            : POSITION;
  float3 Position             : TEXCOORD0; // World position
  float3 Normal               : TEXCOORD1; // World normal orientation
  float3 Diffuse              : TEXCOORD2; // Material diffuse color
  float2 DiffuseUV            : TEXCOORD3; // Texture UV
  float3 ViewDir              : TEXCOORD4; // Camera view direction
  float4 ShadowMapSamplingPos : TEXCOORD5;
  float RealDistance          : TEXCOORD6;
};

//
// Vertex shader
//
PS_IN LitTexturedGeometryVS(VS_IN In)
{
  PS_IN Output;

  // Apply transforms for the out position
  Output.oPosition = mul( In.Position, gWorldViewProjection );

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

//
// Pixel shader
//
float4 LitTexturedGeometryPS( PS_IN In ) : COLOR0
{ 
  // Call the lighting function and return the color!
  const float3 normal = normalize(In.Normal);
  const float3 viewDir = normalize(In.ViewDir);

  float halfHeightTransition = gHeightTransition/2.0f;
  float halfCosAngleTransition = gCosAngleTransition/2.0f;

  float snow = 0.0f, grass = 0.0f, sand = 0.0f, rock = 0.0f;

  float dotNormalUp = dot(normal, float3(0.0f, 1.0f, 0.0f));

  if(dotNormalUp <= gRockCosAngle - halfCosAngleTransition)
  {
    rock = 1.0f;
  } 
  else if(dotNormalUp <  gRockCosAngle + halfCosAngleTransition && dotNormalUp > gRockCosAngle - halfCosAngleTransition)
  {
    rock = 1.0f - smoothstep(gRockCosAngle - halfCosAngleTransition, gRockCosAngle + halfCosAngleTransition, dotNormalUp);
  }

  if(In.Position.y > halfHeightTransition + gSnowHeight)
  {
    snow = 1.0f;
  }
  else if(In.Position.y >= gSnowHeight - halfHeightTransition)
  {
    snow = smoothstep(gSnowHeight - halfHeightTransition, gSnowHeight + halfHeightTransition, In.Position.y);
  }
  else if(In.Position.y >= gSandHeight - halfHeightTransition)
  {
    sand =  1.0f - smoothstep(gSandHeight - halfHeightTransition, gSandHeight + halfHeightTransition, In.Position.y);
  }  
  else
  {
    sand = 1.0f;
  }

  float4 textureGrass = tex2D( GrassTextureSampler, In.DiffuseUV );
  float4 textureSnow = tex2D( SnowTextureSampler, In.DiffuseUV );
  float4 textureSand = tex2D( SandTextureSampler, In.DiffuseUV );
  float4 textureRock = tex2D( GrassRockTextureSampler, In.DiffuseUV );
  float4 textureSnowRock = tex2D( SnowRockTextureSampler, In.DiffuseUV );

  // Find out our overall diffuse color
  grass = 1.0f - sand - snow;
  float4 textureColor = ((1.0f-rock) * (grass * textureGrass + textureSnow * snow + textureSand * sand))
    + (rock * (grass * textureRock + textureSnowRock * snow + textureRock * sand));
  float3 diffuseColor = textureColor.rgb;

  float4 litColor = GetLitPixelColor( In.Position, viewDir, normal, diffuseColor );

  return float4(litColor.rgb, 1.0f);
}

float4 ShadowLitPS( PS_IN In ) : COLOR0
{ 
  // Apply shadow effect
  
  if( gLightTypeDirectional )
  {
    // shadowBias is used as a bias and not as a constant attenuation.
    const float shadowBias            = gLightConstantAttenuation;

    // shadowVarianceEpsilon is used as a bias and not as a linear attenuation.
    const float shadowVarianceEpsilon = gLightLinearAttenuation;

    // Shadow sampling blurring offet
    const float shadowBlurringFactor  = gLightQuadAttenuation;

    // Transform the surface into light space and project
    // NB: Could be done in the vertex shader, but doing it here keeps the "light
    // shader" abstraction and doesn't limit # of shadowed lights.
    float4 surf_tex = In.ShadowMapSamplingPos;
    surf_tex = surf_tex / surf_tex.w;

    // Rescale viewport to be [0,1] (texture coordinate space)
    float2 shadow_tex = surf_tex.xy * float2(0.5, -0.5) + 0.5;

    float4 moments = BlurX6(ShadowMapTextureSampler, shadow_tex, shadowBlurringFactor);

    // Rescale light distance and check if we're in shadow
    
    float rescaled_dist_to_light = In.RealDistance - shadowBias;
    float lit_factor = (rescaled_dist_to_light <= moments.x);

    // Variance shadow mapping
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = min(max(E_x2 - Ex_2, 0.0) + shadowVarianceEpsilon, 1.0);
    float m_d = (moments.x - rescaled_dist_to_light);
    float p = variance / (variance + m_d * m_d);
    float s = max(lit_factor, max(p, 0.35f));

    // Adjust the light color based on the shadow attenuation
    return float4(s,s,s,1);
  }

  return float4(1,1,1,1) * gFadeAmount;
}

//
// Technique
//
technique BaseTechnique
{
  pass P0
  {
    VertexShader      = compile vs_3_0 LitTexturedGeometryVS();
    PixelShader       = compile ps_3_0 LitTexturedGeometryPS();
  }

  pass P1
  {
    PixelShader       = compile ps_3_0 ShadowLitPS();

    AlphaBlendEnable  = True;
    BlendOp           = Add;
    SrcBlend          = Zero;
    DestBlend         = SrcColor;
  }
}
