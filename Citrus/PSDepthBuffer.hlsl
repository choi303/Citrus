SamplerState obj_sampler : register(s0);
Texture2D obj_texture : register(t0);

struct PS_IN
{
    float4 pos : Sv_Position;
    float2 tex : Texcoord;
    float4 depthPosition : Texture0;
};

float4 main(PS_IN input) : SV_Target
{
    float depthValue;
    float4 color = float4(0,0,0,0);
    float4 textureColor = obj_texture.Sample(obj_sampler, input.tex);
    textureColor.w = 0.4f;
    
    //calculating depth value
    depthValue = input.depthPosition.z / input.depthPosition.w;
    
    //if range smaller than 0.9f return red
    if(depthValue < 0.9f)
    {
        color = float4(1.0f, 0.0f, 0.0f, 1.0f) * textureColor;
    }
    
    //if range bigger than 0.9f return green
    if(depthValue > 0.9f)
    {
        color = float4(0.0f, 1.0f, 0.0f, 1.0f) * textureColor;
    }
    
    //if range bigger than 0.925f return blue
    if(depthValue > 0.925f)
    {
        color = float4(0.0f, 0.0f, 1.0f, 1.0f) * textureColor;
    }
    
    return color;
}