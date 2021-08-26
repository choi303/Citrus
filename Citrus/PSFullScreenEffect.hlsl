Texture2D tex : register(t0);
Texture2D hdrTex : register(t2);
Texture2D brightnessColor : register(t4);
Texture2D bloomTex : register(t5);
SamplerState splr : register(s0);

cbuffer param : register(b0)
{
    bool blurEnabled;
    float blurIntensity;
    float exposure;
    float gammaC;
    bool toneMappingEnabled;
    bool bloomEnabled;
    float pad[2];
};

struct PS_IN
{
    float2 uv : Texcoord;
};

static int r = 3 * blurIntensity;

float4 main(PS_IN input) : SV_Target
{
    float3 sceneColor = tex.Sample(splr, input.uv).rgb;
    if(blurEnabled)
    {
        uint width, height;
        tex.GetDimensions(width, height);
        const float dx = 1.0f / width;
        const float dy = 1.0f / height;
        float4 acc = float4(0, 0, 0, 0);
        for (int y = -r; y <= r; y++)
        {
            for (int x = -r; x <= r; x++)
            {
                const float2 tc = input.uv + float2(dx * x, dy * y);
                acc += tex.Sample(splr, tc).rgba;
            }
        }
        
        return acc / 9.0f;
    }
    
    if(toneMappingEnabled)
    {
        //hdr texture sample
        float gamma = gammaC;
        float3 hdrColor = hdrTex.Sample(splr, input.uv).rgb;
        float3 bloomColor = bloomTex.Sample(splr, input.uv).rgb;
        //if bloom is enabled then add bloom color to hdr color
        if(bloomEnabled)
        hdrColor += bloomColor;
        
        //tone mapping
        float3 result = float3(1.0.xxx) - exp(-hdrColor * exposure);
        
        //also gamma correct while  we're at it
        result = pow(result, float3(1.0 / gamma.xxx));
        
        //return mapped data
        return float4(result, 1.0f);
    }

    //return scene color
    return float4(sceneColor, 1.0f);
}