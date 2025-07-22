#include "Scene.hlsli"

cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

Texture2D reflectionTexture : register(t0); // 反射されたシーンのテクスチャ
SamplerState samplerState : register(s0); // サンプラーステート

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};