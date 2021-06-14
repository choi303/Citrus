cbuffer Transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
};

cbuffer FogFactorBuffer : register(b1)
{
    float fogStart;
    float fogEnd;
    float2 pad;
};

struct VS_IN
{
    float3 pos : Position;
    float2 tex : Texcoord;
};

struct VS_OUT
{
    float4 pos : Sv_Position;
    float2 tex : Texcoord;
    float fogFactor : FOG;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    float4 cameraPosition;
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, projection);
    vso.tex = input.tex;
    cameraPosition = mul(float4(input.pos, 1.0f), world);
    cameraPosition = mul(cameraPosition, view);
    vso.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
    return vso;
}