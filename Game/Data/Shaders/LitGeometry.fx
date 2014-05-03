///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Render geometry with lighting effects only.
///

// Include the common base global variables
#include "ParamPool.fx"

// Include the lighting algos
#include "LightingFunctions.fx"

//
// Input / Output structures
//
struct VS_IN
{
  float4 Position   : POSITION;
  float3 Normal     : NORMAL;
  float3 Color      : COLOR;
};

struct VS_OUT
{
  float4 oPosition  :  POSITION;
  float3 Position   :  TEXCOORD0;
  float3 Normal     :  TEXCOORD1;
  float3 Diffuse    :  COLOR;
  float3 ViewDir    :  TEXCOORD2;
};

//
// Vertex shader
//
VS_OUT LitGeometryVS(VS_IN In)
{
  VS_OUT Output;

  // Apply transforms for the out position
  Output.oPosition = mul( In.Position, gWorldViewProjection );

  // Fill the params that need values
  Output.Position = In.Position.xyz / In.Position.w;
  Output.Normal   = In.Normal;
  Output.Diffuse  = In.Color;

  // Apply the vertex shader lighting calculations
  SetVertexShaderValues(Output.Position, Output.Normal);

  Output.ViewDir = gViewPosition - Output.Position;

  // Done!
  return Output;
}

//
// Pixel Shader
//
float4 LitGeometryPS( VS_OUT In ) : COLOR
{
  //return float4( abs(normalize(In.ViewDir)), 1.0f );
  //return float4(In.Diffuse, 1.0f);

  // Find out our overall diffuse color
  float3 diffuseColor = In.Diffuse * gMaterialDiffuseColor;


  // Call the lighting function and return the color!
  return GetLitPixelColor( In.Position, normalize(In.ViewDir), normalize(In.Normal), diffuseColor ) * gFadeAmount;
}





//--------------------------------------------------------------------------------------
/// Techniques available to the Vicuna Renderer
//--------------------------------------------------------------------------------------
technique BaseTechnique
{
  pass P0
  {  
    // Compile the shaders
    VertexShader = compile vs_2_0 LitGeometryVS();
    PixelShader  = compile ps_3_0 LitGeometryPS();
  }
}

