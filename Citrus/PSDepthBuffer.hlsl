SamplerState obj_sampler_state : register(s0);
Texture2D sky_map : register(t0);

struct PS_IN
{
    float4 pos : Sv_Position;
    float4 depthPosition : Texture0;
};

float4 main(PS_IN input) : SV_Target
{
    float depthValue;
    float4 color;
    
    depthValue = input.depthPosition.z / input.depthPosition.w;
    
    if(depthValue < 0.9f)
    {
        color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    
    if(depthValue > 0.9f)
    {
        color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    
    if(depthValue > 0.925f)
    {
        color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    
    return color;
}