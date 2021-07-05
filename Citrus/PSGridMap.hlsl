cbuffer ColorBuffer : register(b0)
{
    float3 color;
    float pad;
};

struct PS_IN
{
    float4 pos : Sv_Position;
};

float4 main(PS_IN input) : SV_Target
{
    return float4(color, 1.0f);
}