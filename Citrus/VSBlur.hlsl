cbuffer Transformation : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
};

cbuffer ScreenSizeBuffer : register(b1)
{
    float screenWidth;
    float screenHeight;
    float2 padding;
};

struct VS_IN
{
    float3 pos : Position;
    float2 tex : Texcoord;
};

struct VS_OUT
{
    float4 pos : Sv_Position;
    float2 tex : Texcoord;
    float2 tex1 : Texcoord1;
    float2 tex2 : Texcoord2;
    float2 tex3 : Texcoord3;
    float2 tex4 : Texcoord4;
    float2 tex5 : Texcoord5;
    float2 tex6 : Texcoord6;
    float2 tex7 : Texcoord7;
    float2 tex8 : Texcoord8;
    float2 tex9 : Texcoord9;
};

VS_OUT main(VS_IN input)
{
    float texelSize;
    float texelSize2;
    VS_OUT vso;
    vso.pos = mul(float4(input.pos, 1.0f), world);
    vso.pos = mul(vso.pos, view);
    vso.pos = mul(vso.pos, projection);
    vso.tex = input.tex;
    
    //calculating texelSize
    texelSize = 1.0f / screenWidth;
    texelSize2 = 1.0f / screenHeight;
    
    //creation of horizontal blur
    vso.tex1 = input.tex + float2(texelSize * -4.0f, texelSize2 * -4.0f);
    vso.tex2 = input.tex + float2(texelSize * -3.0f, texelSize2 * -3.0f);
    vso.tex3 = input.tex + float2(texelSize * -2.0f, texelSize2 * -2.0f);
    vso.tex4 = input.tex + float2(texelSize * -1.0f, texelSize2 * -1.0f);
    vso.tex5 = input.tex + float2(texelSize *  0.0f, texelSize2 *  0.0f);
    vso.tex6 = input.tex + float2(texelSize *  1.0f, texelSize2 *  1.0f);
    vso.tex7 = input.tex + float2(texelSize *  2.0f, texelSize2 *  2.0f);
    vso.tex8 = input.tex + float2(texelSize *  3.0f, texelSize2 *  3.0f);
    vso.tex9 = input.tex + float2(texelSize *  4.0f, texelSize2 *  4.0f);
    return vso;
}