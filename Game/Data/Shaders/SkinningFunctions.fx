// Matrix Pallette
static const int MAX_MATRICES = 40;
uniform float4x4 gWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;

void ApplySkinning(	inout float4 vPosition,
					inout float3 vNormal,
					in float4 vBlendWeights,
					in float4 vBlendIndices,
					uniform int NumBones)
{
	float LastWeight = 0.0f;

	float4 initialPos = vPosition;
	float4 initialNormal = float4(vNormal, 0.0f);
	
	vPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	vNormal = 0.0f;

	// Compensate for lack of UBYTE4 on Geforce3
    int4 IndexVector = D3DCOLORtoUBYTE4(vBlendIndices);
	
	// cast the vectors to arrays for use in the for loop below
    float BlendWeightsArray[4] = (float[4])vBlendWeights;
    int IndexArray[4] = (int[4])IndexVector;

	// calculate the pos/normal using the "normal" weights 
    // and accumulate the weights to calculate the last weight
    for (int iBone = 0; iBone < NumBones - 1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];

        vPosition += mul(initialPos, gWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        vNormal += (mul(initialNormal, gWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone]).xyz;
    }
    LastWeight = 1.0f - LastWeight;

	// Now that we have the calculated weight, add in the final influence
    vPosition += mul(initialPos, gWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight;
    vNormal += (mul(initialNormal, gWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight).xyz;

	// normalize normals and set output
    vNormal = normalize(vNormal);
}
