#include "System/Graphics.h"
#include "SceneGame.h"
//#include "SceneResult.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Player.h"
#include "EffectManager.h"

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	stage = new Stage();
	//�v���C���[������
	//player = new Player();
	Player::Instance().Initializa();

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0,10,-10),//���_
		DirectX::XMFLOAT3(0,0,0),//�����_
		DirectX::XMFLOAT3(0,1,0)//�����
	);

	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),//����p
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),//��ʃA�X�y�N�g��
		0.1f,//�N���b�v����(��)
		1000.0f//�N���b�v����(��)
		);

	cameraController = new CameraController;

	//�G�l�~�[������
	// �G�l�~�[�}�l�[�W���[���C���X�^���X����Ăяo��
	EnemyManager& enemyManager = EnemyManager::Instance();
	
	for (int i = 0; i < 2; ++i) 
	{
		// �G���쐬����
		EnemySlime* slime = new EnemySlime();

		// �G�̈ʒu��ݒ�
		slime->SetPosition(DirectX::XMFLOAT3(i*2.0f, 0, 5));

		slime->SetTerritory(slime->GetPosition(), 10.0f);

		// �쐬�����G���G�l�~�[�}�l�[�W���[�ɓo�^
		enemyManager.Register(slime);
	}

}

// �I����
void SceneGame::Finalize()
{
	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();

	if (cameraController != nullptr) {
		delete cameraController;
		cameraController = nullptr;
	}

	//�v���C���[�I����
	/*if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}*/
	Player::Instance().Finalize();

	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();

	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);
	//�v���C���[�X�V����
	Player::Instance().Update(elapsedTime);
	//�G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);
	//�G�t�F�N�g�}�l�[�W���[�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// �`�揀��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ���C�g�����i�������j
	rc.renderState = graphics.GetRenderState();

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();


	// 3D���f���`��
	{
		//�X�e�[�W�`��
		stage->Render(rc, modelRenderer);
		//�v���C���[�`��
		Player::Instance().Render(rc, modelRenderer);
		//�G�l�~�[�`��
		EnemyManager::Instance().Render(rc, modelRenderer);
		//�G�t�F�N�g�}�l�[�W���[�`��
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);
		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2D�X�v���C�g�`��
	{

	}
}

// GUI�`��
void SceneGame::DrawGUI()
{
	//�v���C���[�f�o�b�O�`��
	Player::Instance().DrawDebugGUI();
}
