#include "MirrorCommon.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 worldPos = mul(float4(input.position, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, projection);

    output.tex = input.tex;
    return output;
}