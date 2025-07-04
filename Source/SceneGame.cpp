#include "System/Graphics.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Player.h"
#include "EffectManager.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include <imgui.h>
/*#include <WICTextureLoader.h>*/ // DirectXTK�̉摜�ǂݍ���
#include <wrl/client.h>
using namespace DirectX;

// �N���X�w�A�p
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> crosshairSRV;
float crosshairWidth = 0;
float crosshairHeight = 0;

// ������
void SceneGame::Initialize()
{
    // �X�e�[�W������
    stage = new Stage();

    // �v���C���[������
    Player::Instance().Initializa();

    // �J���������ݒ�
    Graphics& graphics = Graphics::Instance();
    Camera& camera = Camera::Instance();
    camera.SetLookAt(
        DirectX::XMFLOAT3(0, 10, -10),
        DirectX::XMFLOAT3(0, 0, 0),
        DirectX::XMFLOAT3(0, 1, 0)
    );
    camera.SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),
        graphics.GetScreenWidth() / graphics.GetScreenHeight(),
        0.1f,
        1000.0f
    );

    cameraController = new CameraController;

    // �G�l�~�[������
    EnemyManager& enemyManager = EnemyManager::Instance();
    for (int i = 0; i < 2; ++i)
    {
        EnemySlime* slime = new EnemySlime();
        slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
        slime->SetTerritory(slime->GetPosition(), 10.0f);
        enemyManager.Register(slime);
    }
}

// �I����
void SceneGame::Finalize()
{
    EnemyManager::Instance().Clear();

    if (cameraController != nullptr) {
        delete cameraController;
        cameraController = nullptr;
    }

    Player::Instance().Finalize();

    if (stage != nullptr) {
        delete stage;
        stage = nullptr;
    }

    crosshairSRV.Reset(); // ���
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    // �J�����^�[�Q�b�g�Ǐ]
    DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
    target.y += 0.5f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime);

    stage->Update(elapsedTime);
    Player::Instance().Update(elapsedTime);
    EnemyManager::Instance().Update(elapsedTime);
    EffectManager::Instance().Update(elapsedTime);

    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
    }
}

// �`�揈��
void SceneGame::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
    ModelRenderer* modelRenderer = graphics.GetModelRenderer();

    // �`�揀��
    RenderContext rc;
    rc.deviceContext = dc;
    rc.lightDirection = { 0.0f, -1.0f, 0.0f };
    rc.renderState = graphics.GetRenderState();

    Camera& camera = Camera::Instance();
    rc.view = camera.GetView();
    rc.projection = camera.GetProjection();

    // 3D�`��
    {
        stage->Render(rc, modelRenderer);
        Player::Instance().Render(rc, modelRenderer);
        EnemyManager::Instance().Render(rc, modelRenderer);
        EffectManager::Instance().Render(rc.view, rc.projection);
    }

    // �f�o�b�O�`��
    {
        Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
        EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
    }

    // 2D�X�v���C�g�`��i�N���X�w�A�j
    {
        if (crosshairSRV) {
            float screenWidth = graphics.GetScreenWidth();
            float screenHeight = graphics.GetScreenHeight();

            float x = (screenWidth - crosshairWidth) * 0.5f;
            float y = (screenHeight - crosshairHeight) * 0.5f;

            // ImGui�̑�p�F�܂���DrawSprite������΂�����g��
            ImGui::GetBackgroundDrawList()->AddImage(
                crosshairSRV.Get(),
                ImVec2(x, y),
                ImVec2(x + crosshairWidth, y + crosshairHeight)
            );
        }
    }
}

// GUI�`��
void SceneGame::DrawGUI()
{
    Player::Instance().DrawDebugGUI();
}
