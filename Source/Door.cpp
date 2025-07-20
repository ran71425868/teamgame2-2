#include "Door.h"


//コンストラクタ
Door::Door()
{
	model = new Model("Data/Model/Door/door.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 1.0f;

	//幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

}

//デストラクタ
Door::~Door()
{
	delete model;
}

//更新処理
void Door::Update(float elapsedTime)
{

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//描画処理
void Door::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

}

//デバッグプリミティブ描画
void Door::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Item::RenderDebugPrimitive(rc, renderer);
}