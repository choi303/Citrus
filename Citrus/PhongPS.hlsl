cbuffer Light : register(b0)
{
    float3 lightpos;
    float lightIntensity;
};

struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
};

Texture2D albedo : register(t0);
SamplerState objsampler : register(s0);

float4 main(PS_IN input) : SV_Target
{
    //ambient color
    float ambientIntensity = 0.7f;
    float3 ambientColor = { 0.15f, 0.15f, 0.15f };
    float3 ambientLight = ambientIntensity * ambientColor;
    
    //simple vector algebra
    float3 vectorToLight = lightpos - input.worldPos;   //draw vector between light position and camera position (worldpos)
    float distanceToLight = length(vectorToLight); // get the length between light position and camera position vector
    float3 dirToLight = vectorToLight / distanceToLight;    //calulate direction of light
  
    float att = 1.0f / (1.0f + 0.045f * distanceToLight + 0.0075f * (distanceToLight * distanceToLight));   //calculate attenuation
    
    //diffuse light
    float3 diffuseColor = { 1, 1, 1 };  //diffuse color
    float diffuseIntesity = lightIntensity;  //diffuse Intesity or Light Amount
    float3 diffuse = diffuseColor * diffuseIntesity * att * max(0.0f, dot(dirToLight, input.normal));   //diffuse light creation
    
    float3 reflectedLight = input.normal * dot(vectorToLight, input.normal);    //calculate specular reflection
    float3 r = reflectedLight * 2.0f - vectorToLight;   //caclculate reflection
    
    float specularIntesity = 3.6f;  //specular amount
    float specularPower = 64.0f;    //specular power (opacity)
    //specular reflection creation
    float3 specular = att * (diffuseColor * diffuseIntesity) * specularIntesity * pow(max(0.0f, dot(normalize(r), normalize(input.worldPos))), specularPower);
    
    //final light (d + a + s) * texture or color
    float4 finalLight = float4(saturate(diffuse + ambientLight + specular), 1.0f) * albedo.Sample(objsampler, input.tc);
    
    if(lightIntensity > 0.0f)
    return finalLight;
    else
        return float4(0, 0, 0, 0);
}