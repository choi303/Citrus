struct PS_IN
{
    float4 pos : Sv_Position;
    float4 depthPosition : Texture0;
};

float4 main(PS_IN input) : SV_Target
{
    float depthValue;
    float4 color;
    
    //calculating depth value
    depthValue = input.depthPosition.z / input.depthPosition.w;
    
    //if range smaller than 0.9f return red
    if(depthValue < 0.9f)
    {
        color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    
    //if range bigger than 0.9f return blue
    if(depthValue > 0.9f)
    {
        color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    
    //if range bigger than 0.925f return blue
    if(depthValue > 0.925f)
    {
        color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    
    return color;
}