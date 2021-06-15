cbuffer Transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
};

struct VS_IN
{
    float3 pos : Position;
    float2 tc : Texcoord;
};

struct VS_OUT
{
    float4 pos : Sv_Position;
    float2 tc : Texcoord;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    //set pos multiply with world matrix
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, projection);
    //set out tc to in tc
    vso.tc = input.tc;
    return vso;
}