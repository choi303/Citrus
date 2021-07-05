#include "UI.h"
#include <Pdh.h>

//Classic UI
void UI::ClassicUI(Model* model, std::string uiTitle, float
    pos[3], float rot[3], float scale[3]) const
{
    pos[0] = model->GetPos().x;
    pos[1] = model->GetPos().y;
    pos[2] = model->GetPos().z;
    ImGui::Begin(uiTitle.c_str(), nullptr, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoCollapse);
    ImGui::DragFloat3("Position\nX/Y/Z", pos, 0.01f, 
        -999999.0f, 999999.0f);
    model->SetPos(pos[0], pos[1], pos[2]);
    rot[0] = model->GetRot().x;
    rot[1] = model->GetRot().y;
    rot[2] = model->GetRot().z;
    ImGui::DragFloat3("Rotation\nX/Y/Z", rot, 0.01f, 
        -999999999.0f, 999999999.0f);
    model->SetRot(rot[0], rot[1], rot[2]);
    scale[0] = model->GetScale().x;
    scale[1] = model->GetScale().y;
    scale[2] = model->GetScale().z;
    ImGui::DragFloat3("Scale\nX/Y/Z", scale, 0.01f, 
        -999999999.0f, (999999999.0f));
    model->SetScale(scale[0], scale[1], scale[2]);
    if (ImGui::Button("Reset Position and Rotation", ImVec2
    (200, 45)))
    {
        model->SetPos(0.0f, 0.0f, 0.0f);
        model->SetRot(0.0f, 0.0f, 0.0f);
        model->SetScale(1.0f, 1.0f, 1.0f);
    }
    ImGui::End();
}

//Custom UI for point light
void UI::PointLight(Model* model, const std::string 
    uiTitle, float pos[3], float rot[3], float scale[3],
    float* Intensity,
    float* ambientIntensity, BOOL* normalMappingEnabled,
    float* specularIntensity, BOOL* reflectionEnabled, 
    float* reflectionIntensity) const
{
    pos[0] = model->GetPos().x;
    pos[1] = model->GetPos().y;
    pos[2] = model->GetPos().z;
    ImGui::Begin(uiTitle.c_str(), 0, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoCollapse);
    ImGui::DragFloat3("Position\nX/Y/Z", pos, 0.01f,
        -999999999.0f, 999999999.0f);
    model->SetPos(pos[0], pos[1], pos[2]);
    rot[0] = model->GetRot().x;
    rot[1] = model->GetRot().y;
    rot[2] = model->GetRot().z;
    ImGui::DragFloat3("Rotation\nX/Y/Z", rot, 0.01f, 
        -999999999.0f, 999999999.0f);
    model->SetRot(rot[0], rot[1], rot[2]);
    scale[0] = model->GetScale().x;
    scale[1] = model->GetScale().y;
    scale[2] = model->GetScale().z;
    ImGui::DragFloat3("Scale\n/X/Y/Z", scale, 0.01f, 
        -999999999.0f, 999999999.0f);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::DragFloat("Light\nIntensity", Intensity, 0.01f,
        0.0f, 10000.0f);
    ImGui::DragFloat("Ambient\nIntensity", 
        ambientIntensity, 0.01f, 0.0f, 1000.0f);
    ImGui::DragFloat("Specular\nIntensity", 
        specularIntensity, 0.01f, 0.0f, 1000.0f);
    ImGui::DragFloat("Reflection\nIntensity", 
        reflectionIntensity, 0.01f, 0.0f, 1000.0f);
    if (ImGui::Button("Reset Position", ImVec2(200, 45)))
    {
        model->SetPos(0.0f, 0.0f, 0.0f);
        model->SetRot(0.0f, 0.0f, 0.0f);
        model->SetScale(1.0f, 1.0f, 1.0f);
    }
    ImGui::Checkbox("Normal Mapping", 
        reinterpret_cast<bool*>(normalMappingEnabled));
    ImGui::Checkbox("Reflection", reinterpret_cast<bool*>
        (reflectionEnabled));
    ImGui::End();
}

//Developer UI
void UI::DeveloperUI(std::string adapter_name, const
    std::string cpu_usage, const std::string fps, Camera3D*
    cam3d, bool* depthBufferEnabled, bool* blurEnabled,
    bool* wireframeEnabled,
    XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* 
    fogColor, float* fogStart, float* fogEnd, bool* vsync,
    bool* gridMapEnabled, XMFLOAT3* gridMapColor)
{
    static float wireCol[3] = { 1,1,1 };
    wireColor->x = wireCol[0];
    wireColor->y = wireCol[1];
    wireColor->z = wireCol[2];
    static float gridmapCol[3] = { 1,1,1 };
    gridMapColor->x = gridmapCol[0];
    gridMapColor->y = gridmapCol[1];
    gridMapColor->z = gridmapCol[2];
    static float fogCol[4] = { 1,1,1,1 };
    fogColor->x = fogCol[0];
    fogColor->y = fogCol[1];
    fogColor->z = fogCol[2];
    fogColor->w = fogCol[3];
    ImGui::Begin("Developer Menu", nullptr, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Citrus Graphics Renderer v0.1");
    ImGui::Text(fps.c_str());
    const std::string adapter = "GPU: " + adapter_name;
    ImGui::Text(adapter.c_str());
    ImGui::Text(cpu_usage.c_str());
    ImGui::Checkbox("Vsync", vsync);
    if (ImGui::Button("Reset Camera Position", ImVec2(200,
        45)))
    {
        cam3d->SetPosition(0.0f, 0.0f, -2.0f);
    }
    if (ImGui::Button("Reset Camera Rotation", ImVec2(200,
        45)))
    {
        cam3d->SetRotation(0.0f, 0.0f, 0.0f);
    }
    ImGui::Text("Filters");
    ImGui::Checkbox("Depth Buffer Shader", 
        depthBufferEnabled);
    ImGui::Checkbox("Blur Shader", blurEnabled);
    ImGui::Text("");
    ImGui::Checkbox("Fog Shader", fogEnabled);
    ImGui::DragFloat("Fog Start", fogStart, 0.01f, 0.0f, 
        2000.0f);
    ImGui::DragFloat("Fog End", fogEnd, 0.01f, 0.0f, 
        2000.0f);
    ImGui::ColorPicker4("Fog Color", fogCol);
    ImGui::Text("");
    ImGui::Checkbox("Wireframe", wireframeEnabled);
    ImGui::ColorPicker3("Wireframe\nColor", wireCol);
    ImGui::Text("");
    ImGui::Checkbox("Grid Map", gridMapEnabled);
    ImGui::ColorPicker3("Grid Map\nColor", gridmapCol);
    ImGui::End();
}
