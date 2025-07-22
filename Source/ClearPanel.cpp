#include "ClearPanel.h"


//コンストラクタ
ClearPanel::ClearPanel()
{
	model = new Model("Data/Model/gimmick/Kagami1.mdl");

	scale.x = scale.y = scale.z = 0.5f;
	position.x = 10.0f;
	position.y = 0.0f;
	position.z = 10.0f;
}

//デストラクタ
ClearPanel::~ClearPanel()
{
	delete model;
}

//更新処理
void ClearPanel::Update(float elapsedTime)
{

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//描画処理
void ClearPanel::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

}

//デバッグプリミティブ描画
void ClearPanel::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Item::RenderDebugPrimitive(rc, renderer);
}