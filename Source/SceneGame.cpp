#include "System/Graphics.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Player.h"
#include "EffectManager.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include <imgui.h>
#include "PropManager.h"
#include "ItemManager.h"
#include "Light.h"
#include "Mirror.h"
#include "Fan.h"
#include "Door.h"
#include "Panel.h"
/*#include <WICTextureLoader.h>*/ // DirectXTKの画像読み込み
#include <wrl/client.h>
using namespace DirectX;



// 初期化
void SceneGame::Initialize()
{

    Prop* prop5 = new Prop();
    prop5->SetPosition({ -15,2,-3 });
    PropManager::Instance().Register(prop5);

    // ステージ初期化
    stage = new Stage();

    // プレイヤー初期化
    Player::Instance().Initializa();
    Player::Instance().SetPosition({5,3,7 });
    /*-17,4.0f,0*/
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

    ItemManager& itemManager = ItemManager::Instance();

    Fan* fan = new Fan();
    fan->SetPosition({ 0,2.9,-9 });
    fan->SetAngle({ 0,-80,0 });
    itemManager.Register(fan);

    Door* door = new Door();
    door->SetPosition({ 9,0,-4 });
    itemManager.Register(door);

    /*Panel* panel = new Panel();
    panel->SetPosition({ 5,3,7 });
    itemManager.Register(panel);*/

    Panel* panel2 = new Panel();
    panel2->SetPosition({ 5,4,-2 });
    panel2->SetAngle({ 0,-80,0 });
    itemManager.Register(panel2);
}

// 終了化
void SceneGame::Finalize()
{
    EnemyManager::Instance().Clear();

    cameraController->Finalize();
    if (cameraController != nullptr) {
        delete cameraController;
        cameraController = nullptr;
    }

    Player::Instance().Finalize();

    if (stage != nullptr) {
        delete stage;
        stage = nullptr;
    }

    PropManager::Instance().Clear();
    ItemManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
    // カメラターゲット追従
    DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
    target.y += 0.5f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime);

    //カメラの角度をプレイヤーの角度と同期させる
    DirectX::XMFLOAT3 angle = cameraController->GetAngle();
    Player::Instance().SetAngle(angle);
    Player::Instance().Update(elapsedTime);

    stage->Update(elapsedTime);
    Player::Instance().Update(elapsedTime);
    EnemyManager::Instance().Update(elapsedTime);
    ItemManager::Instance().Update(elapsedTime);
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
        ItemManager::Instance().Render(rc, modelRenderer);
        EffectManager::Instance().Render(rc.view, rc.projection);
    }

    // デバッグ描画
    {
        Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
    }

}

// GUI描画
void SceneGame::DrawGUI()
{
    Player::Instance().DrawDebugGUI();
}
