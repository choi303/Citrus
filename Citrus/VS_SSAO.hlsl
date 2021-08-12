struct VS_OUT
{
    float4 positionViewport : SV_Position;
    float4 positionClip : positionClip;
    float2 texCoord : texCoord;
};

VS_OUT main(uint vertexID : SV_VertexID)
{
    VS_OUT vso;
    vso.texCoord = float2((vertexID << 1) & 2, vertexID & 2);
    vso.positionClip = float4(vso.texCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    vso.positionViewport = vso.positionClip;
    return vso;
}