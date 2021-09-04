struct VS_IN
{
    float3 pos : Position;
    float4 color : Color;
};

struct VS_OUT
{
    float3 pos : Position;
    float4 color : Color;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    
    vso.pos = input.pos;
    vso.color = input.color;
    return vso;
}