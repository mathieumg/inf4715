///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Lighting utility functions
/// 

//--------------------------------------------------------------------------------------
/// Global variables used for lighting
//--------------------------------------------------------------------------------------

// The material's parameters
float4  gMaterialAmbientColor;    // Material's ambient color
float4  gMaterialDiffuseColor;    // Material's diffuse color
float4  gMaterialSpecularColor;   // Material's specular color
float   gMaterialSpecularPower;   // Material's specular power

// The light's parameters
bool    gLightTypeAmbient;
bool    gLightTypePoint;
bool    gLightTypeSpot;
bool    gLightTypeDirectional;
float3  gLightPosition;         // Light's position in world space
float3  gLightDirection;        // Light's direction in world space   
float   gLightCosPhi;           // Opening angle of the spotlight
float   gLightCosTheta;         // Opening angle of the spotlight
float   gLightFalloff;
float4  gLightDiffuseColor;     // Light's diffuse light color
float4  gLightAmbientColor;     // Light's ambient light color
float   gLightRange;            // Maximum distance covered by light
float   gLightConstantAttenuation;
float   gLightLinearAttenuation;
float   gLightQuadAttenuation;


//--------------------------------------------------------------------------------------
/// Root lighting functions for vertex shaders
//--------------------------------------------------------------------------------------

void SetVertexShaderValues(inout float3 position, inout float3 normal)
{
  // Compute vertex world position
  position = mul( float4(position, 1.0f), gWorld ).xyz;

  // Compute vertex normal
  normal = mul( normal, (float3x3)gWorld );
}

//--------------------------------------------------------------------------------------
/// Root lighting functions for pixel shaders
//--------------------------------------------------------------------------------------

float3 Specular(float3 viewDir, float3 lightDir, float3 normal)
{
  float3 L = lightDir;
  float3 N = normal;
  float3 V = viewDir;
  float3 H = normalize( L + V );
 
  return pow( saturate( dot( N, H ) ), gMaterialSpecularPower ) * gMaterialSpecularColor;
}

//--------------------------------------------------------------------------------------
// Returns the color of a pixel using OMNI light
float4 GetPointLighting(in const float3 worldPosition, 
                        in const float3 viewDir, 
                        in const float3 worldNormal, 
                        in const float4 diffuseColor )
{
  // Get the direction of the light
  float3 lightDirection = gLightPosition - worldPosition;
  float  lightDist      = length(lightDirection);
  lightDirection       /= lightDist;

  // This component is used at different places
  float diffComp = saturate( dot(worldNormal, lightDirection) );

  // Take down the diffuse color (modulate the material with the vertex color and the light color)
  float4 finalDiffuseColor = gLightDiffuseColor * diffuseColor;
  
  float att = gLightConstantAttenuation +
              lightDist * ( gLightLinearAttenuation +
                            gLightQuadAttenuation * lightDist);
  
  // Calculate the final color
  return finalDiffuseColor / att * diffComp;
}


//--------------------------------------------------------------------------------------
// Returns the color of a pixel using SPOT light 
float4 GetSpotLighting(in const float3 worldPosition, 
                       in const float3 viewDir, 
                       in const float3 worldNormal, 
                       in const float4 diffuseColor )
{
  float3 L = worldPosition - gLightPosition;  
  float dist = length(L);     
  L = L / dist;  // L is now normilized

  float coef = 0;
  float rho = dot(normalize(gLightDirection), L);  
  if (rho > gLightCosPhi)
  {  
    if (rho > gLightCosTheta)        
      coef = 1;         
    else   
      coef = pow(((rho - gLightCosPhi) / (gLightCosTheta - gLightCosPhi)), gLightFalloff);  
  }  

  float DistAttn = pow(1.0f - smoothstep(0.0f, gLightRange * gLightFalloff, dist), gLightQuadAttenuation);

  float3 specularColor = Specular(viewDir, gLightDirection, worldNormal);


  float3 finalColor = coef * DistAttn * ( diffuseColor.rgb + specularColor );
  
  return float4(finalColor, 1.0f);  
}

//--------------------------------------------------------------------------------------
// Returns the color of a pixel using DIRECTIONAL light
float4 GetDirectionalLighting( in const float3 worldPosition, 
                               in const float3 viewDir, 
                               in const float3 worldNormal, 
                               in const float4 diffuseColor )
{
  // Get the direction of the light
  float3 lightDirection = -gLightDirection;

  // This component is used at different places
  float diffComp = saturate( dot(worldNormal, lightDirection) );

  // Take down the diffuse color (modulate the material with the vertex color and the light color)
  float4 finalDiffuseColor = gLightDiffuseColor * diffuseColor;

  // Compute specular lighting
  float4 finalSpecularColor = float4(Specular(viewDir, lightDirection, worldNormal), 1.0f);

  // Calculate the final color
  return (finalDiffuseColor * diffComp) + finalSpecularColor;
}

//--------------------------------------------------------------------------------------
// Returns the color of a pixel
float4 GetLitPixelColor(in const float3 worldPosition, 
                        in const float3 viewDir, 
                        in const float3 worldNormal, 
                        in const float3 diffuseColor )
{
  // Returns the lit color of a pixel according to light type:
  // - POINT LIGHT
  // - SPOT LIGHT
  // - DIRECTIONAL LIGHT
	
  float4 color = float4( 0, 0, 0, 1 );
	  
  // Use dynamic branching to select the kind of light
  if( gLightTypeAmbient )
  {
    color = gMaterialAmbientColor * gLightAmbientColor * float4(diffuseColor, 1.0f);
  }
  else if( gLightTypeDirectional )
  {
    color = GetDirectionalLighting( worldPosition, viewDir, worldNormal, float4(diffuseColor,1.0f) );
  }
  else if( gLightTypePoint )
  {
    color = GetPointLighting( worldPosition, viewDir, worldNormal, float4(diffuseColor,1.0f) );
  }
  else if( gLightTypeSpot )
  {
    color = GetSpotLighting( worldPosition, viewDir, worldNormal, float4(diffuseColor,1.0f) );
  }

  return color;
}

