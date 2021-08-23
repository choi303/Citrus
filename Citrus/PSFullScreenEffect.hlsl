Texture2D tex : register(t0);
Texture2D hdrTex : register(t2);
SamplerState splr : register(s0);

cbuffer param : register(b0)
{
    bool blurEnabled;
    float blurIntensity;
    float exposure;
    float gammaC;
};

struct PS_IN
{
    float2 uv : Texcoord;
};

static int r = 3 * blurIntensity;

float4 main(PS_IN input) : SV_Target
{
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
    
    //hdr texture sample
    float gamma = gammaC;
    float3 hdrColor = hdrTex.Sample(splr, input.uv).rgb;
    
    //exposure tone mapping
    float3 mapped = float3(1.0.xxx) - exp(-hdrColor * exposure);
    
    //gamma correction
    mapped = pow(mapped, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
    
    return float4(mapped, 1.0f);
}