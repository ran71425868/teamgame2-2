#include "ProjectileStraight.h"

//コンストラクタ
ProjectileStraight::ProjectileStraight(ProjectileManager* manager):Projectile(manager)
{
	/*model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");*/
	model = new Model("Data/Model/Sword/Sword.mdl");

	//表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;
}

//デストラクタ
ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

//更新処理
void ProjectileStraight::Update(float elapsedTime)
{
	//寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0)
	{
		//自分を削除
		Destroy();
	}

	//移動
	float speed = this->speed * elapsedTime;

	//位置 = 位置 + 方向 * 速さ
	position.x += direction.x * speed;//x
	position.y += direction.y * speed;//y
	position.z += direction.z * speed;//z


	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();

}

//描画処理
void ProjectileStraight::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//発射
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;//発射方向(正規化された進行方向ベクトル)
	this->position = position;//発射位置
}
