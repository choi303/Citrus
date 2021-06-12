#include "UI.h"

//Classic UI
void UI::ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], bool* wireframe) const
{
    model->SetPos(pos[0], pos[1], pos[2]);
    model->SetRot(rot[0], rot[1], rot[2]);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::Begin(uiTitle.c_str(), 0, ImGuiWindowFlags_NoMove);
    ImGui::DragFloat3("Pos", pos, 0.01f, -999999.0f, 999999.0f);
    ImGui::DragFloat3("Rot", rot, 0.01f, -999999999.0f, 999999999.0f);
    ImGui::DragFloat3("Scale", scale, 0.01f, -999999999.0f, (999999999.0f));
    ImGui::Checkbox("Wireframe", wireframe);
    ImGui::End();
}

//Custom UI for point light
void UI::PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
    float* ambientIntensity, BOOL* normalMappingEnabled, float* specularlIntensity) const
{
    model->SetPos(pos[0], pos[1], pos[2]);
    model->SetRot(rot[0], rot[1], rot[2]);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::Begin(uiTitle.c_str(), 0, ImGuiWindowFlags_NoMove);
    ImGui::DragFloat3("Pos", pos, 0.01f, -999999999.0f, 999999999.0f);
    ImGui::DragFloat3("Rot", rot, 0.01f, -999999999.0f, 999999999.0f);
    ImGui::DragFloat3("Scale", scale, 0.01f, -999999999.0f, 999999999.0f);
    ImGui::DragFloat("Light\nIntensity", Intensity, 0.01f, 0.0f, 10000.0f);
    ImGui::DragFloat("Ambient\nIntensity", ambientIntensity, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Specular\nIntensity", specularlIntensity, 0.01f, 0.0f, 10.0f);
    ImGui::Checkbox("Normal Map", reinterpret_cast<bool*>(normalMappingEnabled));
    ImGui::End();
}

//Developer UI
void UI::DeveloperUI(const std::string fps, Camera3D* cam3d)
{
    ImGui::Begin("Developer Menu", 0, ImGuiWindowFlags_NoMove);
    ImGui::Text("Citrus Graphics Renderer v0.1");
    ImGui::Text(fps.c_str());
    if (ImGui::Button("Reset Camera Position", ImVec2(200, 50)))
    {
        cam3d->SetPosition(0.0f, 0.0f, -2.0f);
        cam3d->SetRotation(0.0f, 0.0f, 0.0f);
    }
    ImGui::End();
}
