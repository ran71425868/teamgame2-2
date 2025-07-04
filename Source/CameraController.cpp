#include "System/Input.h"
#include "CameraController.h"
#include "Camera.h"
#include "Player.h"

//�X�V����
void CameraController::Update(float elapsedTime) 
{
	switch (CameraState)
	{
	case 0:
		SetCursorVisibility(show_cursor);
		CameraState++;
		[[fallthrough]];
	case 1:
		// 'P' �L�[ �g�O�������i�G�b�W���o�j
		currentPKey = GetKeyState('P') & 0x8000;
		if (currentPKey && !prevPKey) {
			show_cursor = !show_cursor;
			SetCursorVisibility(show_cursor);
		}
		prevPKey = currentPKey;

		if (!show_cursor) {
			// ���݂̃}�E�X���W�擾
			POINT mousePos;
			GetCursorPos(&mousePos);

			// ���Έړ���
			float ax = static_cast<float>(mousePos.x - centerX);
			float ay = static_cast<float>(mousePos.y - centerY);

			// �p�x�X�V
			angle.x = Player::Instance().GetAngle().x;
			angle.y += ax * sensitivity;

			// X����]����
			if (angle.x < minAngleX) angle.x = minAngleX;
			if (angle.x > maxAngleX) angle.x = maxAngleX;

			// Y����]���b�v
			if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
			if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;

			// �}�E�X����ʒ����ɖ߂�
			SetCursorPos(centerX, centerY);

			//�J������]�l����]�s��ɕϊ�	
			Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		}
		//��]�s�񂩂�O�����x�N�g�������o��
		DirectX::XMVECTOR Front = Transform.r[2];
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);

		//�����_������x�N�g�������Ɉ�苗�����ꂽ�J�����x�_�����߂�
		DirectX::XMFLOAT3 eye;
		eye.x = target.x - front.x * range;
		eye.y = target.y - front.y * range;
		eye.z = target.z - front.z * range;

		//�J�����̎��_�ƒ����_��ݒ�
		Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

		break;
	}
}

//�J�[�\���̕\��/��\��
void CameraController::SetCursorVisibility(bool visible)
{
	// ShowCursor �̓����J�E���^�����Z�b�g���A�����I�ɕ\��/��\����؂�ւ���
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