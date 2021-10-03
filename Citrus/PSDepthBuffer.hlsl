SamplerState obj_sampler : register(s0);
Texture2D tex : register(t0);

struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : Texcoord;
};

float4 main(PS_IN input) : SV_Target
{
    float4 color = tex.Sample(obj_sampler, input.uv);
    if (color.a < 0.5)
        discard;
    //set out position to red channel
    return float4(input.pos.z, 0, 0, 1.0f);
}