#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"

enum class EItemType{
	None,

	Light,
	Mirror,
	Fan,
	Door,
};

//エネミー
class Item :public Character
{
public:
	
	Item() {}
	~Item()override {}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//破棄
	void Destroy();

	//描画処理
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

public:
	EItemType itemType = EItemType::None;
	int uniqueId = -1;
};