cbuffer transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix proj;
};

cbuffer Matrices : register(b1)
{
    float3 camera_pos;
}

struct VS_IN
{
    float3 pos : Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 viewDirection : ViewDirection;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    float4 world_position;
    
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
    vso.tc = input.tc;
    vso.normal =  mul(input.normal, (float3x3)world);
    vso.worldPos = (float3) mul(float4(input.pos, 1.0f), world);
    //get world position from world matrix
    world_position = mul(float4(input.pos, 1.0f), view);
    world_position = mul(float4(input.pos, 1.0f), world);
	//calculating view direction
    vso.viewDirection = camera_pos.xyz - world_position.xyz;
    vso.viewDirection = normalize(vso.viewDirection);
    return vso;
}