#pragma once
#include "GfxBase.h"

struct DirectLight
{
    XMFLOAT4 ambientColor;
    XMFLOAT4 diffuseColor;
    XMFLOAT3 lightDirection;
    float ambientIntensity;
    BOOL normalMapEnabled;
    float specularIntensityC;
    float diffuseIntensityC;
    bool reflectionEnabled;
    float reflectionIntensity;
    float biasC;
    BOOL pcfEnabled;
    BOOL alphaClip;
    BOOL normals;
    BOOL emessiveEnabled;
    float emessiveIntensity;
    float pad[1];
};

struct shadowmap {
    XMMATRIX shadowView;
    XMMATRIX shadowProj;
};

struct LightColor
{
    XMFLOAT4 color;
};

class DirectionalLight
{
public:
    DirectionalLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        int width, int height);
    DirectionalLight(DirectionalLight& rhs) {}
    ~DirectionalLight() {}
    Camera3D GetLightCamera();
    XMFLOAT3 GetDirection() const noexcept;
    float GetAmbientIntensity() const noexcept;
    BOOL GetNormalMapEnabled() const noexcept;
    BOOL GetReflectionEnabled() const noexcept;
    BOOL GetPCFEnabled() const noexcept;
    static BOOL* GetAlphaCEnabled();
    static BOOL* GetEmessiveEnabled();
    float GetSpecularIntensity() const noexcept;
    float GetDiffuseIntensity() const noexcept;
    float GetReflectionIntensity() const noexcept;
    float GetEmessiveIntensity() const noexcept;
    float GetBias() const noexcept;
    void SetDireciton(XMFLOAT3 value);
    void SetDireciton(float x,float y,float z);
    void SetAmbientIntensity(float value);
    void SetNormalMapEnabled(BOOL value);
    void SetReflectionEnabled(BOOL value);
    void SetPCFEnabled(BOOL value);
    void SetAlphaCEnabled(BOOL value);
    void SetNormalsEnabled(BOOL value);
    void SetSpecularIntensity(float value);
    void SetDiffuseIntensity(float value);
    void SetReflectionIntensity(float value);
    void SetBias(float value);
    void BindCB(Camera3D cam);
    mutable std::vector<std::string> pDirectLightSavedItems;
    mutable SaveSystem directLightSettings;
private:
    std::unique_ptr<CBuffer<DirectLight>> mLightBuffer;
    std::unique_ptr<CBuffer<LightColor>> mLightColor;
    wrl::ComPtr<ID3D11Device> mDevice;
    wrl::ComPtr<ID3D11DeviceContext> mContext;
    std::unique_ptr<CBuffer<shadowmap>> mShadowCBuffer;
    VertexShader pVS;
    PixelShader pPS;
    std::unique_ptr<InputLayout> pLayout;
    Model pModel;
    Camera3D mLightCam;
    UI mUI;
    int mWidth;
    int mHeight;
};

