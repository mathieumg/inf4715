uniform float ShadingLevels = 5.0;

float4 CellShadeColor( float3 normal, float3 viewDir, float4 textureColor)
{
	textureColor.rgb *= ShadingLevels;
	// Round to the highest integer. This is what flattens out the color
	textureColor.rgb = ceil(textureColor.rgb);
	// Redivide by the number of shading levels to normalize the values to between 0 and 1.
	textureColor.rgb /= ShadingLevels;
	
	return textureColor;
}