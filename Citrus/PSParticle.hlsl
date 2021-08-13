Texture2D shaderTexture : register(t0);
SamplerState samLinear : register(s0);

struct PS_IN
{
    float4 position : SV_Position;
    float2 tex : Texcoord;
    float4 color : Color;
};

float4 main(PS_IN input) : SV_Target
{
    float4 textureColor;
    float4 finalColor;
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(samLinear, input.tex);

    // Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;

    return finalColor;
}