#include "System/Graphics.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
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
			640, 360, 0, 100, 100,
			0,
			1, 1, 1, 1);
	}

}

//GUI�`��
void SceneSelect::DrawGUI()
{
}
