#include "PLayer.h"
#include "System/Input.h"
#include <imgui.h>
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "System/Audio.h"

//������
void Player::Initializa()
{
	model = new Model("Data/Model/kagu/Tana2.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 10.0f;

	//�q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//�q�b�gSE�̓ǂݍ���
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
}

//�I����
void Player::Finalize() 
{
	delete hitSE;

	delete model;

	delete hitEffect;
}

//�X�V����
void Player::Update(float elapsedTime)
{
	//�ړ����͏���
	InputMove(elapsedTime);

	//�J�����̌����ƃv���C���[�̊p�x�𓯊�
	SyncPlayerAngleWithCamera();

	//�W�����v���͏���
	InputJump();

	//�e�ۓ��͏���
	InputProjectile();

	//���͏����X�V
	UpdateVelocity(elapsedTime);

	//�e�ۍX�V����
	projectileManager.Update(elapsedTime);

	//�v���C���[�ƓG�Ƃ̏Փ˔���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փˏ���
	CollisionProjectilesVsEnemies();

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	model->UpdateTransform();
}

//�ړ����͏���
void Player::InputMove(float elapsedTime) 
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//�ړ�����
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const 
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX* cameraRightX+ cameraRightZ* cameraRightZ);

	if (cameraRightLength > 0.0f) 
	{
		//�P�ʃx�N�g��
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		//�P�ʃx�N�g��
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	//�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

// �J�����̌����ƃv���C���[�̊p�x�𓯊�
void Player::SyncPlayerAngleWithCamera()
{
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& front = camera.GetFront();

	// atan2�Ŋp�x�����߂�iY����]�j
	angle.y = atan2f(front.x, front.z);
}

//�v���C���[�ƃG�l�~�[�̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++) 
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(position, radius, height, enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), outPosition))
		{
			
			//�G�̐^��t�߂ɓ����������𔻒�
			//�v���C���[�̈ʒu��XMVECTOR�ɕϊ�
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);

			//�G�̈ʒu��XMVECTOR�ɕϊ�
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());

			//�G����v���C���[�ւ̃x�N�g�����v�Z
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);

			//���̃x�N�g���𐳋K���i�P�ʃx�N�g�����j
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);

			//���K�������x�N�g��XMFLOAT3�ɕϊ�
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			//�ォ�瓥��Â����ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				//���W�����v
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				//�����o����̈ʒu�ݒ�i�ォ�瓥��Â��Ȃ������ꍇ�̏����j
				enemy->SetPosition(outPosition);

			}

		}
	}
}

//�e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//�_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//������΂�
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}

					//�q�b�g�G�t�F�N�g
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//�q�b�gSE�Đ�
					{
						hitSE->Play(false);
					}

					//�e�۔j��
					projectile->Destroy();
				}
			}
		}
	}

}

//���n�������ɌĂ΂��
void Player::OnLanding()
{
	//���݂̃W�����v�񐔂����Z�b�g
	jumpCount = 0;
}


//�W�����v���͏���
void Player::InputJump() 
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//�W�����v�񐔐���(���݂̃W�����v�񐔂��W�����v�̍ő吔��菬�������)
		//���݂̃W�����v�񐔂𑝉�����
		if (jumpCount < jumpLimit) 
		{
			jumpCount++;
			//�W�����v
			Jump(jumpSpeed);

		}

	}

}

//�e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);//�O����(sin��angle�Ōv�Z)
		dir.y = 0.0f;
		dir.z = cosf(angle.y);//�O����(cos��angle�Ōv�Z)

		//���ˈʒu(�v���C���[�̍�������,y��height�̔���)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//����
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);
	}

	//�ǔ�����
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//���ˈʒu(�v���C���[�̍�������,y��height�̔���)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//�^�[�Q�b�g(�f�t�H���g�ł̓v���C���[�̑O��)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; i++)
		{
			//�G�Ƃ̋�������
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);//�v���C���[�̈ʒu
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());//�G�̈ʒu
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);//�G�ւ̕����x�N�g��-�v���C���[�̈ʒu
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);//�G�ւ̕����x�N�g���̒���

			float d;//����̋���(������G�܂ł̋���)
			DirectX::XMStoreFloat(&d, D);
			
			//����̓G�̕����v���C���[�ɋ߂�����ǂ����`�F�b�N
			if (d < dist)
			{
				//�߂�������target�ɐݒ�
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight()*0.5f;
			}
		}

		//����
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);

	}
}

//�`�揈��
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	//�e�ە`�揈��
	projectileManager.Render(rc, renderer);
}

//�f�o�b�O�v���~�e�B�u�`��
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//���N���X�̊֐��Ăяo��
	Character::RenderDebugPrimitive(rc, renderer);

	//�e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.RenderDebugPrimitive(rc, renderer);

}

//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI() 
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Tranaform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			ImGui::InputFloat3("Position", &position.x);
			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//�X�P�[��
			ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Cursor", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("[P] to move cursor in and out");
		}
	}
	ImGui::End();
}