#pragma once

#include <vector>
#include "Enemy.h"
#include <set>

//�G�l�~�[�}�l�[�W���[
class EnemyManager 
{
private:
	EnemyManager(){}
	~EnemyManager(){}
public:
	//�B��̃C���X�^���X�擾
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//�G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	//�G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	//�G�l�~�[�폜
	void Remove(Enemy* enemy);

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//�G�l�~�[�o�^
	void Register(Enemy* enemy);

	//�G�l�~�[�S�폜
	void Clear();

	//�f�o�b�O�v���~�e�B�u�`��
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

private:
	//�G�l�~�[���m�̏Փˏ���
	void CollisionEnemyVsEnemies();

	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};