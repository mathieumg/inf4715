///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

// HLSL Utility functions

// Helper for modifying the saturation of a color.
float4 AdjustSaturation(float4 color, float saturation)
{
    // The constants 0.3, 0.59, and 0.11 are chosen because the
    // human eye is more sensitive to green light, and less to blue.
    float grey = dot(color.rgb, float3(0.3, 0.59, 0.11));

    return lerp(grey, color, saturation);
}

float2x2 RotationMatrix(float rotation)  
{  
    float c = cos(rotation);
    float s = sin(rotation);
 
    return float2x2(c, -s, s ,c);
} 

float4x4 Scale3DMatrix(float scale)
{
	float4x4 scalemat =
	{
			scale, 0, 0, 0,
			0, scale, 0, 0,
			0, 0, scale, 0,
			0, 0, 0, 1
	};
			
	return scalemat;
}

float4x4 Scale3DMatrix(float3 scale)
{
	float4x4 scalemat =
	{
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
	};
			
	return scalemat;
}

float4x4 Scale3DMatrix(float scale, float3 translation)
{
	float4x4 scalemat =
	{
			scale, 0, 0, translation.x,
			0, scale, 0, translation.y,
			0, 0, scale, translation.z,
			0, 0, 0, 1
	};
			
	return scalemat;
}

static const float2 poisson6[6] = 
{
  float2(-0.326212f, -0.40581f), 
  float2(-0.695914f, 0.457137f), 
  float2(0.96234f, -0.194983f), 
  float2(0.519456f, 0.767022f), 
  float2(0.507431f, 0.064425f), 
  float2(-0.32194f, -0.932615f)
};
  
float4 BlurX6(sampler2D tSource, float2 texCoord, float discRadius)
{
	float4 cOut = tex2D(tSource, texCoord);
	cOut += tex2D(tSource, texCoord + poisson6[0]*discRadius);	
	cOut += tex2D(tSource, texCoord + poisson6[1]*discRadius);	
	cOut += tex2D(tSource, texCoord + poisson6[2]*discRadius);	
	cOut += tex2D(tSource, texCoord + poisson6[3]*discRadius);	
	cOut += tex2D(tSource, texCoord + poisson6[4]*discRadius);	
	cOut += tex2D(tSource, texCoord + poisson6[5]*discRadius);	
	return ( cOut/7.0f);
}

#define BIG_KERNEL_SIZE 13

uniform int ScreenWidth = 1280;
uniform int ScreenHeight = 720;

static const float BigKernel[BIG_KERNEL_SIZE] =
{
    -6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6
};

static const float BigWeights[BIG_KERNEL_SIZE] =
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216
};
    
float4 BigGaussianBlur(sampler2D imgsampler, float2 texCoord, float2 direction, float amount)
{
    float4 color = 0;
    
    float2 texSize = float2( 1.0 / ScreenWidth, 1.0 / ScreenHeight );
    
    for (int p = 0; p < BIG_KERNEL_SIZE; p++)
    {
        color += tex2D( imgsampler, texCoord + (amount * direction * BigKernel[p] * texSize) ) * BigWeights[p];
    }
    
	return color;
}

float4 tored(float3 color)
{
	return float4(abs(color.r), -sign(color.r), 0.0f, 1.0f);
}

float4 togreen(float3 color)
{
	return float4(-sign(color.g), abs(color.g), 0.0f, 1.0f);
}

float4 toblue(float3 color)
{
	return float4(-sign(color.b), 0.0f, abs(color.b), 1.0f);
}

float4 tocolor(float3 color)
{
	return float4(abs(color), 1.0f);
}

float4 tocolornorm(float3 color)
{
	return float4((color + 1.0f) / 2, 1.0f);
}

bool InSphere(float radius, float3 pos)
{
	return length(pos) < radius;
}

// http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
bool RaySphereIntersection(float3 origin, float3 direction, float radius, out float3 intersection)
{
    //ComputeA,BandCcoefficients
    float a = dot(direction, direction);
    float b = 2 * dot(direction, origin);
    float c = dot(origin, origin) - radius * radius;
    
    //Finddiscriminant
    float disc = b * b - 4 * a * c;
    
    //ifdiscriminantisnegativetherearenorealroots,soreturn
    //falseasraymissessphere
    if(disc < 0)
        return false;
        
    //computeqasdescribedabove
    float distSqrt = sqrt(disc);
    float q;
    
    if(b < 0)
        q = (-b - distSqrt) / 2.0;
    else
        q = (-b + distSqrt) / 2.0;
        
    //computet0andt1
    float t0 = q / a;
    float t1 = c / q;
    
    //makesuret0issmallerthant1
    if(t0 > t1)
    {
        //ift0isbiggerthant1swapthemaround
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    
    //ift1islessthanzero,theobjectisintheray'snegativedirection
    //andconsequentlytheraymissesthesphere
    if(t1 < 0)
        return false;
    
    //ift0islessthanzero,theintersectionpointisatt1
    if(t0 < 0)
    {
        intersection = origin + t1 * direction;
        return true;
    }
    //elsetheintersectionpointisatt0
    else
    {
        intersection = origin + t0 * direction;
        return true;
    }
}