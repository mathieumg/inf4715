#ifndef VCND3D9_SELECTION_H
#define VCND3D9_SELECTION_H

#include "d3dx9effect.h"
#include "VCNResources\EffectParamSet.h"

class VCND3D9_SelectionFX
{
public:
    VCND3D9_SelectionFX();
    ~VCND3D9_SelectionFX();

    /// Prepare render streams
    void CreateParameterHandles( LPD3DXEFFECT pShader );

    void SetSelectionState( LPD3DXEFFECT pShader, const VCNEffectParamSet& params );
private:

    // Effect global parameters
    D3DXHANDLE mSelected;
};

#endif