Texture2D tex : register(t0);
Texture2D depthMap : register(t1);
Texture2D noiseMap : register(t3);
SamplerState samLinear : register(s0);
SamplerState samClamp : register(s1);

cbuffer SSAOBuffer : register(b2)
{
    float totalStrengthC;
    float baseC;
    float areaC;
    float fallOff;
    float radiusC;
    float pad[3];
};

float3 normalFromDepth(float depth, float2 texcoords)
{
    const float2 offset1 = float2(0.0, 0.001);
    const float2 offset2 = float2(0.001, 0.0);
  
    float depth1 = depthMap.Sample(samClamp, texcoords + offset1).r;
    float depth2 = depthMap.Sample(samClamp, texcoords + offset2).r;
  
    float3 p1 = float3(offset1, depth1 - depth);
    float3 p2 = float3(offset2, depth2 - depth);
  
    float3 normal = cross(p1, p2);
    normal.z = -normal.z;
  
    return normalize(normal);
}

struct PS_IN
{
    float4 positionViewport : SV_Position;
    float4 positionClip : positionClip;
    float2 texCoord : texCoord;
};

float4 main(PS_IN input) : SV_Target
{
    const float totalStrength = totalStrengthC;
    const float base = baseC;
    const float area = areaC;
    const float falloff = fallOff;
    const float radius = radiusC;
  
    const int samples = 16;
    float3 sampleSphere[samples] =
    {
        float3(0.5381, 0.1856, -0.4319), float3(0.1379, 0.2486, 0.4430),
        float3(0.3371, 0.5679, -0.0057), float3(-0.6999, -0.0451, -0.0019),
        float3(0.0689, -0.1598, -0.8547), float3(0.0560, 0.0069, -0.1843),
        float3(-0.0146, 0.1402, 0.0762), float3(0.0100, -0.1924, -0.0344),
        float3(-0.3577, -0.5301, -0.4358), float3(-0.3169, 0.1063, 0.0158),
        float3(0.0103, -0.5869, 0.0046), float3(-0.0897, -0.4940, 0.3287),
        float3(0.7119, -0.0154, -0.0918), float3(-0.0533, 0.0596, -0.5411),
        float3(0.0352, -0.0631, 0.5460), float3(-0.4776, 0.2847, -0.0271)
    };
  
    float3 random = noiseMap.Sample(samClamp, input.texCoord * 8.0).rgb;
  
    float depth = depthMap.Sample(samClamp, input.texCoord).r;
 
    float3 position = float3(input.texCoord, depth);
    float3 normal = normalFromDepth(depth, input.texCoord);
  
    float radiusDepth = radius / depth;
    float occlusion = 0.0;
    for (int i = 0; i < samples; i++)
    {
        float3 ray = radiusDepth * reflect(sampleSphere[i], random);
        float3 hemiRay = position + sign(dot(ray, normal)) * ray;
    
        float occDepth = depthMap.Sample(samClamp, hemiRay.xy).r;
        float difference = depth - occDepth;
    
        occlusion += step(falloff, difference) * (1.0 - smoothstep(falloff, area, difference));
    }
  
    float ao = 1.0 - totalStrength * occlusion / samples;
    return saturate(ao + base);
}