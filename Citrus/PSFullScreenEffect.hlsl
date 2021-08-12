Texture2D tex : register(t0);
SamplerState splr : register(s0);

cbuffer param : register(b0)
{
    bool blurEnabled;
    float blurIntensity;
    float pad[2];
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
    
    return tex.Sample(splr, input.uv);
}