#include "System/Graphics.h"
#include "SceneTutorial.h"
#include "SceneResult.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Character.h"
#include "EnemySlime.h"
#include "Player.h"
#include "EffectManager.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include <imgui.h>
#include <wrl/client.h>
using namespace DirectX;

// 初期化
void SceneTutorial::Initialize()
{
    // ステージ初期化
    stage = new Stage1();

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
    
}

// 終了化
void SceneTutorial::Finalize()
{
    

    if (cameraController != nullptr) {
        delete cameraController;
        cameraController = nullptr;
    }

    Player::Instance().Finalize();

    if (stage != nullptr) {
        delete stage;
        stage = nullptr;
    }
}

// 更新処理
void SceneTutorial::Update(float elapsedTime)
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
void SceneTutorial::Render()
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
        
    }
}

// GUI描画
void SceneTutorial::DrawGUI()
{
    Player::Instance().DrawDebugGUI();
}
