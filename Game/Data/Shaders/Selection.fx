///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

//--------------------------------------------------------------------------------------
/// Global variables
//--------------------------------------------------------------------------------------

// The material's parameters
float gSelected = 0.0f;

float3 gOutlineColor = float3(0.7148f, 0.6875, 0.2695);
float LineThickness = 0.01f;

//--------------------------------------------------------------------------------------
/// ShaderType - SELECTION - Apply selection effect if selected
//--------------------------------------------------------------------------------------
struct SELECTION_VS_IN
{
    float4 Position : POSITION;            // The position of the vertex
    float3 Normal : NORMAL;                // The vertex's normal
};

struct SELECTION_VS_OUT
{
  float4 Position             : POSITION;
  float3 PositionWorld        : TEXCOORD0; // World normal orientation
  float3 Normal               : TEXCOORD1;
};
//--------------------------------------------------------------------------------------
SELECTION_VS_OUT SelectionVS( SELECTION_VS_IN In )
{
  SELECTION_VS_OUT output;

  float4 original = mul( In.Position, gWorldViewProjection);
  float4 normal = mul(gWorldViewProjection, float4(In.Normal, 0));
 
  float4 pos = original + mul(LineThickness, normal);
  float3 posWorld = original.xyz / original.w;
  output.Position = pos;
  output.PositionWorld = posWorld;
  output.Normal = In.Normal;
  return output;
}

//--------------------------------------------------------------------------------------
float4 SelectionPS( SELECTION_VS_OUT In ) : COLOR0
{
  float3 viewDir = gViewPosition - In.PositionWorld;
  if( dot(viewDir, In.Normal) > 0 )
  {
      discard;
  }

  return float4(gOutlineColor, gSelected);
}


//--------------------------------------------------------------------------------------
/// Techniques available to the Vicuna Renderer
//--------------------------------------------------------------------------------------
//technique BaseTechnique
//{
//  pass P0
//  {          
//    VertexShader      = compile vs_3_0 SelectionVS();
//    PixelShader       = compile ps_3_0 SelectionPS();
//  }
//}b7b045