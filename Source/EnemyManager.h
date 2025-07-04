#pragma once

#include <vector>
#include "Enemy.h"
#include <set>

//エネミーマネージャー
class EnemyManager 
{
private:
	EnemyManager(){}
	~EnemyManager(){}
public:
	//唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	//エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	//エネミー削除
	void Remove(Enemy* enemy);

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//エネミー登録
	void Register(Enemy* enemy);

	//エネミー全削除
	void Clear();

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

private:
	//エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();

	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};