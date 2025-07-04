#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class CrossHair {
public:
    CrossHair();
    ~CrossHair();

    // �������i�摜�ǂݍ��݁j
    bool Initialize();

    // �`��i��ʒ����Ɂj
    void Render();

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
    float width = 0.0f;
    float height = 0.0f;
};
