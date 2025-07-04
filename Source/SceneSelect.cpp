#include "System/Graphics.h"
#include "SceneSelect.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"

//������
void SceneSelect::Initialize()
{
	sprite = new Sprite("Data/Sprite/shikaku-illust3.png");
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



	if (mouseCursor.GetButtonDown()&anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
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
			300, 500, 0, 150, 100,
			0,
			1, 1, 1, 1);
	}

}

//GUI�`��
void SceneSelect::DrawGUI()
{
}
