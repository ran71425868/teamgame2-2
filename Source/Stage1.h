#pragma once

#include "System/ModelRenderer.h"

//�X�e�[�W
class Stage1
{
public:
	Stage1();
	~Stage1();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:
	Model* model1 = nullptr;
};