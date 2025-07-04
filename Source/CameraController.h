#pragma once

#include <DirectXMath.h>

//カメラコントローラー
class CameraController {
public:
	//更新処理
	void Update(float elapedTime);

	//ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3 target) { this->target = target; };

	//カーソルの表示/非表示
	void SetCursorVisibility(bool visible);

	//終了処理
	void Finalize();
private:
	DirectX::XMFLOAT3 target = { 0,0,0 };//注視点
	DirectX::XMFLOAT3 angle = { 0,0,0 };//回転角度
	DirectX::XMMATRIX Transform;
	float rollSpeed = DirectX::XMConvertToRadians(90);//回転速度
	float range = 0.1f;//距離
	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-60);
	int CameraState = 0;//ステート
	float screenWidth = 1280;
	float screenHeight = 720;

	// 画面中央座標
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;

	//感度
	float sensitivity = 0.005f;

	POINT center = {};

	//マウスポインタの表示
	bool show_cursor = false;

	//トグル処理用
	bool prevPKey = false;
	bool currentPKey;
};