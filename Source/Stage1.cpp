#include "Stage1.h"

//�R���X�g���N�^
Stage1::Stage1()
{
	//�X�e�[�W���f����ǂݍ���
	
	model1 = new Model("Data/Model/Stage/Tutorial.mdl");
}

Stage1::~Stage1()
{
	//�X�e�[�W���f����j��
	delete model1;
}

//�X�V����
void Stage1::Update(float elapsedTime)
{

}

//�`�揈��
void Stage1::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	//�����_���Ƀ��f����`�悵�Ă��炤
	renderer->Render(rc, transform, model1, ShaderId::Lambert);
}