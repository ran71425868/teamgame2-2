#pragma once

#include <vector>
#include "Item.h"
#include <set>

//エネミーマネージャー
class ItemManager
{
private:
	ItemManager() {}
	~ItemManager() {}
public:
	//唯一のインスタンス取得
	static ItemManager& Instance()
	{
		static ItemManager instance;
		return instance;
	}

	//エネミー数取得
	int GetItemCount() const { return static_cast<int>(items.size()); }

	//エネミー取得
	Item* GetItem(int index) { return items.at(index); }

	//エネミー削除
	void Remove(Item* item);

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//エネミー登録
	void Register(Item* item);

	//エネミー全削除
	void Clear();

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

private:

	std::vector<Item*> items;
	std::set<Item*> removes;
};