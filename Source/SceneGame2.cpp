#include "System/Graphics.h"
#include "SceneGame2.h"
#include "SceneResult.h"
#include "Camera.h"
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
#include "Table.h"
#include "Fan.h"
#include "Door.h"
#include "Panel.h"
#include "Tana.h"
#include "System/Sprite.h"
#include <WICTextureLoader.h> // DirectXTKの画像読み込み
#include <wrl/client.h>
using namespace DirectX;

// ローカル変数（スプライトとポーズ状態）
namespace {
    bool isPaused = false;
    Sprite* pauseTitleSprite = nullptr;
    Sprite* backButtonSprite = nullptr;
    Sprite* quitButtonSprite = nullptr;
    Sprite* pauseBackSprite = nullptr;
}
// 初期化
void SceneGame2::Initialize()
{

    // スプライト初期化（画像読み込み）
    pauseTitleSprite = new Sprite("Data/Sprite/pause gray.png");
    backButtonSprite = new Sprite("Data/Sprite/pause2.png");
    quitButtonSprite = new Sprite("Data/Sprite/pause5.png");
    pauseBackSprite = new Sprite("Data/Sprite/pause back.png");

    Prop* prop5 = new Prop();
    prop5->SetPosition({ -10,2,20});
    PropManager::Instance().Register(prop5);
    Prop* prop = new Prop();
    prop->SetPosition({ -10, 0.5, -1 });
    prop->SetScale({ 0.25, 0.25, 0.25 });
    PropManager::Instance().Register(prop);

    // ステージ初期化
    stage = new Stage2();

    // プレイヤー初期化
    Player::Instance().Initializa();
    Player::Instance().SetPosition({ -10,4,20 });
    //Player::Instance().SetPosition({ 25,4,-8 });
    

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


    Light* light = new Light();
    light->SetPosition(DirectX::XMFLOAT3(25, 4.5f, 5));
    itemManager.Register(light);
    Light* light1 = new Light();
    light1->SetPosition(DirectX::XMFLOAT3(-10, 0.5, -7));
    itemManager.Register(light1);
    Light* light2 = new Light();
    light2->SetPosition(DirectX::XMFLOAT3(2, 0.3, -1));
    itemManager.Register(light2);

    Tana* tana = new Tana();
    tana->SetPosition(DirectX::XMFLOAT3(5, 0, -1));
    tana->SetAngle({ 0,-1.48,0 });
    itemManager.Register(tana);
    Tana* tana1 = new Tana();
    tana1->SetPosition(DirectX::XMFLOAT3(0, 0, -1));
    tana1->SetAngle({ 0,-1.48,0 });
    itemManager.Register(tana1);
    Tana* tana2 = new Tana();
    tana2->SetPosition(DirectX::XMFLOAT3(-5, 0, -1));
    tana2->SetAngle({ 0,-1.48,0 });
    itemManager.Register(tana2);
    Tana* tana3 = new Tana();
    tana3->SetPosition(DirectX::XMFLOAT3(-10, 0, -1));
    tana3->SetAngle({ 0,-1.48,0 });
    itemManager.Register(tana3);

    Fan* fan = new Fan();
    fan->SetPosition({ -1,0,-5 });
    fan->SetAngle({ 0,-80,0 });
    itemManager.Register(fan);

    Door* door = new Door();
    door->itemType = EItemType::Door;
    door->uniqueId = 1;
    door->SetPosition({ 9,0,-4 });
    itemManager.Register(door);

    

    Panel* panel1 = new Panel();
    //panel->SetPosition({29,4,-10 });
    panel1->itemType = EItemType::Panel;
    panel1->uniqueId = 1;
    panel1->SetPosition({ 29,4,-8 });
    panel1->SetAngle({ 0,-1.48,0 });
    itemManager.Register(panel1);

    Panel* panel2 = new Panel();
    panel2->SetPosition({ -8,1,20 });
    panel2->SetAngle({ 0,1.0f,0 });
    itemManager.Register(panel2);

    Table* table = new Table();
    table->SetPosition({ 5,1,20 });
    itemManager.Register(table);


    Mirror* mirror1 = new Mirror();
    mirror1->SetPosition({ -3, 0, -10 });
    mirror1->SetAngle({ 0,-2,0 });
    itemManager.Register(mirror1);
}

// 終了化
void SceneGame2::Finalize()
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

    // スプライト削除
    delete pauseTitleSprite;
    delete backButtonSprite;
    delete quitButtonSprite;
    delete pauseBackSprite;
    pauseTitleSprite = nullptr;
    backButtonSprite = nullptr;
    quitButtonSprite = nullptr;
    pauseBackSprite = nullptr;

    PropManager::Instance().Clear();
    ItemManager::Instance().Clear();
}

// 更新処理
void SceneGame2::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Input& input = Input::Instance();

    // Startボタンでポーズ切り替え
    if (gamePad.GetButtonDown() & GamePad::BTN_UP) {
        isPaused = !isPaused;

        // ポーズ時に強制的にカーソルを表示
        CameraController* controller = cameraController;
        if (controller) {
            controller->SetCursorVisibility(isPaused);
        }
    }

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
    Player::Instance().Update(elapsedTime);

    stage->Update(elapsedTime);
    Player::Instance().Update(elapsedTime);
    ItemManager::Instance().Update(elapsedTime);
    EffectManager::Instance().Update(elapsedTime);
    PropManager::Instance().Update(elapsedTime);

    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
    }
}

// 描画処理
void SceneGame2::Render()
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
void SceneGame2::DrawGUI()
{
    Player::Instance().DrawDebugGUI();
}
