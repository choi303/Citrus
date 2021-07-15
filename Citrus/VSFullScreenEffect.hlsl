struct VS_IN
{
    float2 pos : Position;
};

struct VS_OUT
{
    float2 uv : Texcoord;
    float4 pos : SV_Position;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    vso.pos = float4(input.pos, 0.0f, 1.0f);
    vso.uv = float2((input.pos.x + 1) / 2.0f,
    -(input.pos.y - 1) / 2.0f);
    return vso;
}