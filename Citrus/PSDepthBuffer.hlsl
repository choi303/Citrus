SamplerState obj_sampler : register(s0);

struct PS_IN
{
    float4 pos : SV_Position;
};

const float near = 0.1f;
const float far = 100.0f;

float4 main(PS_IN input) : SV_Target
{
    //set out position to red channel
    return float4(input.pos.z, 0, 0, 1.0f);
}