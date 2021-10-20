Texture2D tex : register(t0);
Texture2D tex2 : register(t7);
Texture2D depthMap : register(t1);
Texture2D normalsMap : register(t6);
SamplerState samLinear : register(s0);
SamplerState samClamp : register(s1);

struct PS_IN
{
    float4 positionViewport : SV_Position;
    float4 positionClip : positionClip;
    float2 texCoord : texCoord;
};

cbuffer ssrBuffer : register(b0)
{
    row_major matrix view;
    row_major matrix proj;
    row_major matrix invProj;
    float3 camPos;
    float minRaySteps;
    float reflectivity;
    float pad[3];
};

float3 CalcViewPositionFromDepth(in float2 TexCoord)
{
    // Combine UV & depth into XY & Z (NDC)
    float3 rawPosition = float3(TexCoord, depthMap.Sample(samClamp, TexCoord).r);

    // Convert from (0, 1) range to (-1, 1)
    float4 ScreenSpacePosition = float4(rawPosition * 2 - 1, 1);

    // Undo Perspective transformation to bring into view space
    float4 ViewPosition = mul(invProj, ScreenSpacePosition);

    // Perform perspective divide and return
    return ViewPosition.xyz / ViewPosition.w;
}

float2 RayCast(float3 dir, inout float3 hitCoord, out float dDepth)
{
    dir *= 0.25f;

    for (int i = 0; i < 10; ++i)
    {
        hitCoord += dir;

        float4 projectedCoord = mul(view, float4(hitCoord, 1.0));
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = CalcViewPositionFromDepth(projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if (dDepth < 0.0)
            return projectedCoord.xy;
    }

    return float2(0.0f.xx);
}

float4 main(PS_IN input) : SV_Target
{
    float3 viewNormal = normalsMap.Sample(samLinear, input.texCoord).xyz;
    float viewDepth = CalcViewPositionFromDepth(input.texCoord);
    float3 screenPos = 2.0f * float3(input.texCoord, viewDepth) - 1.0f;
    float4 viewPos = mul(invProj, float4(screenPos, 1.0f));
    viewPos /= viewPos.w;

    float3 reflected = normalize(reflect(normalize(viewPos.xyz), normalize(viewNormal)));

    float3 hitPos = viewPos.xyz;
    float dDepth;
    float minRayStep = minRaySteps;
    float2 coords = RayCast(reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);
    
    return lerp(tex.SampleLevel(samLinear, input.texCoord, 0), tex.SampleLevel(samLinear, coords, 0), reflectivity);
}