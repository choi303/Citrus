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
    float3 tan : tangent;
    float3 binormal : Binormal;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 tan : Tangent;
    float3 binormal : Binormal;
    float3 viewDirection : ViewDirection;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    float4 world_position;

	//set pos to all matrices
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
	//set output texture coordinates to input texture coordinates
    vso.tc = input.tc;
	//calculating normal and normalize the normals
    vso.normal = mul(input.normal, (float3x3) world);
    vso.normal = normalize(vso.normal);
    vso.worldPos = mul(float4(input.pos, 1.0f), world);//calculating world pos based world matrix for calculating vec to light or specular reflections
    //set tangent value
	vso.tan = mul(input.tan, (float3x3)world);
    vso.tan = normalize(vso.tan);
	//set bi normal value
    vso.binormal = mul(input.binormal, (float3x3) world);
    vso.binormal = normalize(vso.binormal);
	//get world position from world matrix
    world_position = mul(input.pos, world);
	//calculating view direction
    vso.viewDirection = camera_pos.xyz - world_position.xyz;
    vso.viewDirection = normalize(vso.viewDirection);
    return vso;
}