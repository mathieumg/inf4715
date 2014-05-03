///
/// Copyright (C) 2011 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Render only geometry without any lighting effects or textures.
///

#include "ParamPool.fx"

//
// Uniform variables
//

/// Color to render the geometry
float4 gColor;	

//
// Input / Output structures
//
struct VS_IN
{
  float4 Position   : POSITION;
  float3 Color      : COLOR;
};

struct VS_OUT
{
  float4 Position		: POSITION;
  float4 Color      : COLOR;
};

struct PS_OUT
{
  float4 Color : COLOR0;
};

//
// Vertex shader
//
VS_OUT VicunaVertexShader(VS_IN In)
{
  VS_OUT Output;

  // Apply transforms
  Output.Position = mul( In.Position, gWorldViewProjection );
  Output.Color    = float4( In.Color.xyz, 1.0f );

  return Output;    
}


///
/// Pixel Shader
///
PS_OUT VicunaPixelShader( VS_OUT In ) 
{ 
  PS_OUT Output;

  // His color will simply be a mix of his ambient and diffuse
  Output.Color = gColor * In.Color * gFadeAmount;

  return Output;
}


//--------------------------------------------------------------------------------------
/// Techniques available to the Vicuna Renderer
//--------------------------------------------------------------------------------------
technique BaseTechnique
{
  pass BasePass
  { 
    VertexShader = compile vs_1_1 VicunaVertexShader();
    PixelShader  = compile ps_2_0 VicunaPixelShader();

    AlphaBlendEnable = false;
  }
}
