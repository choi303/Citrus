//Tessellation test
cbuffer TessellationBuffer : register(b1)
{
    float tessellationAmount;
    float pad[3];
};

struct HS_IN
{
    float3 pos : Position;
    float4 color : Color;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos : Position;
    float4 color : Color;
};

ConstantOutputType ColorPatchConstantFunction(InputPatch<HS_IN, 3> inputPatch,
    uint patchId : SV_PrimitiveID)
{
    ConstantOutputType hso;
    
    //set the tessellation factors for the three edges of the triangle
    hso.edges[0] = tessellationAmount;
    hso.edges[1] = tessellationAmount;
    hso.edges[2] = tessellationAmount;
    
    hso.inside = tessellationAmount;
    return hso;
}

//Hull Shader
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HS_OUT main(InputPatch<HS_IN, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUT hso;
    
    hso.pos = patch[pointId].pos;
    hso.color = patch[pointId].color;
    return hso;
}