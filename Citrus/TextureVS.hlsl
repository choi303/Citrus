cbuffer transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix proj;
};

struct VS_IN
{
    float3 pos : Position;
    float2 tc : Texcoord;
    float3 n : Normal;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 n : Normal;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso = (VS_OUT) 0;
    
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
    vso.tc = input.tc;
    vso.n = mul(input.n, (float3x3)world);
    
    return vso;
}