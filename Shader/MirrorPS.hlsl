#include "MirrorCommon.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 reflectionColor = reflectionTexture.Sample(samplerState, input.tex);

    // •K—v‚Å‚ ‚ê‚Î”½ËF‚ÉŒø‰Ê‚ğ‰Á‚¦‚é
    // reflectionColor.rgb *= 0.9;

    return reflectionColor;
}