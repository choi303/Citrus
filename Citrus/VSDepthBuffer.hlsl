cbuffer Transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
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
    float4 depthPosition : Texture0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, projection);
    vso.tex = input.tex;
    vso.depthPosition = vso.pos;
    return vso;
}