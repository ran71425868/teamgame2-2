#pragma once

#include "System/ModelRenderer.h"

//ステージ
class Stage1
{
public:
	Stage1();
	~Stage1();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:
	Model* model1 = nullptr;
};