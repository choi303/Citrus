cbuffer Transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
};

struct VS_IN
{
    float3 pos : Position;
};

struct VS_OUT
{
    float4 pos : Sv_Position;
};

VS_OUT main(VS_IN input)
{
    //ONLY GET POSITION BECAUSE WE DON'T NEED ANYTHING ELSE
    VS_OUT vso;
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, projection);
    return vso;
}