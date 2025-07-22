#include "Prop.h"

Prop::Prop()
{
	model=new Model("Data/Model/kagu/ki.mdl");
	
	scale.x = scale.y = scale.z = 0.15f;
	position.x = 10.0f;
	position.y = 0.0f;
	position.z = 10.0f;
}

Prop::~Prop()
{
	delete model;
	
}
void Prop::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	//DirectX::XMFLOAT4X4 transform;
	//DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, transform, model, ShaderId::Lambert);
	
}
void Prop::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}
void Prop::Update(float elapsedTime)
{
	UpdateTransform();

	model->UpdateTransform();
	
}