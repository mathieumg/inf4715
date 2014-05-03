#pragma once

#include <string>

struct _D3DXMESHCONTAINER;
typedef _D3DXMESHCONTAINER *LPD3DXMESHCONTAINER;

class VCNModel;
struct D3DModel;
class VCNMesh;

struct ID3DXAnimationController;
struct _D3DXFRAME;
typedef _D3DXFRAME D3DXFRAME;

typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9;

class VCNNode;

class D3DConverter
{
public:
    static VCNModel* ConvertD3DModel(const D3DModel& model, LPDIRECT3DDEVICE9 device);

private:
    static VCNNode* ConvertMesh(const std::wstring& name, LPD3DXMESHCONTAINER basemesh, D3DXFRAME* frameRoot, 
        ID3DXAnimationController* animController, LPDIRECT3DDEVICE9 device);
};