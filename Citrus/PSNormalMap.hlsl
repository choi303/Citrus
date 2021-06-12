cbuffer Light : register(b0)
{
    float3 lightpos;
    float lightIntensity;
    float ambientIntensity;
    bool normalMapEnabled;
    float specularIntensity;
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
};

Texture2D diff : register(t0);
Texture2D spec : register(t1);
Texture2D normal : register(t2);
SamplerState object_sampler : register(s0);

float4 main(PS_IN input) : SV_Target
{
    const float att1 = 1.0f;
    const float att2 = 0.045f;
    const float att3 = 0.0075f;

	//calculating normal map
    float3 bump_map = normal.Sample(object_sampler, input.tc);
    bump_map = (bump_map * 2.0f) - 1.0f;

    const float3 bump_normal = (bump_map.x * input.tan) + (bump_map.y * input.binormal) + (bump_map.z * input.normal);
	//if normal mapping enabled enable normal map of object
	if(normalMapEnabled)
        input.normal = normalize(bump_normal);

    //ambient color
    const float3 ambient_color = { 0.15f, 0.15f, 0.15f };
    const float3 ambient_light = ambientIntensity * ambient_color;
    
    //simple vector algebra
    const float3 vectorToLight = lightpos - input.worldPos; //draw vector between light position and camera position (world position)
    const float distanceToLight = length(vectorToLight); // get the length between light position and camera position vector
    const float3 dirToLight = vectorToLight / distanceToLight; //calculate direction of light
  
    const float att = 1.0f / (att1 + att2 * distanceToLight + att3 * (distanceToLight * distanceToLight)); //calculate attenuation
    
    //diffuse light
    const float3 diffuseColor = { 1, 1, 1 }; //diffuse color
    const float diffuseIntensity = lightIntensity; //diffuse Intensity or Light Amount
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToLight, input.normal)); //diffuse light creation
    
    const float3 reflectedLight = input.normal * dot(vectorToLight, input.normal); //calculate specular reflection
    const float3 r = reflectedLight * 2.0f - vectorToLight; //caclculate reflection
    
    //specular texture
    const float4 specular_sample = spec.Sample(object_sampler, input.tc);
    const float specularPower = pow(2.0f, specular_sample.a * 6.0f); //specular power based texture (a) channel
    //specular reflection creation
    const float3 specular = att * diffuse * specularIntensity * pow(max(0.0f, dot(normalize(r), normalize(input.viewDirection))), specularPower);
    
    //final light (diffuse + ambient) * sample.rgb + specular * reflection color
    const float4 finalLight = float4(saturate((diffuse + ambient_light) * diff.Sample(object_sampler, input.tc).rgb + specular), 1.0f);
    //return final light
    return finalLight;
}