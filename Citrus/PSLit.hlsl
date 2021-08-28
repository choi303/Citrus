cbuffer DirectLight : register(b0)
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

struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 viewDirection : ViewDirection;
    float4 lightViewPosition : LightViewPosition;
};

Texture2D environment : register(t3);
Texture2D depthMap : register(t4);
SamplerState object_sampler : register(s0);
SamplerState object_sampler_clamp : register(s1);
SamplerComparisonState CMPSampler : register(s2);

float4 main(PS_IN input) : SV_Target
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float shadow;
    float bias;
    
    uint width, height;
    
    //lighdirection set
    lightDir = -lightDirection;
    
    //bias set
    bias = biasC;
    
    //diffuse texture sample
    textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    //diffuse set
    float4 diffuse = diffuseColor * diffuseIntensityC;
    
    //set final color ambient light for default value
    color = ambientColor * ambientIntensity;
    
    //light view projection calculation
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
    
    //if model is in light view projection then make shadows calculation
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        //shadow map sample
        depthValue = depthMap.Sample(object_sampler_clamp, projectTexCoord).r;
        
        //light depth value calculation
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
        lightDepthValue = lightDepthValue - bias;
        
        //if pcf enabled then apply pcf filer
        if (pcfEnabled)
        {
            shadow = 0.0;
            depthMap.GetDimensions(width, height);
            float2 texelSize = 1.0 / float2(width, height);
            for (int x = -1; x <= 1; ++x)
            {
                for (int y = -1; y <= 1; ++y)
                {
                    //sample with CMPSampler for getting correct result of pcf
                    float pcfDepth = depthMap.SampleCmpLevelZero(CMPSampler, projectTexCoord.xy + float2(x, y) * texelSize, lightDepthValue).r;
                    shadow += pcfDepth;
                }
            }
            shadow /= 9.0f;
        }
        
        if (lightDepthValue < depthValue)
        {
            lightIntensity = saturate(dot(input.normal, lightDir));
        
            if (lightIntensity > 0.0f)
            {
                color += (diffuse * lightIntensity);
                if (pcfEnabled)
                    color *= shadow;
                color = saturate(color);
                
                //blinn phong thing
                float3 halfwayDir = normalize(lightDir + input.viewDirection);
                
                reflection = normalize(2 * lightIntensity * input.normal - lightDir);
                specular = (pow(saturate(dot(reflection, input.viewDirection)), 35.0f)) * specularIntensityC;
                
                //if reflection enabled then sample enviorment color with specular color if is not then normally return color
                if (reflectionEnabled)
                {
                    float3 incident = -input.viewDirection;
                    float2 reflectionVector = reflect(incident, input.normal);
                    float4 reflectionColor = environment.Sample(object_sampler,
                        reflectionVector);
                    float4 reflectionFactor = reflectionColor * reflectionIntensity;
                    specular *= reflectionFactor * 10.0f;
                }
                
                color = saturate((color * textureColor) + specular);
            }
        }
    }
    else
    {
        //if model is not in light view projection then simply make default light
        lightIntensity = saturate(dot(input.normal, lightDir));
        if (lightIntensity > 0.0f)
        {
            //blinn phong thing
            float3 halfwayDir = normalize(lightDir + input.viewDirection);
                
            reflection = normalize(2 * lightIntensity * input.normal - lightDir);
            specular = (pow(saturate(dot(reflection, input.viewDirection)), 35.0f)) * specularIntensityC;
                
            //if reflection enabled then sample enviorment color with specular color if is not then normally return color
            if (reflectionEnabled)
            {
                float3 incident = -input.viewDirection;
                float2 reflectionVector = reflect(incident, input.normal);
                float4 reflectionColor = environment.Sample(object_sampler,
                        reflectionVector);
                float4 reflectionFactor = reflectionColor * reflectionIntensity;
                specular *= reflectionFactor * 10.0f;
            }
            
            color += (diffuse * lightIntensity);
            color = saturate((color * textureColor) + specular);
        }
    }
    
    //if normals enabled then make normals looking render
    if (normals)
    {
        input.normal = normalize(input.normal);
        return float4(input.normal, input.pos.z);
    }
    
    //if brightnessRender is true then render brightness only
    if (brightnessRender)
    {
        float4 brightColor;
        float brightness = dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f));
        if (brightness > 1.0f)
        {
            brightColor = float4(color.rgb, 1.0f);
        }
        else
        {
            brightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        
        return brightColor;
    }
    
    //return final color
    return color;
}