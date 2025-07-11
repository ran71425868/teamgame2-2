#include "PropManager.h"
#include "Collision.h"


//エネミー削除
void PropManager::Remove(Prop* prop)
{
	//破棄リストに追加
	removes.insert(prop);

}
void PropManager::RemoveC(Clone* clone)
{
	//破棄リストに追加
	removes1.insert(clone);

}
void PropManager::RemoveM(Mirror* mirror)
{
	//破棄リストに追加
	removes2.insert(mirror);

}
//更新処理
void PropManager::Update(float elapsedTime)
{
	for (Prop* prop : props)
	{
		prop->Update(elapsedTime);
	}
	for (Clone* clone : clones)
	{
		clone->Update(elapsedTime);
	}
	for (Mirror* mirror : mirrors)
	{
		mirror->Update(elapsedTime);
	}

	//破棄処理
	//*enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	//更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Prop* prop : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Prop*>::iterator it = std::find(
			props.begin(), //ここから
			props.end(), //ここまでの中から
			prop//こいつがあるかどうかチェック
		);

		if (it != props.end())
		{
			props.erase(it);
		}

		//弾丸の破棄処理
		delete prop;
	}
	for (Clone* clone : removes1)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Clone*>::iterator it = std::find(
			clones.begin(), //ここから
			clones.end(), //ここまでの中から
			clone//こいつがあるかどうかチェック
		);

		if (it != clones.end())
		{
			clones.erase(it);
		}

		//弾丸の破棄処理
		delete clone;
	}
	for (Mirror* mirror : removes2)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Mirror*>::iterator it = std::find(
			mirrors.begin(), //ここから
			mirrors.end(), //ここまでの中から
			mirror//こいつがあるかどうかチェック
		);

		if (it != mirrors.end())
		{
			mirrors.erase(it);
		}

		//弾丸の破棄処理
		delete mirror;
	}
	//破棄リストをクリア
	removes.clear();
	removes1.clear();
	removes2.clear();

	
}

//描画処理
void PropManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Prop* prop : props)
	{
		prop->Render(rc, renderer);
	}
	for (Clone* clone : clones)
	{
		clone->Render(rc, renderer);
	}
	for (Mirror* mirror : mirrors)
	{
		mirror->Render(rc, renderer);
	}
}

//エネミー登録
void PropManager::Register(Prop* prop)
{
	props.emplace_back(prop);
}
void PropManager::RegisterC(Clone*clone)
{
	clones.emplace_back(clone);
}
void PropManager::RegisterM(Mirror* mirror)
{
	mirrors.emplace_back(mirror);
}


//弾丸全削除
void PropManager::Clear()
{
	for (Prop* prop : props)
	{
		delete prop;
	}
	props.clear();
	for (Clone* clone : clones)
	{
		delete clone;
	}
	clones.clear();
	for (Mirror* mirror : mirrors)
	{
		delete mirror;
	}
	mirrors.clear();
}


