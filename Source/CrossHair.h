#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class CrossHair {
public:
    CrossHair();
    ~CrossHair();

    // 初期化（画像読み込み）
    bool Initialize();

    // 描画（画面中央に）
    void Render();

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
    float width = 0.0f;
    float height = 0.0f;
};
