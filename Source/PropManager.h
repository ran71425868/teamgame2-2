#pragma once

#include <vector>
#include "Prop.h"
#include "Mirror.h"
#include "Fan.h"
#include "Panel.h"
#include <set>

//エネミーマネージャー
class PropManager
{
private:
	PropManager() {}
	~PropManager() {}
public:
	//唯一のインスタンス取得
	static PropManager& Instance()
	{
		static PropManager instance;
		return instance;
	}

	//エネミー数取得
	int GetPropCount() const { return static_cast<int>(props.size()); }

	//エネミー取得
	Prop* GetProP(int index) { return props.at(index); }

	//エネミー削除
	void Remove(Prop* prop);

	//更新処理
	void Update(float elapsedTime);
	
	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//エネミー登録
	void Register(Prop* prop);
	//エネミー全削除
	void Clear();


private:

	std::vector<Prop*> props;
	std::set<Prop*> removes;


};