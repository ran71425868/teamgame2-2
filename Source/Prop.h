#pragma once

#include "System/Model.h" // Model�N���X�̒�`������w�b�_�[�t�@�C��
#include <DirectXMath.h>  // DirectXMath���g�p���邽��

// �O���錾 (RenderContext �����̃w�b�_�[�Œ�`����Ă���ꍇ)
struct RenderContext;
class ModelRenderer;
//
//// �V�[���ɔz�u����ėp�I�ȃ��f���I�u�W�F�N�g
//class Prop
//{
//public:
//    // �R���X�g���N�^
//    // modelPath: ���[�h����MDL�t�@�C���̃p�X
//    // position: �I�u�W�F�N�g�̏����ʒu
//    // scale: �I�u�W�F�N�g�̃X�P�[�� (�f�t�H���g��1.0f)
//    Prop(const char* modelPath, const DirectX::XMFLOAT3& position, float scale = 1.0f);
//
//    // �f�X�g���N�^
//    ~Prop();
//
//    // �X�V���� (�K�v�ɉ����Ēǉ�)
//    void Update(float elapsedTime);
//
//    // �`�揈��
//    void Render(const RenderContext& rc, ModelRenderer* renderer);
//
//    // �ʒu�ݒ�
//    void SetPosition(const DirectX::XMFLOAT3& position) { m_position = position; }
//
//    // �X�P�[���ݒ�
//    void SetScale(float scale) { m_scale = { scale, scale, scale }; }
//    void SetScale(const DirectX::XMFLOAT3& scale) { m_scale = scale; }
//
//    // �ʒu�擾
//    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }
//
//private:
//    Model* m_model;                   // ���[�h���郂�f��
//    DirectX::XMFLOAT3 m_position;     // �ʒu
//    DirectX::XMFLOAT3 m_scale;        // �X�P�[��
//    DirectX::XMFLOAT3 m_rotation;     // ��] (�K�v�ɉ����Ďg�p)
//
//    // �R�s�[�R���X�g���N�^�Ƒ�����Z�q�̋֎~ (�V���O���g���ł͂Ȃ����߁A�K�v�ɉ����Ď���)
//    Prop(const Prop&) = delete;
//    Prop& operator=(const Prop&) = delete;
//};
