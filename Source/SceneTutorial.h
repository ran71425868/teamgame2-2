#pragma once

#include "Stage1.h"
#include "CameraController.h"
#include "Scene.h"

// �Q�[���V�[��
class SceneTutorial :public Scene
{
public:
	SceneTutorial() {};
	~SceneTutorial() override {};

	// ������
	void Initialize()override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render()override;

	// GUI�`��
	void DrawGUI()override;

private:
	Stage1* stage = nullptr;
	CameraController* cameraController = nullptr;

};
