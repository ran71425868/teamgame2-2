#include "CrossHair.h"
#include "System/Graphics.h"
#include <WICTextureLoader.h>
#include <string>
using namespace DirectX;

CrossHair::CrossHair() {}
CrossHair::~CrossHair() {}

bool CrossHair::Initialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    HRESULT hr = CreateWICTextureFromFile(
        device,
        L"Data/Sprite/kurosu1.png", // 画像のパス（Unicode注意）
        resource.GetAddressOf(),
        textureSRV.GetAddressOf()
    );

    if (FAILED(hr)) {
        return false;
    }

    // サイズを取得
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
    if (SUCCEEDED(resource.As(&tex2D))) {
        D3D11_TEXTURE2D_DESC desc;
        tex2D->GetDesc(&desc);
        width = static_cast<float>(desc.Width);
        height = static_cast<float>(desc.Height);
    }

    return true;
}

void CrossHair::Render()
{
    if (!textureSRV) return;

    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();

    float x = (screenWidth - width) * 0.5f;
    float y = (screenHeight - height) * 0.5f;

    // 簡易スプライト描画関数があると仮定
    Graphics::Instance().DrawSprite(textureSRV.Get(), x, y, width, height);
}
