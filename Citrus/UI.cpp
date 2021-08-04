#include "UI.h"
#include <Pdh.h>
#include "Graphics.h"
#include "App.h"

static bool applyVisiblity = false;
static bool can_render = true;

//Classic UI
void UI::ClassicUI(Model* model, std::string uiTitle, float
    pos[3], float rot[3], float scale[3]) const
{
    if (can_render)
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
}

//Custom UI for point light
void UI::PointLight(Model* model, const std::string 
    uiTitle, float pos[3], float rot[3], float scale[3],
    float* Intensity,
    float* ambientIntensity, BOOL* normalMappingEnabled,
    float* specularIntensity, BOOL* reflectionEnabled, 
    float* reflectionIntensity, XMFLOAT3* lightColor) const
{
    if (can_render)
    {
        static float lightColorF[3] = { 1,1,1 };
        lightColor->x = lightColorF[0];
        lightColor->y = lightColorF[1];
        lightColor->z = lightColorF[2];

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
        ImGui::ColorEdit3("Light Color", lightColorF);
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
    if (can_render)
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

        static const char* items[] = { "2", "4",
            "8" };
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
        {
            ImGui::Text("Blur Intensity");
            ImGui::DragFloat("", blurIntensity, 0.01f, 0.1f, 5.0f);
        }
        if (*fogEnabled)
            ImGui::DragFloat("Fog Start", fogStart, 0.01f, 0.0f,
                2000.0f);
        if (*fogEnabled)
            ImGui::DragFloat("Fog End", fogEnd, 0.01f, 0.0f,
                2000.0f);
        if (*fogEnabled)
            ImGui::ColorPicker4("Fog Color", fogCol);
        if (*wireframeEnabled)
            ImGui::ColorPicker3("Wireframe\nColor", wireCol);
        if (*gridMapEnabled)
            ImGui::ColorPicker3("Grid Map\nColor", gridmapCol);
        ImGui::End();
    }
}

