cbuffer DirectLight : register(b5)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float ambientIntensity;
    bool normalMapEnabled;
    float specularIntensityC;
    float diffuseIntensityC;
    bool reflectionEnabled;
    float reflectionIntensity;
    float biasC;
    bool pcfEnabled;
    bool alphaClip;
    bool normals;
    bool emessiveEnabled;
    float emessiveIntensity;
    bool brightnessRender;
};

cbuffer PbrBuffer : register(b9)
{
    float3 albedo;
    float metallic;
    float roughness;
    float ao;
    float pad[2];
};

struct PS_IN
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

SamplerState object_sampler : register(s0);
SamplerState object_sampler_clamp : register(s1);
SamplerComparisonState CMPSampler : register(s2);
const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a2 = roughness * roughness * roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 main(PS_IN input) : SV_Target
{
    //Get calculated normal from vertex shader and normalize it
    float3 N = normalize(input.normal);
    //Get calculated view direction from vertex shader output and normalize it
    float3 V = normalize(input.cameraPos - input.worldPos);
    
    //Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    //of 0.04 and if it's a metal, use the albedo color as F0 (metalic workflow)
    float3 F0 = float3(0.04.xxx);
    F0 = lerp(F0, albedo, metallic);
    
    //Reflectance equation
    float3 Lo = float3(0.0, 0.0, 0.0);
    
    //Calculate directional light radiance
    float3 L = normalize(lightDirection - input.worldPos);
    float3 H = normalize(V + L);
    float distance = length(lightDirection - input.worldPos);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightDirection * attenuation;
    
    //Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(F0, max(0.0, dot(H, V)));
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    float3 specular = numerator / denominator;
    
    //kS is equal to Fresnel
    float3 kS = F;
    //For energy conservation, the diffuse and specular light can't
    //be above 1.0 (unless the surface emits light); to preserve this
    //relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0.xxx) - kS;
    
    //Multiply kD by the inverse metalness such that only non-metals 
    kD *= 1.0 - metallic;
    
    //Scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);
    
    //Add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * NdotL;
    
    //Ambient lighting
    float3 ambient = float3(0.03.xxx) * albedo * ao;
    float3 color = ambient + Lo;
    
    //HDR (High Dynamic Range) tonemapping
    color = color / (color + float3(1.0.xxx));
    
    //Gamma correction
    color = pow(color, float3(1.0 / 2.2.xxx));
    
    //Return pbr lighting value
    return float4(color, 1.0f);
}