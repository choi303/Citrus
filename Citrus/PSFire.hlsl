Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState samLinear : register(s0);
SamplerState samClamp : register(s1);

cbuffer DistortionBuffer : register(b1)
{
    float2 distortion1;
    float2 distortion2;
    float2 distortion3;
    float distortionScale;
    float distortionBias;
};

struct PS_IN
{
    float4 position : SV_Position;
    float2 tex : Texcoord;
    float2 texCoords1 : Texcoord1;
    float2 texCoords2 : Texcoord2;
    float2 texCoords3 : Texcoord3;
};

float4 main(PS_IN input) : SV_Target
{
    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;
    
    //noise texture samples
    noise1 = noiseTexture.Sample(samLinear, input.texCoords1);
    noise2 = noiseTexture.Sample(samLinear, input.texCoords2);
    noise3 = noiseTexture.Sample(samLinear, input.texCoords3);

    //move the noise from the (0, 1) range to the (-1, +1) range.
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;
    
    //noise multiplactions with distortions
    noise1.xy = noise1.xy * distortion1.xy;
    noise2.xy = noise2.xy * distortion2.xy;
    noise3.xy = noise3.xy * distortion3.xy;
    //final noise calc
    finalNoise = noise1 + noise2 + noise3;
    //fire effect creation
    perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;
    noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;
    fireColor = fireTexture.Sample(samClamp, noiseCoords.xy);
    alphaColor = alphaTexture.Sample(samClamp, noiseCoords.xy);
    fireColor.a = alphaColor;
    if(fireColor.a < 0.5)
        discard;
    return fireColor;
}