#include "System/Input.h"
#include "CameraController.h"
#include "Camera.h"
#include "Player.h"

//更新処理
void CameraController::Update(float elapsedTime) 
{
	switch (CameraState)
	{
	case 0:
		SetCursorVisibility(show_cursor);
		CameraState++;
		[[fallthrough]];
	case 1:
		// 'P' キー トグル処理（エッジ検出）
		currentPKey = GetKeyState('P') & 0x8000;
		if (currentPKey && !prevPKey) {
			show_cursor = !show_cursor;
			SetCursorVisibility(show_cursor);
		}
		prevPKey = currentPKey;

		if (!show_cursor) {
			// 現在のマウス座標取得
			POINT mousePos;
			GetCursorPos(&mousePos);

			// 相対移動量
			float ax = static_cast<float>(mousePos.x - centerX);
			float ay = static_cast<float>(mousePos.y - centerY);

			// 角度更新
			angle.x = Player::Instance().GetAngle().x;
			angle.y += ax * sensitivity;

			// X軸回転制限
			if (angle.x < minAngleX) angle.x = minAngleX;
			if (angle.x > maxAngleX) angle.x = maxAngleX;

			// Y軸回転ラップ
			if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
			if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;

			// マウスを画面中央に戻す
			SetCursorPos(centerX, centerY);

			//カメラ回転値を回転行列に変換	
			Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		}
		//回転行列から前方向ベクトルを取り出す
		DirectX::XMVECTOR Front = Transform.r[2];
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);

		//注視点から後ろベクトル方向に一定距離離れたカメラ支点を求める
		DirectX::XMFLOAT3 eye;
		eye.x = target.x - front.x * range;
		eye.y = target.y - front.y * range;
		eye.z = target.z - front.z * range;

		//カメラの視点と注視点を設定
		Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

		break;
	}
}

//カーソルの表示/非表示
void CameraController::SetCursorVisibility(bool visible)
{
	// ShowCursor の内部カウンタをリセットし、強制的に表示/非表示を切り替える
	static int showCount = 0;

	while (showCount < 0) {
		ShowCursor(true);
		++showCount;
	}
	while (showCount > 0) {
		ShowCursor(false);
		--showCount;
	}

	if (visible) {
		ShowCursor(true);
		++showCount;
	}
	else {
		ShowCursor(false);
		--showCount;
	}
}

void CameraController::Finalize()
{
	show_cursor = true;
	SetCursorVisibility(show_cursor);
}