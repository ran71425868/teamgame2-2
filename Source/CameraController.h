#pragma once

#include <DirectXMath.h>

//�J�����R���g���[���[
class CameraController {
public:
	//�X�V����
	void Update(float elapedTime);

	//�^�[�Q�b�g�ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3 target) { this->target = target; };

	//�J�[�\���̕\��/��\��
	void SetCursorVisibility(bool visible);

	//�I������
	void Finalize();
private:
	DirectX::XMFLOAT3 target = { 0,0,0 };//�����_
	DirectX::XMFLOAT3 angle = { 0,0,0 };//��]�p�x
	DirectX::XMMATRIX Transform;
	float rollSpeed = DirectX::XMConvertToRadians(90);//��]���x
	float range = 0.1f;//����
	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-60);
	int CameraState = 0;//�X�e�[�g
	float screenWidth = 1280;
	float screenHeight = 720;

	// ��ʒ������W
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;

	//���x
	float sensitivity = 0.005f;

	POINT center = {};

	//�}�E�X�|�C���^�̕\��
	bool show_cursor = false;

	//�g�O�������p
	bool prevPKey = false;
	bool currentPKey;
};