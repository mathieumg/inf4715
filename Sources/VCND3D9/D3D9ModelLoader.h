#pragma once

#include <string>

class VCNModel;

class D3DModelLoader
{
public:
    static VCNModel* Load(const std::wstring& modelName);
};

