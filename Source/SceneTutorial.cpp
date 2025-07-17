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

    

    Prop* prop1 = new Prop();
    prop1->SetPosition({ 0,1,12 });
    PropManager::Instance().Register(prop1);

    Prop* prop2 = new Prop();
    prop2->SetPosition({ 0,1,5 });
    PropManager::Instance().Register(prop2);

    Prop* prop3 = new Prop();
    prop3->SetPosition({ 0,1,-2 });
    PropManager::Instance().Register(prop3);

    Prop* prop4 = new Prop();
    prop4->SetPosition({ 0,1,-9 });
    PropManager::Instance().Register(prop4);


    // プレイヤー初期化
    Player::Instance().Initializa();
    Player::Instance().SetPosition({-15,5,-3});

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
    
        Light* light = new Light();
        light->SetPosition(DirectX::XMFLOAT3(0, 3, -2));
        itemManager.Register(light);
    
    
        Mirror* mirror = new Mirror();
        mirror->SetPosition({0,3,5});
        itemManager.Register(mirror);
    
    Fan* fan = new Fan();
    fan->SetPosition({ 0,3,-9 });
    fan->SetAngle({ 0,-80,0 });
    itemManager.Register(fan);
    

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
