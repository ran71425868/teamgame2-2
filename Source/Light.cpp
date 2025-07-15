#include "Light.h"


//コンストラクタ
Light::Light()
{
	model = new Model("Data/Model/Light/Light.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.05f;

	//幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

}

//デストラクタ
Light::~Light()
{
	delete model;
}

//更新処理
void Light::Update(float elapsedTime)
{
	
	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
}

//描画処理
void Light::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

}

//デバッグプリミティブ描画
void Light::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスのデバッグプリミティブ描画
	Item::RenderDebugPrimitive(rc, renderer);
}