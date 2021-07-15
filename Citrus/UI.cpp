#include "UI.h"
#include <Pdh.h>
#include "Graphics.h"
#include "App.h"

static bool applyVisiblity = false;

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
    cam3d, bool* wireframeEnabled,
    XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* 
    fogColor, float* fogStart, float* fogEnd, bool* vsync,
    bool* gridMapEnabled, XMFLOAT3* gridMapColor, Graphics* gfx, HWND hwnd, App* app, bool* msaaEnabled,
    bool* blurEnabled, float* blurIntensity)
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

    static const char* items[] = {"2", "4",
        "8"};
    static bool selected[sizeof(items)];
    static std::string previewValue = std::to_string(gfx->msaaQuality);

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
    if (*msaaEnabled)
    {
        ImGui::Text("Msaa Quality");
        if (ImGui::BeginCombo("", std::to_string(gfx->msaaQuality).c_str()))
        {
            for (size_t i = 0; i < IM_ARRAYSIZE(items);
                i++)
            {
                ImGui::Selectable(items[i], &selected[i]);
                if (selected[i])
                {
                    gfx->msaaQuality = std::atoi(items[i]);
                    applyVisiblity = true;
                }
            }

            ImGui::EndCombo();
        }
        if (applyVisiblity)
        {
            if (ImGui::Button("Apply"))
            {
                app->SaveValues();

            #ifdef _DEBUG
                ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Debug\\Citrus.exe"), NULL, NULL, SW_SHOW);
            #else
                system("..\\x64\\Release\\Citrus.exe");
            #endif 

                applyVisiblity = false;
                exit(0);
            }
            ImGui::Text("");
        }
    }
    if (*blurEnabled)
    ImGui::DragFloat("Blue Intensity", blurIntensity, 0.01f, 0.0f, 200.0f);
    if (*fogEnabled)
    ImGui::DragFloat("Fog Start", fogStart, 0.01f, 0.0f, 
        2000.0f);
    if (*fogEnabled)
    ImGui::DragFloat("Fog End", fogEnd, 0.01f, 0.0f, 
        2000.0f);
    if(*fogEnabled)
    ImGui::ColorPicker4("Fog Color", fogCol);
    if (*wireframeEnabled)
    ImGui::ColorPicker3("Wireframe\nColor", wireCol);
    if(*gridMapEnabled)
    ImGui::ColorPicker3("Grid Map\nColor", gridmapCol);
    ImGui::End();
}

void UI::ToolBar(bool* gridMapEnabled, bool* 
    wireframeEnabled, bool* fogEnabled,
    bool* depthBufferEnabled, bool* blurEnabled, bool* msaaEnabled, App* app)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Grid Map"))
            {
                if(*gridMapEnabled)
                {
                    *gridMapEnabled = false;
                }
                else
                {
                    *gridMapEnabled = true;
                }
            }

            if (ImGui::MenuItem("Wireframe"))
            {
                if (*wireframeEnabled)
                {
                    *wireframeEnabled = false;
                }
                else
                {
                    *wireframeEnabled = true;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Effects"))
        {
            if (ImGui::MenuItem("Fog Effect"))
            {
                if (*fogEnabled)
                {
                    *fogEnabled = false;
                }
                else
                {
                    *fogEnabled = true;
                }
            }

            if (ImGui::MenuItem("Blur Effect"))
            {
                if (*blurEnabled)
                {
                    *blurEnabled = false;
                }
                else
                {
                    *blurEnabled = true;
                }
            }

            if (ImGui::MenuItem("Depth Effect"))
            {
                if (*depthBufferEnabled)
                {
                    *depthBufferEnabled = false;
                }
                else
                {
                    *depthBufferEnabled = true;
                }
            }

            if (ImGui::MenuItem("MSAA"))
            {
                if (*msaaEnabled)
                {
                    *msaaEnabled = false;
                    app->SaveValues();

#ifdef _DEBUG
                    ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Debug\\Citrus.exe"), NULL, NULL, SW_SHOW);
#else
                    system("..\\x64\\Release\\Citrus.exe");
#endif 

                    exit(0);
                }
                else
                {
                    *msaaEnabled = true;
                    applyVisiblity = true;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::MenuItem("Info"))
            {
                Error::InfoLog("Citrus Graphics Renderer v0.1\nGitHub: https://github.com/choi303/Citrus");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
