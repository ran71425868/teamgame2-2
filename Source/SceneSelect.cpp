#include "System/Graphics.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneTutorial.h"
#include "SceneManager.h"
#include "SceneLoading.h"

//������
void SceneSelect::Initialize()
{
	sprite = new Sprite("Data/Sprite/pngimg.com - square_PNG94.png");

}

//�I����
void SceneSelect::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//�X�V����
void SceneSelect::Update(float elapsedTime)
{
	Mouse& mouseCursor = Input::Instance().GetMouse();

	const MouseButton anyButton =
		Mouse::BTN_LEFT;

	DirectX::XMFLOAT2 stageleft = { 320,360 };
	DirectX::XMFLOAT2 stageright = { 420,460 };

	DirectX::XMFLOAT2 tutorialleft = { 960,360 };
	DirectX::XMFLOAT2 tutorialright = { 1060,460 };

	if (stageleft.x < mouseCursor.GetPositionX() && stageright.x > mouseCursor.GetPositionX()&&stageleft.y<mouseCursor.GetPositionY()&&stageright.y>mouseCursor.GetPositionY())
	{
		if (mouseCursor.GetButtonDown() & anyButton)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}

	if (tutorialleft.x < mouseCursor.GetPositionX() && tutorialright.x > mouseCursor.GetPositionX() && tutorialleft.y<mouseCursor.GetPositionY() && tutorialright.y>mouseCursor.GetPositionY())
	{
		if (mouseCursor.GetButtonDown() & anyButton)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTutorial));
		}
	}
	
}

//�`�揈��
void SceneSelect::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	// �`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	// 2D�X�v���C�g�`��
	{
		//�^�C�g�����
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		sprite->Render(rc,
			320, 360, 0, 100, 100,
			0,
			1, 1, 1, 1);

		sprite->Render(rc,
			960, 360, 0, 100, 100,
			0,
			1, 1, 1, 1);
	}

}

//GUI�`��
void SceneSelect::DrawGUI()
{
}
