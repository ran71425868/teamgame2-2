#pragma once

#include <vector>
#include "Prop.h"
#include "Clone.h"
#include "Mirror.h"
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
	int GetCloneCount() const { return static_cast<int>(clones.size()); }
	int GetMirrorCount() const { return static_cast<int>(mirrors.size()); }

	//エネミー取得
	Prop* GetProP(int index) { return props.at(index); }
	Clone* GetClone(int index) { return clones.at(index); }
	Mirror* GetMirror(int index) { return mirrors.at(index); }

	//エネミー削除
	void Remove(Prop* prop);
	void RemoveC(Clone* clone);
	void RemoveM(Mirror* mirror);

	//更新処理
	void Update(float elapsedTime);
	
	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//エネミー登録
	void Register(Prop* prop);
	void RegisterC(Clone* clone);
	void RegisterM(Mirror* mirror);
	

	//エネミー全削除
	void Clear();


private:

	std::vector<Prop*> props;
	std::set<Prop*> removes;
	std::vector<Clone*> clones;
	std::set<Clone*> removes1;
	std::vector<Mirror*> mirrors;
	std::set<Mirror*> removes2;

};