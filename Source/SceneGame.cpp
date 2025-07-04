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
/*#include <WICTextureLoader.h>*/ // DirectXTKの画像読み込み
#include <wrl/client.h>
using namespace DirectX;

// クロスヘア用
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> crosshairSRV;
float crosshairWidth = 0;
float crosshairHeight = 0;

// 初期化
void SceneGame::Initialize()
{
    // ステージ初期化
    stage = new Stage();

    // プレイヤー初期化
    Player::Instance().Initializa();

    // カメラ初期設定
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

    // エネミー初期化
    EnemyManager& enemyManager = EnemyManager::Instance();
    for (int i = 0; i < 2; ++i)
    {
        EnemySlime* slime = new EnemySlime();
        slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
        slime->SetTerritory(slime->GetPosition(), 10.0f);
        enemyManager.Register(slime);
    }
}

// 終了化
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

    crosshairSRV.Reset(); // 解放
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    // カメラターゲット追従
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

// 描画処理
void SceneGame::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
    ModelRenderer* modelRenderer = graphics.GetModelRenderer();

    // 描画準備
    RenderContext rc;
    rc.deviceContext = dc;
    rc.lightDirection = { 0.0f, -1.0f, 0.0f };
    rc.renderState = graphics.GetRenderState();

    Camera& camera = Camera::Instance();
    rc.view = camera.GetView();
    rc.projection = camera.GetProjection();

    // 3D描画
    {
        stage->Render(rc, modelRenderer);
        Player::Instance().Render(rc, modelRenderer);
        EnemyManager::Instance().Render(rc, modelRenderer);
        EffectManager::Instance().Render(rc.view, rc.projection);
    }

    // デバッグ描画
    {
        Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
        EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
    }

    // 2Dスプライト描画（クロスヘア）
    {
        if (crosshairSRV) {
            float screenWidth = graphics.GetScreenWidth();
            float screenHeight = graphics.GetScreenHeight();

            float x = (screenWidth - crosshairWidth) * 0.5f;
            float y = (screenHeight - crosshairHeight) * 0.5f;

            // ImGuiの代用：またはDrawSpriteがあればそれを使う
            ImGui::GetBackgroundDrawList()->AddImage(
                crosshairSRV.Get(),
                ImVec2(x, y),
                ImVec2(x + crosshairWidth, y + crosshairHeight)
            );
        }
    }
}

// GUI描画
void SceneGame::DrawGUI()
{
    Player::Instance().DrawDebugGUI();
}
