#include "Player.h"
#include "System/Input.h"
#include <imgui.h>
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "System/Audio.h"
#include "EnemySlime.h"
#include <PropManager.h>
#include "ItemManager.h"
#include "Light.h"
#include "Mirror.h"



//初期化
void Player::Initializa()
{
	model = new Model("Data/Model/Light/Light_head.mdl");
	model1 = new Model("Data/Model/Light/Light_body.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.015f;

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSEの読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");
}

//終了化
void Player::Finalize() 
{
	delete hitSE;

	delete model;

	delete model1;

	delete hitEffect;
}

//更新処理
void Player::Update(float elapsedTime)
{
	//移動入力処理
	//InputMove(elapsedTime);

	//カメラの向きとプレイヤーの角度を同期
	SyncPlayerAngleWithCamera();

	//ジャンプ入力処理
	InputJump();

	//弾丸入力処理
	InputProjectile();

	//速力処理更新
	UpdateVelocity(elapsedTime);

	//弾丸更新処理
	projectileManager.Update(elapsedTime);

	//プレイヤーと敵との衝突判定
	CollisionPlayerVsEnemies();

	// レイキャスト処理 (追加)
	PerformRaycastToSlime();

	// レイキャスト処理 (clone)
	PerformRaycastToLight();

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();

	//オブジェクト行列を更新
	ModelUpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
	model1->UpdateTransform();
}

//プレイヤー用の行列計算
void Player::ModelUpdateTransform()
{
	//オブジェクト行列を更新
	auto prev_scale = scale;
	auto prev_position = position;
	auto prev_angle = angle;
	scale = { 1, 1, 1 };
	angle = { prev_angle.x, 0, prev_angle.z };
	position = {0, 18, -3};
	UpdateTransform();
	local_transform = transform;

	angle = { 0, prev_angle.y , 0 };
	scale = prev_scale;
	position = prev_position;
	UpdateTransform();
	parent_transform = transform;
	angle = prev_angle;
	scale = prev_scale;
	position = prev_position;

	auto LocalTransform = DirectX::XMLoadFloat4x4(&local_transform);
	auto ParentTransform = DirectX::XMLoadFloat4x4(&parent_transform);
	DirectX::XMStoreFloat4x4(&local_transform, LocalTransform * ParentTransform);

	{
		using namespace DirectX;

		XMMATRIX parent = XMLoadFloat4x4(&parent_transform);
		XMMATRIX offset = XMMatrixTranslation(0.0f, 0.0f, -11.0f);
		parent = offset * parent;
		XMStoreFloat4x4(&parent_transform, parent);
	}
}

//移動入力処理
void Player::InputMove(float elapsedTime) 
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//移動処理
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const 
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX* cameraRightX+ cameraRightZ* cameraRightZ);

	if (cameraRightLength > 0.0f) 
	{
		//単位ベクトル
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// カメラの向きとプレイヤーの角度を同期
void Player::SyncPlayerAngleWithCamera()
{
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& front = camera.GetFront();
	const DirectX::XMFLOAT3& up = camera.GetUp();
	const DirectX::XMFLOAT3& right = camera.GetRight();

	// atan2で角度を求める（Y軸回転）
	angle.y = atan2f(front.x, front.z);

}

// ベクトルの反射処理
DirectX::XMFLOAT3 Player::Reflect(const DirectX::XMFLOAT3& incident, const DirectX::XMFLOAT3& normal)
{
	using namespace DirectX;
	XMVECTOR i = XMLoadFloat3(&incident);
	XMVECTOR n = XMLoadFloat3(&normal);
	XMVECTOR r = XMVectorSubtract(i, XMVectorScale(n, 2.0f * XMVectorGetX(XMVector3Dot(i, n))));
	XMFLOAT3 reflected;
	XMStoreFloat3(&reflected, r);
	return reflected;
}


//プレイヤーとエネミーの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++) 
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(position, radius, height, enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), outPosition))
		{
			
			//敵の真上付近に当たったかを判定
			//プレイヤーの位置をXMVECTORに変換
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);

			//敵の位置をXMVECTORに変換
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());

			//敵からプレイヤーへのベクトルを計算
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);

			//そのベクトルを正規化（単位ベクトル化）
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);

			//正規化したベクトルXMFLOAT3に変換
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			//上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				//小ジャンプ
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				//押し出し後の位置設定（上から踏んづけなかった場合の処理）
				enemy->SetPosition(outPosition);

			}

		}
	}
}

//弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}

					//ヒットエフェクト
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					//ヒットSE再生
					{
						hitSE->Play(false);
					}

					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}

}

//着地した時に呼ばれる
void Player::OnLanding()
{
	//現在のジャンプ回数をリセット
	jumpCount = 0;
}


//ジャンプ入力処理
void Player::InputJump() 
{
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//if (gamePad.GetButtonDown() & GamePad::BTN_A)
	//{
	//	//ジャンプ回数制限(現在のジャンプ回数がジャンプの最大数より小さければ)
	//	//現在のジャンプ回数を増加させ
	//	if (jumpCount < jumpLimit) 
	//	{
	//		jumpCount++;
	//		//ジャンプ
	//		Jump(jumpSpeed);

	//	}

	//}

}

//弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);//前方向(sinとangleで計算)
		dir.y = 0.0f;
		dir.z = cosf(angle.y);//前方向(cosとangleで計算)

		//発射位置(プレイヤーの腰あたり,yがheightの半分)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);
	}

	//追尾処理
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		//発射位置(プレイヤーの腰あたり,yがheightの半分)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//ターゲット(デフォルトではプレイヤーの前方)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; i++)
		{
			//敵との距離判定
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);//プレイヤーの位置
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());//敵の位置
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);//敵への方向ベクトル-プレイヤーの位置
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);//敵への方向ベクトルの長さ

			float d;//今回の距離(球から敵までの距離)
			DirectX::XMStoreFloat(&d, D);
			
			//今回の敵の方がプレイヤーに近いからどうかチェック
			if (d < dist)
			{
				//近かったらtargetに設定
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight()*0.5f;
			}
		}

		//発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);

	}
}

// レイキャスト処理 (追加)
void Player::PerformRaycastToSlime()
{
	using namespace DirectX;

	// レイの始点をプレイヤー位置より少し上にする（例：1.5fだけ上に）
	XMFLOAT3 rayOrigin = GetPosition();
	rayOrigin.y += 0.2f;
	
	// レイの方向はカメラの前方向を使う
	XMFLOAT3 rayDirection = Camera::Instance().GetFront();

	// 正規化（念のため）
	XMVECTOR dirVec = XMLoadFloat3(&rayDirection);
	dirVec = XMVector3Normalize(dirVec);
	XMStoreFloat3(&rayDirection, dirVec);

	// 1バウンド目
	hit1 = false;
	XMFLOAT3 normal1;
	int hitEnemyIndex = -1;

	//EnemyとRayCast
	if (RaycastToSlimes(rayOrigin, rayDirection, rayHitPoint, normal1, hitEnemyIndex))
	{
		hit1 = true;
	}
	else
	{
		hasRayHit = false;

		// ヒットしなかった場合はカメラの前方向に適当な長さだけ進んだ位置を代入
		float rayLength = 1000.0f;
		rayHitPoint = {
			rayOrigin.x + rayDirection.x * rayLength,
			rayOrigin.y + rayDirection.y * rayLength,
			rayOrigin.z + rayDirection.z * rayLength
		};
	}

	if (hit1)
	{
		Mouse& mouseCursor = Input::Instance().GetMouse();

		//playerの位置を保存
		XMFLOAT3 playerPos = Player::Instance().GetPosition();

		EnemyManager& enemyManager = EnemyManager::Instance();

		Enemy* enemy = enemyManager.GetEnemy(hitEnemyIndex);
		EnemySlime* enemySlime = dynamic_cast<EnemySlime*>(enemy);
		XMFLOAT3 slimePos = enemySlime->GetPosition();

		if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
		{
			Player::Instance().SetPosition(slimePos);
			enemySlime->SetPosition(playerPos);

		}

	}

	// 2バウンド目（反射）
	hit2 = false;
	hitPoint2;

	if (hit1)
	{
		reflectedDir = Reflect(rayDirection, normal1);
		XMFLOAT3 newOrigin = {
			rayHitPoint.x + reflectedDir.x * 0.01f,
			rayHitPoint.y + reflectedDir.y * 0.01f,
			rayHitPoint.z + reflectedDir.z * 0.01f,
		};

		XMFLOAT3 dummyNormal;
		if (RaycastToSlimes(newOrigin, reflectedDir, hitPoint2, dummyNormal, hitEnemyIndex))
		{
			hit2 = true;
		}

	}

	// 保存
	hasRayHit = hit1;
	rayHitPoint = hit1 ? rayHitPoint : rayHitPoint;

	hasReflectHit = hit2;
	reflectedHitPoint = hit2 ? hitPoint2 : hitPoint2;
}

 //レイキャスト処理 (追加)
