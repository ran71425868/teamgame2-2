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
#include "PropManager.h"
#include "ItemManager.h"
#include "Light.h"
#include "Mirror.h"
using namespace DirectX;

// 初期化
void SceneTutorial::Initialize()
{
    // ステージ初期化
    stage = new Stage1();

    Prop*prop = new Prop();
    PropManager::Instance().Register(prop);

    Prop* prop1 = new Prop();
    prop1->SetPosition({ 5,5,5 });
    PropManager::Instance().Register(prop1);

   /* Clone* clone = new Clone();
    clone->SetPosition({ 5,0,5 });
    PropManager::Instance().RegisterC(clone);*/


    
    Fan* fan = new Fan();
    fan->SetPosition({ -10,0,-10 });
    PropManager::Instance().RegisterF(fan);

    // プレイヤー初期化
    Player::Instance().Initializa();
    Player::Instance().SetPosition({3,10,3});

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
    ItemManager& itemManager = ItemManager::Instance();
    for (int i = 0; i < 2; ++i)
    {
        Light* light = new Light();
        light->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
        itemManager.Register(light);
    }
    for (float i = 0; i < 2; ++i)
    {
        Mirror* mirror = new Mirror();
        mirror->SetPosition({i*5,0,5});
        itemManager.Register(mirror);
    }

    

}

// 終了化
void SceneTutorial::Finalize()
{
    ItemManager::Instance().Clear();
    
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

}

// 更新処理
void SceneTutorial::Update(float elapsedTime)
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

   
    PropManager::Instance().Update(elapsedTime);

    stage->Update(elapsedTime);

    Player::Instance().Update(elapsedTime);
    ItemManager::Instance().Update(elapsedTime);
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
        PropManager::Instance().Render(rc, modelRenderer);
        
        Player::Instance().Render(rc, modelRenderer);
        ItemManager::Instance().Render(rc, modelRenderer);
        EffectManager::Instance().Render(rc.view, rc.projection);
    }

    // デバッグ描画
    {
        Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
        ItemManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
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
