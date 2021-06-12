SamplerState obj_sampler_state : register(s0);
Texture2D sky_map : register(t0);

struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
};

float4 main(PS_IN input) : SV_Target
{
    return sky_map.Sample(obj_sampler_state, input.tc);
}