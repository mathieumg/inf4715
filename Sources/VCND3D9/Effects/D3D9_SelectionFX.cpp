#include "Precompiled.h"
#include "D3D9_SelectionFX.h"
#include "VCNRenderer\LightingCore.h"
#include "VCNRenderer\MaterialCore.h"
#include "VCNRenderer\GPUProfiling.h"
#include "VCNRenderer\Shader.h"
#include "..\D3D9Shader.h"


VCND3D9_SelectionFX::VCND3D9_SelectionFX()
{
}


VCND3D9_SelectionFX::~VCND3D9_SelectionFX()
{
}

void VCND3D9_SelectionFX::CreateParameterHandles( LPD3DXEFFECT pShader )
{
    mSelected = pShader->GetParameterByName( NULL, "gSelected" );
}

void VCND3D9_SelectionFX::SetSelectionState( LPD3DXEFFECT pShader, const VCNEffectParamSet& params )
{
    HRESULT hr;

    float f = params.GetFloat(VCNTXT("Selected"));
    hr = pShader->SetFloat( mSelected, f);

    VCN_ASSERT( SUCCEEDED(hr) );
}
