#pragma once

#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

//�O���錾
class ProjectileManager;

//�e��
class Projectile
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile(){}

	//�X�V����
	virtual void Update(float elapsedTime) = 0;

	//�`�揈��
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	//�f�o�b�O�v���~�e�B�u�`��
	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition()const { return position; }

	//�����擾
	const DirectX::XMFLOAT3& GetDirection()const { return direction; }

	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	//���a�擾
	float GetRadius() const { return radius; }

	//�j��
	void Destroy();

protected:
	//�s��X�V����
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };//�ʒu
	DirectX::XMFLOAT3 direction = { 0,0,1 }; //����(�i�s����)
	DirectX::XMFLOAT3 scale = { 1,1,1 };//�傫��
	//�p���s��
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	ProjectileManager* manager = nullptr;
	float radius = 0.5f;

};