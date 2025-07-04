#include "PLayer.h"
#include "System/Input.h"
#include <imgui.h>
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "System/Audio.h"
#include "EnemySlime.h"

//初期化
void Player::Initializa()
{
	model = new Model("Data/Model/Light/Light.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.05f;

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

	delete hitEffect;
}

//更新処理
void Player::Update(float elapsedTime)
{
	//移動入力処理
	InputMove(elapsedTime);

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

	//弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform();
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

	// atan2で角度を求める（Y軸回転）
	angle.y = atan2f(front.x, front.z);
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
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		//ジャンプ回数制限(現在のジャンプ回数がジャンプの最大数より小さければ)
		//現在のジャンプ回数を増加させ
		if (jumpCount < jumpLimit) 
		{
			jumpCount++;
			//ジャンプ
			Jump(jumpSpeed);

		}

	}

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
	rayOrigin.y += 0.3f;
	
	// レイの方向はカメラの前方向を使う
	XMFLOAT3 rayDirection = Camera::Instance().GetFront();

	// 正規化（念のため）
	XMVECTOR dirVec = XMLoadFloat3(&rayDirection);
	dirVec = XMVector3Normalize(dirVec);
	XMStoreFloat3(&rayDirection, dirVec);

	// （以下省略）レイキャストの処理はそのまま
	DirectX::XMFLOAT3 closestHitPoint = { 0, 0, 0 };
	float closestHitDistance = FLT_MAX;
	bool anyHit = false;

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		EnemySlime* slime = dynamic_cast<EnemySlime*>(enemy);
		if (slime)
		{
			XMFLOAT3 slimePos = slime->GetPosition();
			float slimeRadius = slime->GetRadius();
			float slimeHeight = slime->GetHeight();

			XMFLOAT3 currentHitPoint;
			float currentHitDistance;

			if (Collision::IntersectRayVsCylinder(
				rayOrigin, rayDirection,
				slimePos, slimeRadius, slimeHeight,
				currentHitPoint, currentHitDistance))
			{
				if (currentHitDistance < closestHitDistance)
				{
					closestHitDistance = currentHitDistance;
					closestHitPoint = currentHitPoint;
					anyHit = true;
				}
			}
		}
	}

	hasRayHit = anyHit;
	rayHitPoint = closestHitPoint;
}


//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

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
		rayOrigin.y += 0.3f;

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

		// レイ可視化（青）
		renderer->RenderLine(rc, rayOrigin, rayEnd, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	// レイが当たった場所にデバッグ円を描画 (追加)
	if (hasRayHit)
	{
		// 衝突点に赤い円を描画
		renderer->RenderSphere(rc, rayHitPoint, 0.2f, {1.0f, 0.0f, 0.0f, 1.0f}); // 赤い円、半径0.2f
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
		}
	}
	ImGui::End();
}