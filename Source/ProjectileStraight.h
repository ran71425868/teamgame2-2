#pragma once

#include "System/Model.h"
#include "Projectile.h"

//���i�e��
class ProjectileStraight :public Projectile
{
public:
	ProjectileStraight(ProjectileManager* manager);
	~ProjectileStraight() override;

	// �X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;
	
	//����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float speed = 10.0f;
	float lifeTimer = 3.0f;

};