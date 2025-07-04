//#include "Prop.h"
//#include "System/Model.h"   // Model�N���X�̒�`������w�b�_�[�t�@�C��
//#include "System/Graphics.h" // RenderContext��ModelRenderer���֘A����\���̂���w�b�_�[
//
//// �R���X�g���N�^
//Prop::Prop(const char* modelPath, const DirectX::XMFLOAT3& position, float scale)
//    : m_position(position),
//    m_scale({ scale, scale, scale }), // �����X�P�[����ݒ�
//    m_rotation({ 0.0f, 0.0f, 0.0f })   // ������]��ݒ� (�K�v�ɉ����ĕύX)
//{
//    m_model = new Model(modelPath); // MDL�t�@�C�������[�h
//}
//
//// �f�X�g���N�^
//Prop::~Prop()
//{
//    if (m_model)
//    {
//        delete m_model;
//        m_model = nullptr;
//    }
//}
//
//// �X�V���� (�K�v�ɉ����Ēǉ��A����͋�Ŗ�肠��܂���)
//void Prop::Update(float elapsedTime)
//{
//    // �K�v�ɉ����āA�ʒu���]�̍X�V���W�b�N�������ɋL�q
//}
//
//// �`�揈��
//void Prop::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//    if (!m_model || !renderer)
//    {
//        return; // ���f���܂��̓����_���[�������Ȃ牽�����Ȃ�
//    }
//
//    // ���[���h�s��̍쐬
//    // �X�P�[�� -> ��] -> �ړ� �̏��ɓK�p����̂���ʓI
//    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
//    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
//    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//
//    DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
//
//    // RenderContext �Ƀ��[���h�s���ݒ�
//    RenderContext currentRc = rc; // RenderContext �̓R�s�[���Ďg�p
//    DirectX::XMStoreFloat4x4(&currentRc.world, worldMatrix); // XMFLOAT4x4�ɕϊ����ĕۑ�
//
//    // ���f����`��
//    m_model->Render(currentRc, renderer);
//}