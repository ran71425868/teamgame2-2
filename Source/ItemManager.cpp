#include "ItemManager.h"
#include "Collision.h"


//エネミー削除
void ItemManager::Remove(Item* item)
{
	//破棄リストに追加
	removes.insert(item);

}
//更新処理
void ItemManager::Update(float elapsedTime)
{

	//破棄処理
	//*enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	//更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Item* item : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Item*>::iterator it = std::find(
			items.begin(), //ここから
			items.end(), //ここまでの中から
			item//こいつがあるかどうかチェック
		);

		if (it != items.end())
		{
			items.erase(it);
		}

		//弾丸の破棄処理
		delete item;
	}

	//破棄リストをクリア
	removes.clear();

	for (Item* item : items)
	{
		item->Update(elapsedTime);
	}
}

//描画処理
void ItemManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Item* item : items)
	{
		item->Render(rc, renderer);
	}
}

//エネミー登録
void ItemManager::Register(Item* item)
{
	items.emplace_back(item);
}

//デバッグプリミティブ描画
void ItemManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Item* item : items)
	{
		item->RenderDebugPrimitive(rc, renderer);
	}
}