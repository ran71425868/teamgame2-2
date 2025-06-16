#include "EnemyManager.h"
#include "Collision.h"


//�G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
	//�j�����X�g�ɒǉ�
	removes.insert(enemy);

}
//�X�V����
void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//�j������
	//*enemies�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	//�X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Enemy* enemy : removes)
	{
		//std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Enemy*>::iterator it = std::find(
			enemies.begin(), //��������
			enemies.end(), //�����܂ł̒�����
			enemy//���������邩�ǂ����`�F�b�N
		);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		//�e�ۂ̔j������
		delete enemy;
	}
	//�j�����X�g���N���A
	removes.clear();

	//�G���m�̏Փˏ���
	CollisionEnemyVsEnemies();
}

//�`�揈��
void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

//�G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}


//�e�ۑS�폜
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

//�f�o�b�O�v���~�e�B�u�`��
void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->RenderDebugPrimitive(rc, renderer);
	}
}

//�G�l�~�[���m�̏Փˏ���
void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++)
	{
		for (int j = i + 1; j < enemyCount; j++)
		{
			Enemy* enemy1 = enemyManager.GetEnemy(i);
			Enemy* enemy2 = enemyManager.GetEnemy(j);

			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectSphereVsSphere(enemy1->GetPosition(),enemy1->GetRadius(), enemy2->GetPosition(), enemy2->GetRadius(), outPosition))
			//{
			//	//�����o����̈ʒu�ݒ�
			//	enemy2->SetPosition(outPosition);
			//	
			//}

			if (Collision::IntersectCylinderVsCylinder(
				enemy1->GetPosition(),
				enemy1->GetRadius(), 
				enemy1->GetHeight(), 
				enemy2->GetPosition(),
				enemy2->GetRadius(),
				enemy2->GetHeight(),
				outPosition))
			{
				//�����o����̈ʒu�ݒ�
				enemy2->SetPosition(outPosition);
			}
		}
	}

}
