///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Global variables common to all shaders
///

//
// Transformations
//

shared float4x4 gWorld;               // World matrix
shared float4x4 gView;                // View matrix
shared float4x4 gModelView;           // World * View
shared float4x4 gWorldViewProjection; // World * View * Projection matrix
shared float4x4 gNormalMatrix;        // Normal matrix
shared float3   gViewPosition;        // Position of the camera in world space
shared float    gFadeAmount = 1.0f;