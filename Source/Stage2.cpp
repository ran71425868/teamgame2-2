#include "Stage2.h"

//コンストラクタ
Stage2::Stage2()
{
	//ステージモデルを読み込み

	model = new Model("Data/Model/Stage/Stage2.mdl");
}

Stage2::~Stage2()
{
	//ステージモデルを破棄
	delete model;
}

//更新処理
void Stage2::Update(float elapsedTime)
{

}

//描画処理
void Stage2::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}