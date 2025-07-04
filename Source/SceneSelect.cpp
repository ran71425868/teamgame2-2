#include "System/Graphics.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"

//初期化
void SceneSelect::Initialize()
{
	sprite = new Sprite("Data/Sprite/pngimg.com - square_PNG94.png");

}

//終了化
void SceneSelect::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//更新処理
void SceneSelect::Update(float elapsedTime)
{
	Mouse& mouseCursor = Input::Instance().GetMouse();

	const MouseButton anyButton =
		Mouse::BTN_LEFT;

	DirectX::XMFLOAT2 left = { 640,360 };
	DirectX::XMFLOAT2 right = { 740,460 };

	if (left.x < mouseCursor.GetPositionX() && right.x > mouseCursor.GetPositionX()&&left.y<mouseCursor.GetPositionY()&&right.y>mouseCursor.GetPositionY())
	{
		if (mouseCursor.GetButtonDown() & anyButton)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}
	
}

//描画処理
void SceneSelect::Render()
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
			640, 360, 0, 100, 100,
			0,
			1, 1, 1, 1);
	}

}

//GUI描画
void SceneSelect::DrawGUI()
{
}
