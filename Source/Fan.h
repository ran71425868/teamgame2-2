#pragma once
#include "System/ModelRenderer.h"

class Fan
{
public:
	Fan();
	~Fan();

	void Update(float elapsedTime);
	void Render(const RenderContext& rc, ModelRenderer* renderer);
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	void UpdateTransform();
private:
	Model* model = nullptr;
	Model* model1 = nullptr;
protected:
	DirectX::XMFLOAT3  position = { 0,0,0 };
	DirectX::XMFLOAT3  angle = { 0,0,0 };
	DirectX::XMFLOAT3  scale = { 1,1,1 };
	DirectX::XMFLOAT4X4  transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};
