#pragma once

#include "Stage2.h"
//#include "Player.h"
#include "CameraController.h"
#include "Scene.h"

// ゲームシーン
class SceneGame2 :public Scene
{
public:
	SceneGame2() {};
	~SceneGame2() override {};

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
	Stage2* stage = nullptr;
	//Player* player = nullptr;
	CameraController* cameraController = nullptr;

};
