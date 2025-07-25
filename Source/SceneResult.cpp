#include "System/Graphics.h"
#include "SceneTitle.h"
#include "SceneResult.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include <System/Audio.h>

//初期化
void SceneResult::Initialize()
{
	sprite = new Sprite("Data/Sprite/result.png");
	BGM = Audio::Instance().LoadAudioSource("Data/Sound/2-48987504.wav");
}

//終了化
void SceneResult::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	BGM->Stop();
}

//更新処理
void SceneResult::Update(float elapsedTime)
{
	BGM->Play(false);
	BGM->SetVolume(0.1f);
	//なにかボタンを押したらゲームシーンへ切り替え
	Mouse& mouseCursor = Input::Instance().GetMouse();

	const MouseButton anyButton =
		Mouse::BTN_LEFT;

	if (mouseCursor.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	}
}

//描画処理
void SceneResult::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	// 2Dスプライト描画
	{
		//タイトル画面
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		sprite->Render(rc,
			0, 0, 0, screenWidth, screenHeight,
			0,
			1, 1, 1, 1);
	}

}

//GUI描画
void SceneResult::DrawGUI()
{
}
