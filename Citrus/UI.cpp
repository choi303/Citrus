#include "UI.h"
#include <Pdh.h>
#include "Graphics.h"
#include "GameObject.h"
#include "App.h"
#include <shobjidl.h> 
#include <shellapi.h>

#pragma comment(lib, "shell32")

static bool applyVisiblity = false;
static bool can_render = true;
static bool uiVisiblity = true;

//Classic UI
void UI::ClassicUI(GameObject* gameObject, std::string uiTitle, float
    pos[3], float rot[3], float scale[3], bool& isDestroyed) const
{
    if (can_render)
    {
        pos[0] = gameObject->GetMesh()->GetPos().x;
        pos[1] = gameObject->GetMesh()->GetPos().y;
        pos[2] = gameObject->GetMesh()->GetPos().z;
        if (uiVisiblity)
        {
            if (!isDestroyed)
            {
                if (ImGui::Begin(uiTitle.c_str()))
                {
                    ImGui::DragFloat3("Position\nX/Y/Z", pos, 0.01f,
                        -999999.0f, 999999.0f);
                    gameObject->GetMesh()->SetPos(pos[0], pos[1], pos[2]);
                    rot[0] = gameObject->GetMesh()->GetRot().x;
                    rot[1] = gameObject->GetMesh()->GetRot().y;
                    rot[2] = gameObject->GetMesh()->GetRot().z;
                    ImGui::DragFloat3("Rotation\nX/Y/Z", rot, 0.01f,
                        -999999999.0f, 999999999.0f);
                    gameObject->GetMesh()->SetRot(rot[0], rot[1], rot[2]);
                    scale[0] = gameObject->GetMesh()->GetScale().x;
                    scale[1] = gameObject->GetMesh()->GetScale().y;
                    scale[2] = gameObject->GetMesh()->GetScale().z;
                    ImGui::DragFloat3("Scale\nX/Y/Z", scale, 0.01f,
                        -999999999.0f, (999999999.0f));
                    gameObject->GetMesh()->SetScale(scale[0], scale[1], scale[2]);
                    if (ImGui::Button("Reset Position and Rotation", ImVec2
                    (200, 45)))
                    {
                        gameObject->GetMesh()->SetPos(0.0f, 0.0f, 0.0f);
                        gameObject->GetMesh()->SetRot(0.0f, 0.0f, 0.0f);
                        gameObject->GetMesh()->SetScale(1.0f, 1.0f, 1.0f);
                    }
                    if (ImGui::Button("Destroy", ImVec2
                    (200, 45)))
                    {
                        gameObject->Destroy();
                        isDestroyed = true;
                    }
                }
                ImGui::End();
            }
        }
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
        if (uiVisiblity)
        {
            if (ImGui::Begin(uiTitle.c_str()))
            {
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
    }
}

//Developer UI
void UI::DeveloperUI(std::string adapter_name, const
    std::string cpu_usage, const std::string fps, Camera3D*
    cam3d, bool* wireframeEnabled,
    XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* 
    fogColor, float* fogStart, float* fogEnd, bool* vsync,
    bool* gridMapEnabled, XMFLOAT3* gridMapColor, Graphics* gfx, HWND hwnd, App* app, bool* msaaEnabled,
    bool* blurEnabled, float* blurIntensity, BOOL* ssaoEnabled, float* totalStrength, float* base,
    float* area, float* fallOff, float* radius, float* exposure, float* gamma, BOOL* toneMappingEnabled,
    float* bloomIntensity, BOOL* bloomEnabled, std::string& versionStr, BOOL* ssrEnabled,
    float* minRaySteps, float* reflectivity, D3D_DRIVER_TYPE& pDriverType)
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
        static std::string rendererPreviewValue = "";
        if (pDriverType == D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE)
            rendererPreviewValue = "GPU";
        else
            rendererPreviewValue = "CPU";
        std::string version = "Citrus Graphics Renderer v" + versionStr;

        static const char* rendererItems[] = { "CPU", "GPU" };
        static bool rendererSelected[sizeof(rendererItems)];

        if (uiVisiblity)
        {
            if (ImGui::Begin("Developer Menu"))
            {
                ImGui::Text(version.c_str());
                ImGui::Text(fps.c_str());
                ImGui::Text("Render\nDevice");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\n", rendererPreviewValue.c_str()))
                {
                    for (size_t i = 0; i < IM_ARRAYSIZE(rendererItems);
                        i++)
                    {
                        ImGui::Selectable(rendererItems[i], &rendererSelected[i]);
                        if (rendererSelected[i])
                        {
                            rendererPreviewValue = rendererItems[i];
                            if (rendererPreviewValue == "CPU")
                            {
                                if(pDriverType != D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE)
                                if (MessageBoxA(NULL, "This selection will change your rendering device and restart the application, are you sure you wanna change this?", "Warning", MB_YESNO | MB_ICONWARNING) == IDYES)
                                {
                                    pDriverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE;
                                    app->SaveValues();
                                    app->SaveValues();
                                    ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Debug\\Citrus.exe"), NULL, NULL, SW_SHOW);
                                    exit(0);
                                }
                            }
                            else if (rendererPreviewValue == "GPU")
                            {
                                if (pDriverType != D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE)
                                if (MessageBoxA(NULL, "This selection will change your rendering device and restart the application, are you sure you wanna change this?", "Warning", MB_YESNO | MB_ICONWARNING) == IDYES)
                                {
                                    pDriverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE;
                                    app->SaveValues();
                                    app->SaveValues();
                                    ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Debug\\Citrus.exe"), NULL, NULL, SW_SHOW);
                                    exit(0);
                                }
                            }
                        }
                    }
                    ImGui::EndCombo();
                }
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
                if (*blurEnabled)
                {
                    ImGui::DragFloat("Blur\nIntensity", blurIntensity, 0.01f, 0.1f, 5.0f);
                }
                if (*toneMappingEnabled)
                {
                    ImGui::DragFloat("Expsoure", exposure, 0.01f, 0.01f, 100.0f);
                    ImGui::DragFloat("Gamma", gamma, 0.01f, 0.01f, 100.0f);
                    if (ImGui::Button("Reset"))
                    {
                        *exposure = 1.690f;
                        *gamma = 0.750f;
                    }
                }
                if (*bloomEnabled)
                {
                    ImGui::DragFloat("Bloom\nIntensity", bloomIntensity, 0.01f, 0.0f, 1000.0f);
                }
                if (*ssrEnabled)
                {
                    ImGui::DragFloat("Min Ray Steps", minRaySteps, 0.01f, -100.0f, 999999.0f);
                    ImGui::DragFloat("Reflectivity", reflectivity, 0.01f, 0.0f, 1.0f);
                }
                if (*ssaoEnabled)
                {
                    ImGui::Text("");
                    ImGui::Text("SSAO Settings");
                    ImGui::DragFloat("Total\nStrength", totalStrength, 0.001f, -100.0f, 100.0f);
                    ImGui::DragFloat("Base", base, 0.0001f, -100.0f, 100.0f);
                    ImGui::DragFloat("Area", area, 0.0001f, -100.0f, 100.0f);
                    ImGui::DragFloat("Fall Off", fallOff, 0.0001f, -100.0f, 100.0f);
                    ImGui::DragFloat("Radius", radius, 0.0001f, -100.0f, 100.0f);
                    if (ImGui::Button("Reset"))
                    {
                        *base = 0.223f;
                        *area = 0.0000000000001f;
                        *fallOff = 0.000000001f;
                        *radius = 0.014f;
                    }
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
                
            }
            ImGui::End();
        }
    }
}

void UI::ToolBar(bool* gridMapEnabled, bool* 
    wireframeEnabled, bool* fogEnabled,
    bool* depthBufferEnabled, bool* blurEnabled, bool* msaaEnabled, App* app, bool* fxaaEnabled,
    bool* backfaceCulling, bool* frontfaceCulling,
    App* rApp, BOOL* alphaClip, BOOL* ssaoEnabled, BOOL* toneMappingEnabled,
    BOOL* bloomEnabled, std::vector<GameObject*>& pGameObjects, ID3D11Device* pDevice,
    ID3D11DeviceContext* pContext, int width, int height, BOOL* autoExposureEnabled,
    std::string& versionStr, BOOL* kuwaharaEnabled, BOOL* ssrEnabled)
{
    if (can_render)
    {
        std::string title = "";
        PWSTR pszFilePath = PWSTR("");
        if (uiVisiblity)
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Load Model (.obj)"))
                    {
                        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
                            COINIT_DISABLE_OLE1DDE);
                        if (SUCCEEDED(hr))
                        {
                            IFileOpenDialog* pFileOpen;

                            // Create the FileOpenDialog object.
                            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                            if (SUCCEEDED(hr))
                            {
                                COMDLG_FILTERSPEC ComDlgFS[1] = { L"Executable Files", L"*.obj;" };
                                pFileOpen->SetFileTypes(1, ComDlgFS);
                                pFileOpen->SetTitle(L"Select File (.obj)");
                                // Show the Open dialog box.
                                hr = pFileOpen->Show(NULL);

                                // Get the file name from the dialog box.
                                if (SUCCEEDED(hr))
                                {
                                    IShellItem* pItem;
                                    hr = pFileOpen->GetResult(&pItem);
                                    if (SUCCEEDED(hr))
                                    {
                                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                        pItem->Release();
                                    }
                                }
                                pFileOpen->Release();
                            }                   
                        }
                        std::wstring wPath = std::wstring(pszFilePath);
                        std::string sPath = std::string(wPath.begin(), wPath.end());
                        if(pszFilePath != PWSTR(""))
                        pGameObjects.push_back(new GameObject(pDevice, pContext, sPath, width, height, true));
                    }

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
                            app->SaveValues();
#ifdef _DEBUG
                            ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Debug\\Citrus.exe"), NULL, NULL, SW_SHOW);
#else
                            ShellExecuteA(NULL, NULL, static_cast<LPCSTR>("..\\x64\\Release\\Citrus.exe"), NULL, NULL, SW_SHOW);
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

                    if (*ssaoEnabled)
                        title = "SSAO: On";
                    else
                        title = "SSAO: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*ssaoEnabled)
                        {
                            *ssaoEnabled = false;
                        }
                        else
                        {
                            *ssaoEnabled = true;
                        }
                    }

                    if (*toneMappingEnabled)
                        title = "Tone Mapping: On";
                    else
                        title = "Tone Mapping: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*toneMappingEnabled)
                        {
                            *toneMappingEnabled = false;
                        }
                        else
                        {
                            *toneMappingEnabled = true;
                        }
                    }

                    if (*bloomEnabled)
                        title = "Bloom: On";
                    else
                        title = "Bloom: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*bloomEnabled)
                        {
                            *bloomEnabled = false;
                        }
                        else
                        {
                            *bloomEnabled = true;
                        }
                    }

                    if (*autoExposureEnabled)
                        title = "Auto Exposure: On";
                    else
                        title = "Auto Exposure: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*autoExposureEnabled)
                        {
                            *autoExposureEnabled = false;
                        }
                        else
                        {
                            *autoExposureEnabled = true;
                        }
                    }

                    if (*kuwaharaEnabled)
                        title = "Kuwahara: On";
                    else
                        title = "Kuwahara: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*kuwaharaEnabled)
                        {
                            *kuwaharaEnabled = false;
                        }
                        else
                        {
                            *kuwaharaEnabled = true;
                        }
                    }

                    if (*ssrEnabled)
                        title = "SSR: On";
                    else
                        title = "SSR: Off";
                    if (ImGui::MenuItem(title.c_str()))
                    {
                        if (*ssrEnabled)
                        {
                            *ssrEnabled = false;
                        }
                        else
                        {
                            *ssrEnabled = true;
                        }
                    }

                    ImGui::EndMenu();
                }
                

                if (ImGui::BeginMenu("About"))
                {
                    if (ImGui::MenuItem("Info"))
                    {
                        std::string aboutText = "Citrus Graphics Renderer v" + versionStr + "\nGitHub: https://github.com/choi303/Citrus";
                        Error::InfoLog(aboutText);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }
    }
}

