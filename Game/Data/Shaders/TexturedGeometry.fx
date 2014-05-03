///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "ParamPool.fx"
#include "Selection.fx"

//--------------------------------------------------------------------------------------
/// Global variables
//--------------------------------------------------------------------------------------

// The material's parameters
float4  gMaterialDiffuseColor;  // Material's diffuse color
texture gDiffuseTexture;        // Diffuse color texture


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
};


//--------------------------------------------------------------------------------------
/// ShaderType - ST_TEXTURED_GEOMETRY - Render geometry without any lighting effects
/// but apply textures.
//--------------------------------------------------------------------------------------
struct VS_OUT
{
  float4 Position   : POSITION;
  float2 DiffuseUV  : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
VS_OUT TexturedGeometryVS(  float4 inPosition  : POSITION, float2 inTexCoord  : TEXCOORD0 )
{
  VS_OUT Output;

  // Apply transforms
  Output.Position = mul( inPosition, gWorldViewProjection );

  // Copy the set of UVs for diffuse texture
  Output.DiffuseUV = inTexCoord;

  return Output;    
}

//--------------------------------------------------------------------------------------
float4 TexturedGeometryPS( VS_OUT In ) : COLOR0
{ 
  // Lookup texture color and modulate it with the material color
  float4 textureColor = tex2D( DiffuseTextureSampler, In.DiffuseUV );
  float4 finalColor = textureColor * float4(gMaterialDiffuseColor);
  
  return finalColor * gFadeAmount;
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

    CullMode = CW;
    AlphaTestEnable = TRUE;
    AlphaFunc       = Greater;
    AlphaRef        = 0x000080;
  }

  pass P0
  {          
    // Alpha blending
    AlphaBlendEnable  = True;
    SrcBlend          = SrcAlpha;
    DestBlend         = InvSrcAlpha;
    CullMode          = CCW;

    VertexShader      = compile vs_3_0 TexturedGeometryVS();
    PixelShader       = compile ps_3_0 TexturedGeometryPS();
  }
}