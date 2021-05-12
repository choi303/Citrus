struct PS_IN
{
    float4 pos : SV_Position;
    float3 color : Color;
};

float4 main(PS_IN input) : SV_Target
{
    
    return float4(input.color, 1.0f);
}