void UI::TessellationTri(float* tessAmount)
{
    if (can_render)
    {
        if (uiVisiblity)
        {
            if (ImGui::Begin("Tessellation Tri"))
            {
                ImGui::SliderFloat("Tessellation\nAmount", tessAmount, 0.1f, 100.0f);
            }
            ImGui::End();
        }
    }
}

void UI::SetCanRendered(bool value)
{
    can_render = value;
}

void UI::SetVisiblity(bool value)
{
    uiVisiblity = value;
}

bool UI::GetVisiblity() noexcept
{
    return uiVisiblity;
}

void UI::ParticleUI(std::string uiTitle, float* mParticleDeviationX, float* mParticleDeviationY, float* mParticleDeviationZ, float* mParticleVelocity, float* mParticleVelocityVariation, float* mParticleSize, float* mAccumulatedTime, int* mCurrentParticleCount, float* lifeTime, bool* isKilled, bool* isLifetime,
    float* posX, float* posY, float* posZ)
{
    if (can_render)
    {
        if (uiVisiblity)
        {
            if (!*isKilled)
            {
                if (ImGui::Begin(uiTitle.c_str()))
                {
                    ImGui::Text("Particle Info");
                    ImGui::Text("Accumulated\nTime: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(*mAccumulatedTime).c_str());
                    ImGui::Text("Current particle count: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(*mCurrentParticleCount).c_str());
                    ImGui::Text("");
                    ImGui::Text("Particle Settings");
                    ImGui::DragFloat("Position\nX", posX, 0.01f, -99999.0f, 99999.0f);
                    ImGui::DragFloat("Position\nY", posY, 0.01f, -99999.0f, 99999.0f);
                    ImGui::DragFloat("Position\nZ", posZ, 0.01f, -99999.0f, 99999.0f);
                    ImGui::DragFloat("Deviation\nX", mParticleDeviationX, 0.01f, -1000.0f, 1000.0f);
                    ImGui::DragFloat("Deviation\nY", mParticleDeviationY, 0.01f, -1000.0f, 1000.0f);
                    ImGui::DragFloat("Deviation\nZ", mParticleDeviationZ, 0.01f, -1000.0f, 1000.0f);
                    ImGui::DragFloat("Velocity", mParticleVelocity, 0.01f, 0.0f, 1000.0f);
                    ImGui::DragFloat("Variation\nVelocity", mParticleVelocityVariation, 0.001f, -100.0f, 100.0f);
                    ImGui::DragFloat("Size", mParticleSize, 0.001f, -1000.0f, 1000.0f);
                    if(*isLifetime)
                    ImGui::DragFloat("Life\nTime", lifeTime, 0.01f, -1000.0f, 1000.0f);
                    ImGui::Checkbox("Life\nTime\nEnabled", isLifetime);
                    
                }
                ImGui::End();
            }
        }
    }
}

void UI::DirectionalLigth(XMFLOAT4* diffuseColor,
    XMFLOAT3* lightDirection, XMFLOAT4* ambientColor, float* ambientIntensity,
    BOOL* normalMapEnabled, float* specularIntensity, float* diffuseIntensity,
    BOOL* reflectionEnabled, float* reflectionIntensity, float* bias, BOOL* pcfEnabled,
    BOOL* emessiveEnabled, float* emessiveIntensity)
{
    if (can_render)
    {
        static float direction[3] = {0.300f, -1.0f, 1.6f};
        direction[0] = lightDirection->x;
        direction[1] = lightDirection->y;
        direction[2] = lightDirection->z;
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
        if (uiVisiblity)
        {
            if (ImGui::Begin("Directional Light"))
            {
                ImGui::DragFloat3("Light\nDirection",
                    direction, 0.0001f, -2000.0f, 2000.0f);
                lightDirection->x = direction[0];
                lightDirection->y = direction[1];
                lightDirection->z = direction[2];
                if (ImGui::Button("Reset"))
                {
                    lightDirection->x = 0.0f;
                    lightDirection->y = 0.0f;
                    lightDirection->z = 1.0f;
                }
                ImGui::Checkbox("Normal Map Enabled", reinterpret_cast<bool*>(normalMapEnabled));
                ImGui::Checkbox("Reflection Enabled", reinterpret_cast<bool*>(reflectionEnabled));
                ImGui::Checkbox("PCF Enabled", reinterpret_cast<bool*>(pcfEnabled));
                ImGui::Checkbox("Emessive Enabled", reinterpret_cast<bool*>(emessiveEnabled));
                ImGui::ColorEdit4("Light\nColor", diffuseCol);
                ImGui::ColorEdit4("Ambient\nColor", ambientCol);
                ImGui::DragFloat("Ambient\nIntensity", ambientIntensity, 0.01f, 0.0f, 50.0f);
                ImGui::DragFloat("Diffuse\nIntensity", diffuseIntensity, 0.01f, 0.0f, 2000.0f);
                ImGui::DragFloat("Specular\nIntensity", specularIntensity, 0.01f, 0.0f, 2000.0f);
                ImGui::DragFloat("Reflection\nIntensity", reflectionIntensity, 0.01f, 0.0f, 2000.0f);
                ImGui::DragFloat("Emessive\nIntensity", emessiveIntensity, 0.01f, 0.0f, 2000.0f);
                ImGui::DragFloat("Shadow\nBias", bias, 0.001f, -2000.0f, 2000.0f);
            }

            ImGui::End();
        }
    }
}

void UI::FireUI(float* posX, float* posY, float* posZ, float* sSpeedX, float* sSpeedY, float* sSpeedZ, float* scalesX, float* scalesY,
    float* scalesZ, float* distortion1X, float* distortion1Y, float* distortion2X, float* distortion2Y, float* distortion3X,
    float* distortion3Y, float* distortionScale, float* distortionBias, float* scaleX, float* scaleY, float* scaleZ)
{
    if (can_render)
    {
        if (uiVisiblity)
        {
            if (ImGui::Begin("Fire"))
            {
                ImGui::Text("Fire Settings");
                ImGui::DragFloat("Position\nX", posX, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Position\nY", posY, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Position\nZ", posZ, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scale\nX", scaleX, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scale\nY", scaleY, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scale\nZ", scaleZ, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scroll\nSpeed\nX", sSpeedX, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scroll\nSpeed\nY", sSpeedY, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scroll\nSpeed\nZ", sSpeedZ, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scales\nX", scalesX, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scales\nY", scalesY, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Scales\nZ", scalesZ, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 1\nX", distortion1X, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 1\nY", distortion1Y, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 2\nX", distortion2X, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 2\nY", distortion2Y, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 3\nX", distortion3X, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion 3\nY", distortion3Y, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion\nScale", distortionScale, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Distortion\nBias", distortionBias, 0.01f, -1000.0f, 1000.0f);
                
            }
                ImGui::End();
        }
    }
}

void UI::PbrUI(Model* pbrModel, float
    pos[3], float rot[3], float scale[3], XMFLOAT3* albedo, float* metallic,
    float* roughness, float* ao, ID3D11DeviceContext* pContext, std::shared_ptr<Texture> tAlbedo,
    std::shared_ptr<Texture> tNormal, std::shared_ptr<Texture> tMetallic,
    std::shared_ptr<Texture> tRoughness, std::shared_ptr<Texture> tAO)
{
    static float mAlbedo[3] = { 1,0,0 };
    albedo->x = mAlbedo[0];
    albedo->y = mAlbedo[1];
    albedo->z = mAlbedo[2];

    if (can_render)
    {
        if (uiVisiblity)
        {
            if (ImGui::Begin("Pbr Model"))
            {
                ImGui::DragFloat3("Position\nX/Y/Z", pos, 0.01f,
                    -999999.0f, 999999.0f);
				pbrModel->SetPos(pos[0], pos[1], pos[2]);
                rot[0] = pbrModel->GetRot().x;
                rot[1] = pbrModel->GetRot().y;
                rot[2] = pbrModel->GetRot().z;
                ImGui::DragFloat3("Rotation\nX/Y/Z", rot, 0.01f,
                    -999999999.0f, 999999999.0f);
                pbrModel->SetRot(rot[0], rot[1], rot[2]);
                scale[0] = pbrModel->GetScale().x;
                scale[1] = pbrModel->GetScale().y;
                scale[2] = pbrModel->GetScale().z;
                ImGui::DragFloat3("Scale\nX/Y/Z", scale, 0.01f,
                    -999999999.0f, (999999999.0f));
                pbrModel->SetScale(scale[0], scale[1], scale[2]);
                /*ImGui::ColorEdit3("Albedo", mAlbedo);
                ImGui::DragFloat("Metallic", metallic, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Roughness", roughness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("AO", ao, 0.01f, 0.0f, 1.0f);*/

                std::shared_ptr<Texture> pAlbedo = tAlbedo;
                std::shared_ptr<Texture> pNormal = tNormal;
                std::shared_ptr<Texture> pMetallic = tMetallic;
                std::shared_ptr<Texture> pRoughness = tRoughness;
                std::shared_ptr<Texture> pAO = tAO;
                ImVec2&& texSize = ImVec2(256, 256);
                ImGui::Text("Albedo Texture");
                ImGui::SameLine();
                if (ImGui::Button("Open Albedo Path")) {
                    ShellExecuteA(NULL, "open", pAlbedo->GetDirectory().c_str(), NULL, NULL, SW_SHOW);
                    ShellExecuteA(NULL, "open", pAlbedo->GetPath().c_str(), NULL, NULL, SW_SHOW);
                }
                ImGui::Image(reinterpret_cast<void*>(pAlbedo->GetShaderResourceView()), texSize);
                ImGui::Text("Normal Texture");
                ImGui::SameLine();
                if (ImGui::Button("Open Normal Path")) {
                    ShellExecuteA(NULL, "open", pNormal->GetDirectory().c_str(), NULL, NULL, SW_SHOW);
                    ShellExecuteA(NULL, "open", pNormal->GetPath().c_str(), NULL, NULL, SW_SHOW);
                }
                ImGui::Image(reinterpret_cast<void*>(pNormal->GetShaderResourceView()), texSize);
                ImGui::Text("Metallic Texture");
                ImGui::SameLine();
                if (ImGui::Button("Open Metallic Path")) {
                    ShellExecuteA(NULL, "open", pMetallic->GetDirectory().c_str(), NULL, NULL, SW_SHOW);
                    ShellExecuteA(NULL, "open", pMetallic->GetPath().c_str(), NULL, NULL, SW_SHOW);
                }
                ImGui::Image(reinterpret_cast<void*>(pMetallic->GetShaderResourceView()), texSize);
                ImGui::Text("Roughness Texture");
                ImGui::SameLine();
                if (ImGui::Button("Open Roughness Path")) {
                    ShellExecuteA(NULL, "open", pRoughness->GetDirectory().c_str(), NULL, NULL, SW_SHOW);
                    ShellExecuteA(NULL, "open", pRoughness->GetPath().c_str(), NULL, NULL, SW_SHOW);
                }
                ImGui::Image(reinterpret_cast<void*>(pRoughness->GetShaderResourceView()), texSize);
                ImGui::Text("Ambient Occlusion Texture");
                ImGui::SameLine();
                if (ImGui::Button("Open AO Path")) {
                    ShellExecuteA(NULL, "open", pAO->GetDirectory().c_str(), NULL, NULL, SW_SHOW);
                    ShellExecuteA(NULL, "open", pAO->GetPath().c_str(), NULL, NULL, SW_SHOW);
                }
                ImGui::Image(reinterpret_cast<void*>(pAO->GetShaderResourceView()), texSize);
            }
            ImGui::End();
        }
    }
}
