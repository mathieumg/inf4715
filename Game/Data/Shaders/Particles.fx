///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

//
// Uniforms
//
float4x4 View  : VIEW;
float4x4 Proj  : PROJECTION;

//
// Texture samples
//
texture partTex;
sampler texSample= sampler_state
{
  Texture = (partTex);
  MipFilter = LINEAR; 
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};


struct VS_OUTPUT
{
  float4 Pos      : POSITION;
  float4 Diff     : COLOR0;
  float2 TexCoord : TEXCOORD0;
};

//
// Vertex shader
//
VS_OUTPUT VS(float4 Position : POSITION,
             float4 Color    : COLOR0,
             float3 TexCoord : TEXCOORD0 )
{
  VS_OUTPUT Out = (VS_OUTPUT)0;
  
  // Make into billboard, transform and project
  float4 ViewPos = mul(Position, View);
  float4 Pos = ViewPos + float4(TexCoord.xy, 0, 0);

  // Construct rotation matrix to rotate about z axis around a point in the x-y plane (i.e. center)
  float c = cos(TexCoord.z); //cos of input angle
  float s = sin(TexCoord.z); //sin of input angle
  float4x4 Rotate = { float4(c,-s, 0, -c*ViewPos.x + s*ViewPos.y + ViewPos.x),
                      float4(s, c, 0, -s*ViewPos.x - c*ViewPos.y + ViewPos.y),
                      float4(0,0,1,0),
                      float4(0,0,0,1) };
    
  Pos = mul(Pos, transpose(Rotate));
  Out.Pos = mul(Pos, Proj);
  
  // Clamp texture coordinates to [0,1]
  // Make anything greater than 0.001 equal 1 and anything else 0
  Out.TexCoord = step( saturate(TexCoord), 0.001f );
  
  Out.Diff = Color;
  
  return Out;
}


//
// Pixel shader
//
float4 PS(VS_OUTPUT In) : COLOR
{
  // Texture for model
  float4 Tex = tex2D(texSample, In.TexCoord);

  // The alpha channel of tex should control the original alpha. 
  // The diff will control the fading over time. 
  // Either the diff or the tex can control the color.
  return In.Diff * Tex; 
}


technique Particles
{
  pass P0
  {
    VertexShader      = compile vs_1_1 VS();
    PixelShader       = compile ps_2_0 PS();

    AlphaBlendEnable  = TRUE;
    ZWriteEnable      = FALSE;
    ZEnable           = TRUE;
    CullMode          = CCW;
  
    ColorOp[0]        = Add; 
    ColorArg1[0]      = Diffuse;
    ColorArg2[0]      = Texture;
    AlphaOp[0]        = Modulate;
    AlphaArg1[0]      = Diffuse;
    AlphaArg2[0]      = Texture;
  }
}

