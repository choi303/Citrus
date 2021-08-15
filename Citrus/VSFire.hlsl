cbuffer MatrixBuffer : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix proj;
};

cbuffer NoiseBuffer : register(b1)
{
    float frameTime;
    float3 scrollSpeeds;
    float3 scales;
    float padding;
};

struct VS_IN
{
    float4 position : Position;
    float2 tex : Texcoord;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 tex : Texcoord;
    float2 texCoords1 : Texcoord1;
    float2 texCoords2 : Texcoord2;
    float2 texCoords3 : Texcoord3;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    input.position.w = 1.0f;
    //matrix calculations
    vso.position = mul(input.position, world);
    vso.position = mul(vso.position, view);
    vso.position = mul(vso.position, proj);
    //texcoord calc
    vso.tex = input.tex;
    //set texcoord based on scales for noise texture
    vso.texCoords1 = (input.tex * scales.x);
    vso.texCoords1.y = vso.texCoords1.y + (frameTime * scrollSpeeds.x);
    vso.texCoords2 = (input.tex * scales.y);
    vso.texCoords2.y = vso.texCoords2.y + (frameTime * scrollSpeeds.y);
    vso.texCoords3 = (input.tex * scales.z);
    vso.texCoords3.y = vso.texCoords3.y + (frameTime * scrollSpeeds.z);
    return vso;
}