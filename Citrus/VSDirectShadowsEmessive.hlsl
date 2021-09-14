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

cbuffer shadowBuffer : register(b2)
{
    row_major matrix lightViewMatrix;
    row_major matrix lightProjectionMatrix;
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
    float4 lightViewPosition : LightViewPosition;
    float3 TangentLightPos : TangentLightPos;
    float3 TangentViewPos : TangentViewPos;
    float3 TangentFragPos : TangentFragPos;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso;
    float4 worldPosition;
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, proj);
    vso.tc = input.tc;
    vso.normal = mul(input.normal, (float3x3) world);
    vso.normal = normalize(vso.normal);
    vso.tan = mul(input.tan, (float3x3) world);
    vso.tan = normalize(vso.tan);
	//set bi normal value
    vso.binormal = mul(input.binormal, (float3x3) world);
    vso.binormal = normalize(vso.binormal);
        // Calculate the position of the vertex in the world.
    worldPosition = mul(float4(input.pos, 1.0f), world);
    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    vso.viewDirection = camera_pos.xyz - worldPosition.xyz;
    // Normalize the viewing direction vector.
    vso.viewDirection = normalize(vso.viewDirection);
    
    vso.lightViewPosition = mul(float4(input.pos, 1.0f), world);
    vso.lightViewPosition = mul(vso.lightViewPosition, lightViewMatrix);
    vso.lightViewPosition = mul(vso.lightViewPosition, lightProjectionMatrix);
    
    float3 T = normalize(mul((float3x3) world, input.tan));
    float3 B = normalize(mul((float3x3) world, input.binormal));
    float3 N = normalize(mul((float3x3) world, input.normal));
    float3x3 TBN = transpose(float3x3(T, B, N));
    
    vso.TangentLightPos = mul(TBN, vso.lightViewPosition.xyz);
    vso.TangentViewPos = mul(TBN, vso.viewDirection);
    vso.TangentFragPos = mul(TBN, vso.pos.xyz);
    
    return vso;
}