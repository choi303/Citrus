struct PS_IN
{
    float4 pos : SV_Position;
    float4 color : Color;
};

float4 main(PS_IN input) : SV_Target
{
    return float4(1.0,0,0,1);
}