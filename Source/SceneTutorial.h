#pragma once

#include "Stage1.h"
#include "CameraController.h"
#include "Scene.h"

// ゲームシーン
class SceneTutorial :public Scene
{
public:
	SceneTutorial() {};
	~SceneTutorial() override {};

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

	// GUI描画
	void DrawGUI()override;

private:
	Stage1* stage = nullptr;
	CameraController* cameraController = nullptr;

};
