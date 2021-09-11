Texture2D tex : register(t0);
SamplerState splr : register(s0);

struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : Texcoord;
};

float4 main(PS_IN input) : SV_TARGET
{
    const int radius = 5;
    
    uint resX, resY;
    tex.GetDimensions(resX, resY);
    float2 src_size = float2(1.0 / resX, 1.0 / resY);
    float2 uv = input.pos.xy / float2(resX, resY);
    float n = float((radius + 1) * (radius + 1));
    int i, j;
    float3 m0 = float3(0.0.xxx);
    float3 m1 = float3(0.0.xxx);
    float3 m2 = float3(0.0.xxx);
    float3 m3 = float3(0.0.xxx);
    float3 s0 = float3(0.0.xxx);
    float3 s1 = float3(0.0.xxx);
    float3 s2 = float3(0.0.xxx);
    float3 s3 = float3(0.0.xxx);
    float3 c;
    
    for (j = -radius; j <= 0; ++j)
    {
        for (int i = -radius; i <= 0; ++i)
        {
            c = tex.Sample(splr, uv + float2(i, j) * src_size).rgb;
            m0 += c;
            s0 += c * c;
        }
    }

    for (j = -radius; j <= 0; ++j)
    {
        for (int i = 0; i <= radius; ++i)
        {
            c = tex.Sample(splr, uv + float2(i, j) * src_size).rgb;
            m1 += c;
            s1 += c * c;
        }
    }

    for (j = 0; j <= radius; ++j)
    {
        for (int i = 0; i <= radius; ++i)
        {
            c = tex.Sample(splr, uv + float2(i, j) * src_size).rgb;
            m2 += c;
            s2 += c * c;
        }
    }

    for (j = 0; j <= radius; ++j)
    {
        for (int i = -radius; i <= 0; ++i)
        {
            c = tex.Sample(splr, uv + float2(i, j) * src_size).rgb;
            m3 += c;
            s3 += c * c;
        }
    }
    
    float min_sigma2 = 1e+2;
    float4 color;
    m0 /= n;
    s0 = abs(s0 / n - m0 * m0);

    float sigma2 = s0.r + s0.g + s0.b;
    if (sigma2 < min_sigma2)
    {
        min_sigma2 = sigma2;
        color = float4(m0, 1.0);
    }

    m1 /= n;
    s1 = abs(s1 / n - m1 * m1);

    sigma2 = s1.r + s1.g + s1.b;
    if (sigma2 < min_sigma2)
    {
        min_sigma2 = sigma2;
        color = float4(m1, 1.0);
    }

    m2 /= n;
    s2 = abs(s2 / n - m2 * m2);

    sigma2 = s2.r + s2.g + s2.b;
    if (sigma2 < min_sigma2)
    {
        min_sigma2 = sigma2;
        color = float4(m2, 1.0);
    }

    m3 /= n;
    s3 = abs(s3 / n - m3 * m3);

    sigma2 = s3.r + s3.g + s3.b;
    if (sigma2 < min_sigma2)
    {
        min_sigma2 = sigma2;
        color = float4(m3, 1.0);
    }
    
    return color;
}