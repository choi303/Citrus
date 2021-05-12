cbuffer transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix proj;
};

struct VS_IN
{
    float3 pos : Position;
    float3 color : Color;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 color : Color;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso = (VS_OUT)0;
    
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
    vso.color = input.color;
    
    return vso;
}