void Player::PerformRaycastToLight()
{
	using namespace DirectX;

	// レイの始点をプレイヤー位置より少し上にする（例：1.5fだけ上に）
	XMFLOAT3 rayOrigin = GetPosition();
	rayOrigin.y += 0.2f;

	// レイの方向はカメラの前方向を使う
	XMFLOAT3 rayDirection = Camera::Instance().GetFront();

	// 正規化（念のため）
	XMVECTOR dirVec = XMLoadFloat3(&rayDirection);
	dirVec = XMVector3Normalize(dirVec);
	XMStoreFloat3(&rayDirection, dirVec);

	// 1バウンド目
	hit1 = false;
	hit3 = false;
	XMFLOAT3 normal1;
	int hitCloneIndex = -1;

	//PropとRaycast
	if (RaycastToLights(rayOrigin, rayDirection, rayHitPoint, normal1, hitCloneIndex))
	{
		hit1 = true;
		hasRayHit = hit1;
		rayHitPoint = hit1 ? rayHitPoint : rayHitPoint;
	}
	else if (RaycastToMirrors(rayOrigin, rayDirection, rayHitPoint, normal1, hitCloneIndex))
	{
		hit3 = true;
		hasRayHit = hit3;
		rayHitPoint = hit3 ? rayHitPoint : rayHitPoint;
	}
	else
	{
		hasRayHit = false;

		// ヒットしなかった場合はカメラの前方向に適当な長さだけ進んだ位置を代入
		float rayLength = 1000.0f;
		rayHitPoint = {
			rayOrigin.x + rayDirection.x * rayLength,
			rayOrigin.y + rayDirection.y * rayLength,
			rayOrigin.z + rayDirection.z * rayLength
		};
	}

	if (hit1)
	{
		Mouse& mouseCursor = Input::Instance().GetMouse();

		//playerの位置を保存
		XMFLOAT3 playerPos = Player::Instance().GetPosition();

		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitCloneIndex);
		Light* light = dynamic_cast<Light*>(item);
		XMFLOAT3 lightPos = light->GetPosition();
		

		if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
		{
			Player::Instance().SetPosition(lightPos);
			light->SetPosition(playerPos);

		}

	}

	if (hit3)
	{
		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitCloneIndex);
		Mirror* mirror = dynamic_cast<Mirror*>(item);
	}
	
	// 2バウンド目（反射）
	hit2 = false;
	hitPoint2;

	if (hit1)
	{
		reflectedDir = Reflect(rayDirection, normal1);
		XMFLOAT3 newOrigin = {
			rayHitPoint.x + reflectedDir.x * 0.01f,
			rayHitPoint.y + reflectedDir.y * 0.01f,
			rayHitPoint.z + reflectedDir.z * 0.01f,
		};

		XMFLOAT3 dummyNormal;
		if (RaycastToLights(newOrigin, reflectedDir, hitPoint2, dummyNormal, hitCloneIndex))
		{
			hit2 = true;
		}
	}
	else if (hit3)
	{
		reflectedDir = Reflect(rayDirection, normal1);
		XMFLOAT3 newOrigin = {
			rayHitPoint.x + reflectedDir.x * 0.01f,
			rayHitPoint.y + reflectedDir.y * 0.01f,
			rayHitPoint.z + reflectedDir.z * 0.01f,
		};

		XMFLOAT3 dummyNormal;
		if (RaycastToLights(newOrigin, reflectedDir, hitPoint2, dummyNormal, hitCloneIndex))
		{
			hit2 = true;
		}
		if (hit2)
		{
			Mouse& mouseCursor = Input::Instance().GetMouse();

			//playerの位置を保存
			XMFLOAT3 playerPos = Player::Instance().GetPosition();

			ItemManager& itemManager = ItemManager::Instance();

			Item* item = itemManager.GetItem(hitCloneIndex);
			Light* light = dynamic_cast<Light*>(item);
			XMFLOAT3 lightPos = light->GetPosition();


			if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
			{
				Player::Instance().SetPosition(lightPos);
				light->SetPosition(playerPos);

			}
		}
	}


	hasReflectHit = hit2;
	reflectedHitPoint = hit2 ? hitPoint2 : hitPoint2;
}

