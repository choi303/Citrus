Texture2D image : register(t4);
SamplerState splr : register(s0);

cbuffer bloomBuffer : register(b0)
{
    float bloomIntensity;
    float pad[3];
}

struct PS_IN
{
    float2 uv : Texcoord;
};

float4 main(PS_IN input) : SV_Target
{
    int amount = 2; //amount of blur
    const int samples = 5;  //sample number
    //random pre-calculated weight samples
    float weight[samples] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
    uint width, height;
    image.GetDimensions(width, height); //get dimensions of brightness pass
    float2 imageOffset = 1.0f / float2(width, height);
    float3 result = image.Sample(splr, input.uv).rgb * weight[0];   //sample pass with weight
    //apply horizontal and vertical gaussian blur
    for (unsigned int i = 0; i < amount; i++)
    {
        for (int i = 1; i < samples; ++i)
        {
            result += image.Sample(splr, input.uv + float2(imageOffset.x * (i * bloomIntensity), 0.0f)).rgb * weight[i];
            result += image.Sample(splr, input.uv - float2(imageOffset.x * (i * bloomIntensity), 0.0f)).rgb * weight[i];
        }
        for (i = 1; i < samples; ++i)
        {
            result += image.Sample(splr, input.uv + float2(0.0f, imageOffset.y * (i * bloomIntensity))).rgb * weight[i];
            result += image.Sample(splr, input.uv - float2(0.0f, imageOffset.y * (i * bloomIntensity))).rgb * weight[i];
        }
    }
    //return blured result
    return float4(result, 1.0f);
}