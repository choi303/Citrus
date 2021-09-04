cbuffer transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix proj;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos : Position;
    float4 color : Color;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float4 color : Color;
};

//Domain Shader
[domain("tri")]
PS_IN main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation,
const OutputPatch<HS_OUT, 3> patch)
{
    float3 vertexPosition;
    PS_IN pso;
    vertexPosition = uvwCoord.x * patch[0].pos + uvwCoord.y * patch[1].pos + uvwCoord.z * patch[2].pos;
    
    pso.pos = mul(float4(vertexPosition, 1.0), world);
    pso.pos = mul(pso.pos, view);
    pso.pos = mul(pso.pos, proj);
    pso.color = patch[0].color;
    return pso;
}