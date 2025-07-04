#pragma once

#include "System/ModelRenderer.h"
#include "Character.h" 
#include "ProjectileManager.h"
#include "Effect.h"
#include "System/AudioSource.h"
#include <DirectXMath.h>

//�v���C��[
class Player : public Character
{
private:
	Player() {};
	~Player() override {};

public:
	//�C���X�^���X�擾
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	//������
	void Initializa();

	//�I����
	void Finalize();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//�f�o�b�O�v���~�e�B�u�`��
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	//�W�����v���͏���
	void InputJump();

private:
	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;

	//�ړ����͏���
	void InputMove(float elapsedTime);

	// �J�����̌����ƃv���C���[�̊p�x�𓯊�
	void SyncPlayerAngleWithCamera();

	//�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayerVsEnemies();

	// ���C�L���X�g���� (�ǉ�)
	void PerformRaycastToSlime();

	//�e�ۓ��͏���
	void InputProjectile();

	//�e�ۂƓG�̏Փ˔���
	void CollisionProjectilesVsEnemies();

protected:
	//���n�������ɌĂ΂��
	void OnLanding() override;


private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 12.0f;

	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;//�e�ۊǗ�

	Effect* hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

	// ���C�L���X�g���ʂ�ێ����郁���o�ϐ� (�ǉ�)
	bool hasRayHit = false;
	DirectX::XMFLOAT3 rayHitPoint = { 0, 0, 0 };
};