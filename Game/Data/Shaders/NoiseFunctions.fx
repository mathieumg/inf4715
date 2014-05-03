sampler3D volumeSampler = sampler_state
{
    texture = g_VolumeTexture;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MipFilter = Linear;
    MinFilter = Linear;
    MagFilter = Linear;
};

sampler3D volumeNoMipSampler = sampler_state
{
    texture = g_VolumeTexture;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
};

float NoMipNoise(float2 pos)
{
	return tex3D(volumeNoMipSampler, float3(pos, 0)).a;
}

float NoMipNoise(float3 pos)
{
	return tex3D(volumeNoMipSampler, pos).a;
}

float Noise(float2 pos)
{
    return tex3D(volumeSampler, float3(pos, 0)).a;
}

float Noise(float3 pos)
{
    return tex3D(volumeSampler, pos).a;
}