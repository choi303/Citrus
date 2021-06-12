SamplerState obj_sampler_state : register(s0);
Texture2D obj_texture : register(t0);

struct PS_IN
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

float4 main(PS_IN input) : SV_Target
{
    float weight0, weight1, weight2, weight3, weight4;
    float normalization;
    float4 color;
    
    //set values of blur scales or amounts
    weight0 = 991.0f;
    weight1 = 990.9f;
    weight2 = 990.55f;
    weight3 = 990.18f;
    weight4 = 990.1f;
    
    //normalize that values with a scale value
    normalization = (weight0 + 4.0f * (weight1 + weight2 + weight3 + weight4));
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;
    
    //multiply texture coordinates with scale values
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    color += obj_texture.Sample(obj_sampler_state, input.tex1) * weight4;
    color += obj_texture.Sample(obj_sampler_state, input.tex2) * weight3;
    color += obj_texture.Sample(obj_sampler_state, input.tex3) * weight2;
    color += obj_texture.Sample(obj_sampler_state, input.tex4) * weight1;
    color += obj_texture.Sample(obj_sampler_state, input.tex5) * weight0;
    color += obj_texture.Sample(obj_sampler_state, input.tex6) * weight1;
    color += obj_texture.Sample(obj_sampler_state, input.tex7) * weight2;
    color += obj_texture.Sample(obj_sampler_state, input.tex8) * weight3;
    color += obj_texture.Sample(obj_sampler_state, input.tex9) * weight4;
    color.a = 1.0f;
    
    return color;
}