Texture2D cube_texture : register(t0);
SamplerState samLinear : register(s0);

struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
};

float4 main(PS_IN input) : SV_Target
{
    //sample sky map texture
    return cube_texture.Sample(samLinear, input.tc);
}