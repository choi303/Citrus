struct PS_IN
{
    float4 pos : SV_Position;
    float2 tc : Texcoord;
    float3 n : Normal;
};

Texture2D albedo;
SamplerState objsampler;

float4 main(PS_IN input) : SV_Target
{
    float4 textureColor = albedo.Sample(objsampler, input.tc);
    return textureColor;
}