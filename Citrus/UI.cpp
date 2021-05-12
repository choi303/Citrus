#include "UI.h"

//Classic UI
void UI::ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3])
{
    model->SetPos(pos[0], pos[1], pos[2]);
    model->SetRot(rot[0], rot[1], rot[2]);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::Begin(uiTitle.c_str());
    ImGui::DragFloat3("Pos", pos, 0.1f, (float)-999999999, (float)999999999);
    ImGui::DragFloat3("Rot", rot, 0.1f, (float)-999999999, (float)999999999);
    ImGui::DragFloat3("Scale", scale, 0.1f, (float)-999999999, (float)999999999);
    ImGui::End();
}

//Custom UI for point light
void UI::PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity)
{
    model->SetPos(pos[0], pos[1], pos[2]);
    model->SetRot(rot[0], rot[1], rot[2]);
    model->SetScale(scale[0], scale[1], scale[2]);
    ImGui::Begin(uiTitle.c_str());
    ImGui::DragFloat3("Pos", pos, 0.1f, (float)-999999999, (float)999999999);
    ImGui::DragFloat3("Rot", rot, 0.1f, (float)-999999999, (float)999999999);
    ImGui::DragFloat3("Scale", scale, 0.1f, (float)-999999999, (float)999999999);
    ImGui::DragFloat("Light\nIntensity", Intensity, 0.01f, 0.0f, 1000.0f);
    ImGui::End();
}

