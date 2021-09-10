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
    float3 tan : Tangent;
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
    float3 cameraPos : CameraPosition;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    float4 worldPosition;
    
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
    
    // Calculate the position of the vertex in the world.
    worldPosition = mul(float4(input.pos, 1.0f), world);
    
    vso.tc = input.tc;
    
    vso.normal = mul(input.normal, (float3x3) world);
    
    vso.worldPos = mul(float4(input.pos, 1.0f), world);
    
    vso.tan = mul(input.tan, (float3x3) world);
    vso.tan = normalize(vso.tan);
    
	//set bi normal value
    vso.binormal = mul(input.binormal, (float3x3) world);
    vso.binormal = normalize(vso.binormal);
    
    //determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    vso.viewDirection = camera_pos.xyz - worldPosition.xyz;
    
    vso.cameraPos = camera_pos;
    
    return vso;
}