cbuffer Light : register(b0)
{
    float3 lightpos;
    float lightIntensity;
    float ambientIntensity;
    float specularIntensity;
    bool reflectionEnabled;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float3 viewDirection : ViewDirection;
};

Texture2D diff : register(t0);
Texture2D spec : register(t1);
Texture2D environment : register(t2);
SamplerState objsampler : register(s0);

float4 main(PS_IN input) : SV_Target
{
    //ambient color
    float ambient_amount = ambientIntensity;
    float3 ambientColor = { 0.15f, 0.15f, 0.15f };
    float3 ambientLight = ambientIntensity * ambientColor;
    
    //simple vector algebra
    float3 vectorToLight = lightpos - input.worldPos; //draw vector between light position and camera position (worldpos)
    float distanceToLight = length(vectorToLight); // get the length between light position and camera position vector
    float3 dirToLight = vectorToLight / distanceToLight; //calulate direction of light
  
    float att = 1.0f / (1.0f + 0.045f * distanceToLight + 0.0075f * (distanceToLight * distanceToLight)); //calculate attenuation
    
    //diffuse light
    float3 diffuseColor = { 1, 1, 1 }; //diffuse color
    float diffuseIntesity = lightIntensity; //diffuse Intesity or Light Amount
    float3 diffuse = diffuseColor * diffuseIntesity * att * max(0.0f, dot(dirToLight, input.normal)); //diffuse light creation
    
    float3 reflectedLight = input.normal * dot(vectorToLight, input.normal); //calculate specular reflection
    float3 r = reflectedLight * 2.0f - vectorToLight; //caclculate reflection
    
    //specular texture
    float4 specularSample = spec.Sample(objsampler, input.tc); //specular texture sample
    float3 specularReflectionColor = specularSample.rgb; //sample colors
    float specularPower = pow(2.0f, specularSample.a * 13.0f); //specular power based texture (a) channel
    //specular reflection creation
    float3 specular = att * (diffuseColor * diffuseIntesity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(input.viewDirection))), specularPower);
    
    float4 diffTexture = diff.Sample(objsampler, input.tc);
    
    float3 incident = -input.viewDirection;
    float2 reflectionVector = reflect(incident, input.normal);
    float4 reflectionColor = environment.Sample(objsampler,
        reflectionVector);
    float4 reflectionFactor = reflectionColor * 9.0f;
    
    if (reflectionEnabled)
    {
        //final light (d + a) * texture or color
        float4 finalLight = float4(saturate((diffuse + ambientLight) * (diffTexture.rgb * reflectionColor) + specular * specularReflectionColor), 
        1.0f);
        
        return finalLight;
    }
    
    //final light (d + a) * texture or color
    float4 finalLight = float4(saturate((diffuse + ambientLight) * diffTexture.rgb + specular * specularReflectionColor), 1.0f);
    
    return finalLight;
}