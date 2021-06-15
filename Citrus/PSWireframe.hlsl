//Create a color buffer for changing color dynamically
cbuffer ColorBuffer : register(b0)
{
    float3 wireColor;
    float pad;
};

struct PS_IN
{
    float4 pos : Sv_Position;
};

float4 main(PS_IN input) : SV_Target
{ 
    //only return wire color
    return float4(wireColor, 1.0f);
}