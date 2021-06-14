SamplerState obj_sampler : register(s0);
Texture2D obj_texture : register(t0);

cbuffer FogBuffer : register(b0)
{
    float4 fogColor;
};

struct PS_IN
{
    float4 pos : Sv_Position;
    float2 tex : Texcoord;
    float fogFactor : FOG;
};

float4 main(PS_IN input) : SV_Target
{
    //texture sample
    float4 tex_sample = obj_texture.Sample(obj_sampler, input.tex);
    //set fog color
    //set final color texture multiply with fog factor
    float4 finalColor = input.fogFactor * tex_sample + (1.0 - input.fogFactor) * fogColor;
    return finalColor;
}