// スライムに対するレイキャストを共通化
bool Player::RaycastToSlimes(
	const DirectX::XMFLOAT3& rayOrigin,
	const DirectX::XMFLOAT3& rayDir,
	DirectX::XMFLOAT3& outHitPoint,
	DirectX::XMFLOAT3& outHitNormal,
	int& enemyHitIndex)
{
	using namespace DirectX;

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	float closestDistance = FLT_MAX;
	bool anyHit = false;

	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		EnemySlime* slime = dynamic_cast<EnemySlime*>(enemy);
		if (!slime) continue;

		XMFLOAT3 slimePos = slime->GetPosition();
		float radius = slime->GetRadius();
		float height = slime->GetHeight();

		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsCylinder(
			rayOrigin, rayDir,
			slimePos, radius, height,
			hitPoint, hitDistance))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeCylinderNormal(hitPoint, slimePos);
				anyHit = true;
				enemyHitIndex = i;
			}
		}
	}

	return anyHit;
}

//lightに対するレイキャストを共通化
bool Player::RaycastToLights(
	const DirectX::XMFLOAT3& rayOrigin,
	const DirectX::XMFLOAT3& rayDir,
	DirectX::XMFLOAT3& outHitPoint,
	DirectX::XMFLOAT3& outHitNormal,
	int& lightHitIndex)
{
	using namespace DirectX;

	ItemManager& itemManager = ItemManager::Instance();
	int itemCount = itemManager.GetItemCount();

	float closestDistance = FLT_MAX;
	bool anyHit = false;

	for (int i = 0; i < itemCount; i++)
	{
		Item* item = itemManager.GetItem(i);
		Light* light = dynamic_cast<Light*>(item);
		if (!light) continue;
		
		XMFLOAT3 clonePos = light->GetPosition();
		float radius = light->GetRadius();
		float height = light->GetHeight();
	
		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsCylinder(
			rayOrigin, rayDir,
			clonePos, radius, height,
			hitPoint, hitDistance))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeCylinderNormal(hitPoint, clonePos);
				anyHit = true;
				lightHitIndex = i;
			}
		}

	}

	return anyHit;
}
bool Player::RaycastToMirrors(
	const DirectX::XMFLOAT3& rayOrigin,
	const DirectX::XMFLOAT3& rayDir,
	DirectX::XMFLOAT3& outHitPoint,
	DirectX::XMFLOAT3& outHitNormal,
	int& lightHitIndex)
{
	using namespace DirectX;

	ItemManager& itemManager = ItemManager::Instance();
	int itemCount = itemManager.GetItemCount();

	float closestDistance = FLT_MAX;
	bool anyHit = false;

	for (int i = 0; i < itemCount; i++)
	{
		Item* item = itemManager.GetItem(i);

		Mirror* mirror = dynamic_cast<Mirror*>(item);
		if (!mirror) continue;

		XMFLOAT3 mirrorPos = mirror->GetPosition();
		float mirror_radius = mirror->GetRadius();
		float mirror_height = mirror->GetHeight();

		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsCylinder(
			rayOrigin, rayDir,
			mirrorPos, mirror_radius, mirror_height,
			hitPoint, hitDistance))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeCylinderNormal(hitPoint, mirrorPos);
				anyHit = true;
				lightHitIndex = i;
			}
		}
	}

	return anyHit;
}
// Cylinderからの法線をだす
DirectX::XMFLOAT3 Player::ComputeCylinderNormal(
	const DirectX::XMFLOAT3& hitPoint,
	const DirectX::XMFLOAT3& cylinderCenter)
{
	using namespace DirectX;

	XMFLOAT3 normalXZ = {
		hitPoint.x - cylinderCenter.x,
		0.0f,
		hitPoint.z - cylinderCenter.z
	};

	XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&normalXZ));
	XMFLOAT3 out;
	XMStoreFloat3(&out, n);
	return out;
}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, local_transform, model, ShaderId::Lambert);
	renderer->Render(rc, parent_transform, model1, ShaderId::Lambert);

	//弾丸描画処理
	projectileManager.Render(rc, renderer);
}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	//弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);

	// レイキャストの視覚化
	{
		using namespace DirectX;

		XMFLOAT3 rayOrigin = GetPosition();
		rayOrigin.y += 0.2f;

		XMFLOAT3 rayDirection = Camera::Instance().GetFront();
		XMVECTOR dirVec = XMLoadFloat3(&rayDirection);
		dirVec = XMVector3Normalize(dirVec);
		XMStoreFloat3(&rayDirection, dirVec);

		float rayLength = 1000.0f;
		XMFLOAT3 rayEnd = {
			rayOrigin.x + rayDirection.x * rayLength,
			rayOrigin.y + rayDirection.y * rayLength,
			rayOrigin.z + rayDirection.z * rayLength
		};

		// プレイヤーからの1本目（白）
		renderer->RenderLine(rc, rayOrigin, rayHitPoint, { 1.0f, 1.0f, 1.0f, 1.0f });

		if (hit1)
		{
			// 反射レイ(2本目)（青など）
			if (hasReflectHit)
			{
				renderer->RenderLine(rc, rayHitPoint, reflectedHitPoint, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
			else
			{
				XMFLOAT3 reflectEnd = {
					rayHitPoint.x + reflectedDir.x * 20.0f,
					rayHitPoint.y + reflectedDir.y * 20.0f,
					rayHitPoint.z + reflectedDir.z * 20.0f
				};
				renderer->RenderLine(rc, rayHitPoint, reflectEnd, { 1.0f, 1.0f, 1.0f, 1.0f });

			}
		}
		if (hit3)
		{
			// 反射レイ(2本目)（青など）
			if (hasReflectHit)
			{
				renderer->RenderLine(rc, rayHitPoint, reflectedHitPoint, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
			else
			{
				XMFLOAT3 reflectEnd = {
					rayHitPoint.x + reflectedDir.x * 20.0f,
					rayHitPoint.y + reflectedDir.y * 20.0f,
					rayHitPoint.z + reflectedDir.z * 20.0f
				};
				renderer->RenderLine(rc, rayHitPoint, reflectEnd, { 1.0f, 1.0f, 1.0f, 1.0f });

			}
		}
	}

	// レイが当たった場所にデバッグ円を描画 (追加)
	if (hasRayHit)
	{
		// 衝突点に赤い円を描画
		renderer->RenderSphere(rc, rayHitPoint, 0.2f, {1.0f, 0.0f, 0.0f, 1.0f}); // 赤い円、半径0.2f
	}
	if (hasReflectHit)
	{
		// 衝突点に緑い円を描画
		renderer->RenderSphere(rc, reflectedHitPoint, 0.2f, { 1.0f, 1.0f, 0.0f, 1.0f });
	}
}

//デバッグ用GUI描画
void Player::DrawDebugGUI() 
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Tranaform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Cursor", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("[P] to move cursor in and out");
		}
		//レイキャスト結果 (追加)
		if (ImGui::CollapsingHeader("Raycast Result", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Has Hit", &hasRayHit);
			if (hasRayHit)
			{
				ImGui::InputFloat3("Hit Point", &rayHitPoint.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			}
			ImGui::Checkbox("Has Reflect Hit", &hasReflectHit);
			if (hasReflectHit)
			{
				ImGui::InputFloat3("Hit Point", &reflectedHitPoint.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			}
		}
	}
	ImGui::End();
}