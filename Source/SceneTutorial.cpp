#include "System/Graphics.h"
#include "SceneTutorial.h"
#include "SceneResult.h"
#include "Camera.h"
#include "Character.h"
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
#include "Fan.h"
#include "Door.h"
#include "Panel.h"
using namespace DirectX;

// ローカル変数（スプライトとポーズ状態）
namespace {
    bool isPaused = false;
    Sprite* pauseTitleSprite = nullptr;
    Sprite* backButtonSprite = nullptr;
    Sprite* quitButtonSprite = nullptr;
    Sprite* pauseBackSprite = nullptr;
    Sprite* setumeiSprite = nullptr;
}

// 初期化
void SceneTutorial::Initialize()
{

    // スプライト初期化（画像読み込み）
    pauseTitleSprite = new Sprite("Data/Sprite/pause_gray.png");
    backButtonSprite = new Sprite("Data/Sprite/pause2.png");
    quitButtonSprite = new Sprite("Data/Sprite/pause5.png");
    pauseBackSprite = new Sprite("Data/Sprite/pause back.png");
    setumeiSprite = new Sprite("Data/Sprite/setumei.png");

    // ステージ初期化
    stage = new Stage1();

    

    Prop* prop1 = new Prop();
    prop1->SetPosition({ -9,1,7 });
    PropManager::Instance().Register(prop1);

    Prop* prop2 = new Prop();
    prop2->SetPosition({ 0,1,5 });
    PropManager::Instance().Register(prop2);

    Prop* prop3 = new Prop();
    prop3->SetPosition({ 6,1,2 });
    PropManager::Instance().Register(prop3);

    Prop* prop4 = new Prop();
    prop4->SetPosition({ 0,1,-9 });
    PropManager::Instance().Register(prop4);

    Prop* prop5 = new Prop();
    prop5->SetPosition({ -15,2,-3 });
    PropManager::Instance().Register(prop5);

    // プレイヤー初期化
    Player::Instance().Initializa();
    Player::Instance().SetPosition({-15,3.9,-3});

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
    light->SetPosition(DirectX::XMFLOAT3(6, 2.9, 2));
    itemManager.Register(light);
    
    
    Mirror* mirror = new Mirror();
    mirror->SetPosition({0,2.9,5});
    mirror->SetAngle({ 0,0,0 });
    itemManager.Register(mirror);
    
    Fan* fan = new Fan();
    fan->SetPosition({ 0,2.9,-9 });
    fan->SetAngle({ 0,-80,0 });
    itemManager.Register(fan);


    //Door* door = new Door(EItemType::Door, 0);
    Door* door = new Door();
    door->itemType = EItemType::Door;
    door->uniqueId = 0;
    door->SetPosition({ 9,0,-4.5 });
    itemManager.Register(door);

    Panel* panel = new Panel();
    panel->itemType = EItemType::Panel;
    panel->uniqueId = 0;
    panel->SetPosition({ -9,3,7 });
    panel->SetAngle({ 0,-2.2f,0 });
    itemManager.Register(panel);
    

}

// 終了化
void SceneTutorial::Finalize()
{
    
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

    // スプライト削除
    delete pauseTitleSprite;
    delete backButtonSprite;
    delete quitButtonSprite;
    delete pauseBackSprite;
    delete setumeiSprite;
    pauseTitleSprite = nullptr;
    backButtonSprite = nullptr;
    quitButtonSprite = nullptr;
    pauseBackSprite = nullptr;
    setumeiSprite = nullptr;
}

// 更新処理
void SceneTutorial::Update(float elapsedTime)
{
    Input& input = Input::Instance();

    // Pキーでポーズ切り替え
    currentPKey = GetKeyState('P') & 0x8000;
    if (currentPKey && !prevPKey) {
        isPaused = !isPaused;

        // ポーズ時に強制的にカーソルを表示
        CameraController* controller = cameraController;
        if (controller) {
            controller->SetCursorVisibility(isPaused);
        }
    }
    prevPKey = currentPKey;
    // ポーズ中のマウス入力処理
    if (isPaused) {
        if (input.GetMouseButtonDown(0)) {
            POINT mousePos = input.GetMousePosition();

            RECT back = { 500, 300, 800, 340 };
            RECT quit = { 500, 360, 800, 400 };

            if (PtInRect(&back, mousePos)) {
                isPaused = false;

                // ポーズ解除時にカーソルを非表示
                CameraController* controller = cameraController;
                if (controller) {
                    controller->SetCursorVisibility(false);
                }
            }
            else if (PtInRect(&quit, mousePos)) {
                isPaused = false;

                // カーソル再表示（必要なら）
                CameraController* controller = cameraController;
                if (controller) {
                    controller->SetCursorVisibility(true);
                }

                // タイトルへシーン遷移
                SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
            }
        }
        return; // ポーズ中は以降の処理スキップ
    }

    // カメラターゲット追従
    DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
    target.y += 0.5f;
    cameraController->SetTarget(target);
    cameraController->Update(elapsedTime);

    //カメラの角度をプレイヤーの角度と同期させる
    DirectX::XMFLOAT3 angle = cameraController->GetAngle();
    Player::Instance().SetAngle(angle);

    stage->Update(elapsedTime);

    Player::Instance().Update(elapsedTime);
    ItemManager::Instance().Update(elapsedTime);
    PropManager::Instance().Update(elapsedTime);
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
        Player::Instance().RayRender(rc, shapeRenderer);
        ItemManager::Instance().Render(rc, modelRenderer);
        EffectManager::Instance().Render(rc.view, rc.projection);
    }

    // デバッグ描画
   /* {
        Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
        ItemManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
    }*/

    // UI描画
    {
        setumeiSprite->Render(rc, 0, 0, 0, 400, 400, 0, 0, 400, 400, 0, 1, 1, 1, 1);
    }

    // ポーズ中UI描画
    if (isPaused) {
        pauseBackSprite->Render(rc, 0, 0, 0, 1280, 720, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
        pauseTitleSprite->Render(rc, 470, 100, 0, 1280, 720, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
        backButtonSprite->Render(rc, 500, 300, 0, 1000, 500, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
        quitButtonSprite->Render(rc, 500, 360, 0, 1000, 500, 0, 0, 1280, 720, 0, 1, 1, 1, 1);
    }
}

// GUI描画
void SceneTutorial::DrawGUI()
{
    /*Player::Instance().DrawDebugGUI();*/
}
