#include "Stage1.h"

//コンストラクタ
Stage1::Stage1()
{
	//ステージモデルを読み込み
	
	model1 = new Model("Data/Model/Stage/Tutorial.mdl");
}

Stage1::~Stage1()
{
	//ステージモデルを破棄
	delete model1;
}

//更新処理
void Stage1::Update(float elapsedTime)
{

}

//描画処理
void Stage1::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, transform, model1, ShaderId::Lambert);
}