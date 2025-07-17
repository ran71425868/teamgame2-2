#include "Fan.h"


//コンストラクタ
Fan::Fan()
{
	model = new Model("Data/Model/gimmick/Fan_b.mdl");
	model1 = new Model("Data/Model/gimmick/Fan_h.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.05f;

	//幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

}

//デストラクタ
Fan::~Fan()
{
	delete model;
	delete model1;
}

//更新処理
void Fan::Update(float elapsedTime)
{

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//描画処理
void Fan::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

}

//デバッグプリミティブ描画
void Fan::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Item::RenderDebugPrimitive(rc, renderer);
}