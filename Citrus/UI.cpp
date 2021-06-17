#include "UI.h"
#include <Pdh.h>

//Classic UI
void UI::ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3]) const
{
    pos[0] = model->GetPos().x;
    pos[1] = model->GetPos().y;
    pos[2] = model->GetPos().z;
    ImGui::Begin(uiTitle.c_str(), 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::DragFloat3("Pos", pos, 0.01f, -999999.0f, 999999.0f);
    model->SetPos(pos[0], pos[1], pos[2]);
    rot[0] = model->GetRot().x;
    rot[1] = model->GetRot().y;
    rot[2] = model->GetRot().z;
    ImGui::DragFloat3("Rot", rot, 0.01f, -999999999.0f, 999999999.0f);
    model->SetRot(rot[0], rot[1], rot[2]);
    scale[0] = model->GetScale().x;
    scale[1] = model->GetScale().y;
    scale[2] = model->GetScale().z;
    ImGui::DragFloat3("Scale", scale, 0.01f, -999999999.0f, (999999999.0f));
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::End();
}

//Custom UI for point light
void UI::PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
    float* ambientIntensity, BOOL* normalMappingEnabled, float* specularlIntensity) const
{
    pos[0] = model->GetPos().x;
    pos[1] = model->GetPos().y;
    pos[2] = model->GetPos().z;
    ImGui::Begin(uiTitle.c_str(), 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::DragFloat3("Pos", pos, 0.01f, -999999999.0f, 999999999.0f);
    model->SetPos(pos[0], pos[1], pos[2]);
    rot[0] = model->GetRot().x;
    rot[1] = model->GetRot().y;
    rot[2] = model->GetRot().z;
    ImGui::DragFloat3("Rot", rot, 0.01f, -999999999.0f, 999999999.0f);
    model->SetRot(rot[0], rot[1], rot[2]);
    scale[0] = model->GetScale().x;
    scale[1] = model->GetScale().y;
    scale[2] = model->GetScale().z;
    ImGui::DragFloat3("Scale", scale, 0.01f, -999999999.0f, 999999999.0f);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::DragFloat("Light\nIntensity", Intensity, 0.01f, 0.0f, 10000.0f);
    ImGui::DragFloat("Ambient\nIntensity", ambientIntensity, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Specular\nIntensity", specularlIntensity, 0.01f, 0.0f, 10.0f);
    if (ImGui::Button("Reset Position"))
    {
        model->SetPos(0.0f, 0.0f, 0.0f);
        model->SetRot(0.0f, 0.0f, 0.0f);
        model->SetScale(1.0f, 1.0f, 1.0f);
    }
    ImGui::Checkbox("Normal Map", reinterpret_cast<bool*>(normalMappingEnabled));
    ImGui::End();
}

//Developer UI
void UI::DeveloperUI(std::string ms, const std::string cpu_usage, const std::string fps, Camera3D* cam3d, bool* depthBufferEnabled, bool* blurEnabled,
    bool* wireframeEnabled,
    XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* fogColor, float* fogStart, float* fogEnd)
{
    static float wireCol[3] = { 1,1,1 };
    wireColor->x = wireCol[0];
    wireColor->y = wireCol[1];
    wireColor->z = wireCol[2];
    static float fogCol[4] = { 1,1,1,1 };
    fogColor->x = fogCol[0];
    fogColor->y = fogCol[1];
    fogColor->z = fogCol[2];
    fogColor->w = fogCol[3];
    ImGui::Begin("Developer Menu", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Citrus Graphics Renderer v0.1");
    std::string ms_string = ms;
    ms_string += " ms/frame\n(";
    ms_string += fps;
    ImGui::Text(ms_string.c_str());
    ImGui::Text(cpu_usage.c_str());
    if (ImGui::Button("Reset Camera Position", ImVec2(200, 50)))
    {
        cam3d->SetPosition(0.0f, 0.0f, -2.0f);
        cam3d->SetRotation(0.0f, 0.0f, 0.0f);
    }
    ImGui::Text("Filters");
    ImGui::Checkbox("Depth Buffer Shader", depthBufferEnabled);
    ImGui::Checkbox("Blur Shader", blurEnabled);
    ImGui::Text("");
    ImGui::Checkbox("Fog Shader", fogEnabled);
    ImGui::DragFloat("Fog Start", fogStart, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat("Fog End", fogEnd, 0.01f, 0.0f, 100.0f);
    ImGui::ColorPicker4("Fog Color", fogCol);
    ImGui::Text("");
    ImGui::Checkbox("Wireframe", wireframeEnabled);
    ImGui::ColorPicker3("Wireframe\nColor", wireCol);
    ImGui::End();
}
