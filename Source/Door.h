#pragma once

#include "System/Model.h"
#include "Item.h"

//ライト
class Door :public Item
{
public:
	Door();
	~Door() override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

private:
	Model* model = nullptr;

};