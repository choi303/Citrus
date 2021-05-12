struct PS_IN
{
    float4 pos : SV_Position;
    float3 n : Normal;
    float2 tc : Texcoord;
};

Texture2D albedo;
SamplerState objsampler;

float4 main(PS_IN input) : SV_Target
{
    float4 textureColor = albedo.Sample(objsampler, input.tc);
    textureColor.w = 1.0f;
    return textureColor;
}