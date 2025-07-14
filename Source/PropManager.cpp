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
void PropManager::RemoveF(Fan* fan)
{
	//破棄リストに追加
	removes3.insert(fan);

}
void PropManager::RemoveP(Panel* panel)
{
	//破棄リストに追加
	removes4.insert(panel);

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
	for (Fan* fan : fans)
	{
		fan->Update(elapsedTime);
	}
	for (Panel* panel : panels)
	{
		panel->Update(elapsedTime);
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
	for (Fan* fan : removes3)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Fan*>::iterator it = std::find(
			fans.begin(), //ここから
			fans.end(), //ここまでの中から
			fan//こいつがあるかどうかチェック
		);

		if (it != fans.end())
		{
			fans.erase(it);
		}

		//弾丸の破棄処理
		delete fan;
	}
	for (Panel* panel : removes4)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Panel*>::iterator it = std::find(
			panels.begin(), //ここから
			panels.end(), //ここまでの中から
			panel//こいつがあるかどうかチェック
		);

		if (it != panels.end())
		{
			panels.erase(it);
		}

		//弾丸の破棄処理
		delete panel;
	}
	//破棄リストをクリア
	removes.clear();
	removes1.clear();
	removes2.clear();
	removes3.clear();
	removes4.clear();

	
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
	for (Fan* fan : fans)
	{
		fan->Render(rc, renderer);
	}
	for (Panel* panel : panels)
	{
		panel->Render(rc, renderer);
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
void PropManager::RegisterF(Fan* fan)
{
	fans.emplace_back(fan);
}
void PropManager::RegisterP(Panel* panel)
{
	panels.emplace_back(panel);
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
	for (Fan* fan : fans)
	{
		delete fan;
	}
	fans.clear();
	for (Panel* panel : panels)
	{
		delete panel;
	}
	panels.clear();
}