void UI::ToolBar(bool* gridMapEnabled, bool* 
    wireframeEnabled, bool* fogEnabled,
    bool* depthBufferEnabled, bool* blurEnabled, bool* msaaEnabled, App* app, bool* fxaaEnabled,
    bool* backfaceCulling, bool* frontfaceCulling,
    App* rApp, BOOL* alphaClip)
{
    if (can_render)
    {
        std::string title = "";
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save Scene Settings"))
                {
                    rApp->SaveValues();
                    Error::InfoLog("Scene settings successfully saved.");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Editor"))
            {
                if (*gridMapEnabled)
                    title = "Grid Map: On";
                else
                    title = "Grid Map: Off";
                if (ImGui::MenuItem(title.c_str()))
                {
                    if (*gridMapEnabled)
                    {
                        *gridMapEnabled = false;
                    }
                    else
                    {
                        *gridMapEnabled = true;
                    }
                }

                if (*wireframeEnabled)
                    title = "Wireframe: On";
                else
                    title = "Wireframe: Off";
                if (ImGui::MenuItem(title.c_str()))
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

                if (*alphaClip)
                    title = "Alpha Clip: On";
                else
                    title = "Alpha Clip: Off";
                if (ImGui::MenuItem(title.c_str()))
                {
                    if (*alphaClip)
                    {
                        *alphaClip = false;
                    }
                    else
                    {
                        *alphaClip = true;
                    }
                }

                if (ImGui::BeginMenu("Culling"))
                {
                    if (*backfaceCulling)
                        title = "Back-face Culling: On";
                    else
                        title = "Back-face Culling: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*backfaceCulling)
                        {
                            *backfaceCulling = false;
                        }
                        else
                        {
                            *backfaceCulling = true;
                        }
                    }

                    if (*frontfaceCulling)
                        title = "Front-face Culling: On";
                    else
                        title = "Front-face Culling: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*frontfaceCulling)
                        {
                            *frontfaceCulling = false;
                        }
                        else
                        {
                            *frontfaceCulling = true;
                        }
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Effects"))
            {
                if (*fogEnabled)
                    title = "Fog: On";
                else
                    title = "Fog: Off";
                if (ImGui::MenuItem(title.c_str()))
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

                if (*blurEnabled)
                    title = "Blur: On";
                else
                    title = "Blur: Off";
                if (ImGui::MenuItem(title.c_str()))
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

                if (*depthBufferEnabled)
                    title = "Depth Buffer: On";
                else
                    title = "Depth Buffer: Off";
                if (ImGui::MenuItem(title.c_str()))
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

                if (*msaaEnabled && applyVisiblity)
                    title = "MSAA: Partly On";
                else if (*msaaEnabled && !applyVisiblity)
                    title = "MSAA: On";
                else
                    title = "MSAA: Off";
                if (ImGui::MenuItem(title.c_str()))
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

                if (*fxaaEnabled)
                    title = "FXAA: On";
                else
                    title = "FXAA: Off";
                if (ImGui::MenuItem(title.c_str()))
                {
                    if (*fxaaEnabled)
                    {
                        *fxaaEnabled = false;
                    }
                    else
                    {
                        *fxaaEnabled = true;
                    }
                }

                ImGui::EndMenu();
            }

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

void UI::SetCanRendered(bool value)
{
    can_render = value;
}

void UI::DirectionalLigth(XMFLOAT4* diffuseColor,
    XMFLOAT3* lightDirection, XMFLOAT4* ambientColor, float* ambientIntensity,
    BOOL* normalMapEnabled, float* specularIntensity, float* diffuseIntensity,
    BOOL* reflectionEnabled, float* reflectionIntensity, float* bias, BOOL* pcfEnabled)
{
    if (can_render)
    {
        static float direction[3] = { 1.550,-1,0 };
        lightDirection->x = direction[0];
        lightDirection->y = direction[1];
        lightDirection->z = direction[2];
        static float diffuseCol[4] = { 1,1,1,1 };
        diffuseColor->x = diffuseCol[0];
        diffuseColor->y = diffuseCol[1];
        diffuseColor->z = diffuseCol[2];
        diffuseColor->w = diffuseCol[3];
        static float ambientCol[4] = { 1,1,1,1 };
        ambientColor->x = ambientCol[0];
        ambientColor->y = ambientCol[1];
        ambientColor->z = ambientCol[2];
        ambientColor->w = ambientCol[3];
        if (ImGui::Begin("Directional Light", nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse))
        {
            ImGui::DragFloat3("Light\nDirection",
                direction, 0.0001f, -2000.0f, 2000.0f);
            if (ImGui::Button("Reset"))
            {
                direction[0] = 0.0f;
                direction[1] = 0.0f;
                direction[2] = 1.0f;
            }
            ImGui::Checkbox("Normal Map\nEnabled", reinterpret_cast<bool*>(normalMapEnabled));
            ImGui::Checkbox("Reflection\nEnabled", reinterpret_cast<bool*>(reflectionEnabled));
            ImGui::Checkbox("PCF\nEnabled", reinterpret_cast<bool*>(pcfEnabled));
            ImGui::ColorEdit4("Light\nColor", diffuseCol);
            ImGui::ColorEdit4("Ambient\nColor", ambientCol);
            ImGui::DragFloat("Ambient\nIntensity", ambientIntensity, 0.01f, 0.0f, 50.0f);
            ImGui::DragFloat("Diffuse\nIntensity", diffuseIntensity, 0.01f, 0.0f, 2000.0f);
            ImGui::DragFloat("Specular\nIntensity", specularIntensity, 0.01f, 0.0f, 2000.0f);
            ImGui::DragFloat("Reflection\nIntensity", reflectionIntensity, 0.01f, 0.0f, 2000.0f);
            ImGui::DragFloat("Shadow\nBias", bias, 0.001f, -2000.0f, 2000.0f);
            ImGui::End();
        }
    }
}
