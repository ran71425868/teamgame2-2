#include "Player.h"
#include "System/Input.h"
#include <imgui.h>
#include "Camera.h"
#include "Collision.h"
#include "System/Audio.h"
#include <PropManager.h>
#include "ItemManager.h"
#include "Light.h"
#include "Mirror.h"
#include "Door.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneResult.h"

//初期化
void Player::Initializa()
{
	model = new Model("Data/Model/Light/Light_head.mdl");
	model1 = new Model("Data/Model/Light/Light_body.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.025f;

	//ヒットエフェクト読み込みimakara
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//ヒットSEの読み込み
	BTNSE = Audio::Instance().LoadAudioSource("Data/Sound/4-a13ed491.wav");
	windSE = Audio::Instance().LoadAudioSource("Data/Sound/592c2e49.wav");
	DoorSE = Audio::Instance().LoadAudioSource("Data/Sound/2-a13ed491.wav");
	WPSE = Audio::Instance().LoadAudioSource("Data/Sound/3-a13ed491.wav");
	panelSE = Audio::Instance().LoadAudioSource("Data/Sound/4-a13ed491.wav");

	for (int i = 0; i < 5; ++i) {
		clearpanels[i] = new ClearPanel();
	}
}

//終了化
void Player::Finalize() 
{
	delete BTNSE;
	delete windSE;
	delete DoorSE;
	delete WPSE;
	delete panelSE;

	delete model;

	delete model1;

	delete hitEffect;

	for (int i = 0; i < 5; ++i) {
		delete clearpanels[i];
		clearpanels[i] = nullptr; // nullptrを設定しておくのが安全
	}

	panelcount = 0;
	panelcount1 = 0;
}

//更新処理
void Player::Update(float elapsedTime)
{
	//移動入力処理
	//InputMove(elapsedTime);

	//カメラの向きとプレイヤーの角度を同期
	SyncPlayerAngleWithCamera();

	//速力処理更新
	UpdateVelocity(elapsedTime);

	//弾丸更新処理
	projectileManager.Update(elapsedTime);

	// レイキャスト処理 (clone)
	PerformRaycastToLight();

	//弾丸と敵の衝突処理

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
	position = {0, 3, -3};
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


//弾丸と敵の衝突処理
//void Player::CollisionProjectilesVsEnemies()
//{
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	//全ての弾丸と全ての敵を総当たりで衝突処理
//	int projectileCount = projectileManager.GetProjectileCount();
//	int enemyCount = enemyManager.GetEnemyCount();
//	for (int i = 0; i < projectileCount; ++i)
//	{
//		Projectile* projectile = projectileManager.GetProjectile(i);
//
//		for (int j = 0; j < enemyCount; ++j)
//		{
//			Enemy* enemy = enemyManager.GetEnemy(j);
//
//			//衝突処理
//			DirectX::XMFLOAT3 outPosition;
//			if (Collision::IntersectSphereVsCylinder(
//				projectile->GetPosition(),
//				projectile->GetRadius(),
//				enemy->GetPosition(),
//				enemy->GetRadius(),
//				enemy->GetHeight(),
//				outPosition))
//			{
//				//ダメージを与える
//				if (enemy->ApplyDamage(1, 0.5f))
//				{
//					//吹き飛ばす
//					{
//						DirectX::XMFLOAT3 impulse;
//						const float power = 10.0f;
//						const DirectX::XMFLOAT3& e = enemy->GetPosition();
//						const DirectX::XMFLOAT3& p = projectile->GetPosition();
//						float vx = e.x - p.x;
//						float vz = e.z - p.z;
//						float lengthXZ = sqrtf(vx * vx + vz * vz);
//						vx /= lengthXZ;
//						vz /= lengthXZ;
//
//						impulse.x = vx * power;
//						impulse.y = power * 0.5f;
//						impulse.z = vz * power;
//
//						enemy->AddImpulse(impulse);
//					}
//
//					//ヒットエフェクト
//					{
//						DirectX::XMFLOAT3 e = enemy->GetPosition();
//						e.y += enemy->GetHeight() * 0.5f;
//						hitEffect->Play(e);
//					}
//
//					//ヒットSE再生
//					{
//						hitSE->Play(false);
//					}
//
//					//弾丸破棄
//					projectile->Destroy();
//				}
//			}
//		}
//	}
//
//}

 //レイキャスト処理 (追加)
void Player::PerformRaycastToLight()
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

	// 1バウンド目
	hit1 = false;
	hit3 = false;
	hitFan = false;
	hit_panel = false;
	XMFLOAT3 normal1;
	int hitCloneIndex = -1;
	int hitPanelIndex = -1;
	int hitMirrorIndex = -1;

	//ライトとRaycast
	if (RaycastToLights(rayOrigin, rayDirection, rayHitPoint, normal1, hitCloneIndex))
	{
		hit1 = true;
		hasRayHit = hit1;
		rayHitPoint = hit1 ? rayHitPoint : rayHitPoint;
	}
	//ミラーとRaycast
	else if (RaycastToMirrors(rayOrigin, rayDirection, rayHitPoint, normal1, hitMirrorIndex))
	{
		hit3 = true;
		hasRayHit = hit3;
		rayHitPoint = hit3 ? rayHitPoint : rayHitPoint;
	}
	//扇風機とRaycast
	else if (RaycastToFans(rayOrigin, rayDirection, rayHitPoint, normal1, hitCloneIndex))
	{
		hitFan = true;
		hasRayHit = hitFan;
		rayHitPoint = hitFan ? rayHitPoint : rayHitPoint;
	}
	//パネルとRaycast
	else if (RaycastToPanels(rayOrigin, rayDirection, rayHitPoint, normal1, hitPanelIndex))
	{
		hit_panel = true;
		hasRayHit = hit_panel;
		rayHitPoint = hit_panel ? rayHitPoint : rayHitPoint;
	}
	//ヒットしない
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

	//ライトに対しての処理
	if (hit1)
	{
		Mouse& mouseCursor = Input::Instance().GetMouse();

		//playerの位置を保存
		XMFLOAT3 playerPos = Player::Instance().GetPosition();

		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitCloneIndex);
		Light* light = dynamic_cast<Light*>(item);
		//lightの位置を保存
		XMFLOAT3 lightPos = light->GetPosition();
		
		if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
		{
			//入れ替え
			Player::Instance().SetPosition(lightPos);
			light->SetPosition(playerPos);
			WPSE->Play(false);

		}

	}
	//ミラーに対しての処理
	if (hit3)
	{
		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitMirrorIndex);
		Mirror* mirror = dynamic_cast<Mirror*>(item);
	}
	//扇風機に対しての処理
	if (hitFan)
	{
		Mouse& mouseCursor = Input::Instance().GetMouse();

		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitCloneIndex);
		Fan* fan = dynamic_cast<Fan*>(item);
		
		// ファンの前方方向からレイキャスト出す

		// ヒットしたアイテムをドアに変換する

		// 変換してドアだったら
			// 左クリックして回転させる


#if true
		if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
		{
			ItemManager& itemManager = ItemManager::Instance();

			Door* door = itemManager.GetDoor(0);
			if (door != nullptr)
				door->SetAngle({ 0,60,0 });

			door = itemManager.GetDoor(1);
			if (door != nullptr)
				door->SetAngle({ 0,60,0 });
		}
#endif
	}
	
	//パネルに対しての処理
	if (hit_panel)
	{
		Mouse& mouseCursor = Input::Instance().GetMouse();

		// 現在のボタン状態を取得
		bool currentMouseState = mouseCursor.GetButtonDown();

		// 前フレームでは押されておらず、今フレームで押されたなら true（1フレームだけ）
		bool mouseBtnDownFlag = !prevMouseBtnState && currentMouseState;

		// 状態を保存
		prevMouseBtnState = currentMouseState;

		ItemManager& itemManager = ItemManager::Instance();

		Item* item = itemManager.GetItem(hitPanelIndex);


		if (mouseBtnDownFlag & Mouse::BTN_LEFT && panelcount <= 0)
		{

			Panel* panel = itemManager.GetPanel(0);
			if (panel != nullptr)
			{
				clearpanels[0]->SetPosition({ -9,3,7 });
				clearpanels[0]->SetAngle({ 0,-2.2f,0 });
				itemManager.Register(clearpanels[0]);
			}

		}

		// stage1奥
			if (mouseBtnDownFlag && Mouse::BTN_LEFT && panelcount == 0)
			{
				Panel* panel2 = dynamic_cast<Panel*>(item);

				panelcount++;

				clearpanels[2]->SetPosition(panel2->GetPosition());
				clearpanels[2]->SetAngle(panel2->GetAngle());
				itemManager.Register(clearpanels[2]);
				itemManager.Remove(panel2);

			}
		//stage1手前
			else if (mouseBtnDownFlag && Mouse::BTN_LEFT && panelcount == 1)
			{
				Panel* panel2 = dynamic_cast<Panel*>(item);

				panelcount++;
				clearpanels[1]->SetPosition(panel2->GetPosition());
				clearpanels[1]->SetAngle(panel2->GetAngle());
				itemManager.Register(clearpanels[1]);
				itemManager.Remove(panel2);

				if (panelcount > 1)
				{
					PanelReset();
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
				}

			}

		//stage1奥
		//if (mouseBtnDownFlag && Mouse::BTN_LEFT&&panelcount==0)
		//{
		//	
		//	Panel* panel = itemManager.GetPanel(2);
		//	if (panel != nullptr)
		//	{
		//		panelcount++;
		//		clearpanel2->SetPosition({ 1,4,-2 });
		//		clearpanel2->SetAngle({ 0,-90,0 });
		//		itemManager.Register(clearpanel2);
		//		itemManager.Remove(panel);
		//		
		//	}
		//}
		////stage1手前
		//else if (mouseBtnDownFlag && Mouse::BTN_LEFT && panelcount == 1)
		//{
		//	Panel* panel1 = itemManager.GetPanel(1);
		//	if (panel1 != nullptr)
		//	{
		//		panelcount++;
		//		clearpanel1->SetPosition({ 29,4,-8 });
		//		clearpanel1->SetAngle({ 0,-1.48,0 });
		//		itemManager.Register(clearpanel1);
		//		itemManager.Remove(panel1);
		//		
		//		if (panelcount > 1)
		//		{
		//			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
		//		}
		//		
		//	}
		//		
		//}

		//stage2奥
		if (mouseBtnDownFlag && Mouse::BTN_LEFT && panelcount1 == 0)
		{
			Panel* panel2 = dynamic_cast<Panel*>(item);
			panelcount1++;
			clearpanels[3]->SetPosition(panel2->GetPosition());
			clearpanels[3]->SetAngle(panel2->GetAngle());
			itemManager.Register(clearpanels[3]);
			itemManager.Remove(panel2);

		}

		//stage2手前
		else if (mouseBtnDownFlag && Mouse::BTN_LEFT && panelcount1 == 1)
		{
			Panel* panel2 = dynamic_cast<Panel*>(item);
			panelcount1++;
			clearpanels[4]->SetPosition(panel2->GetPosition());
			clearpanels[4]->SetAngle(panel2->GetAngle());
			itemManager.Register(clearpanels[4]);

			itemManager.Remove(panel2);

			if (panelcount1 > 1)
			{
				PanelReset();
				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
			}
		}


	}
	
	// 2バウンド目（反射）
	hit2 = false;
	hitPoint2;

	//ライトに対しての処理
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
	//ミラーに対しての処理
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
			Mouse& mouseCursor = Input::Instance().GetMouse();

			//playerの位置を保存
			XMFLOAT3 playerPos = Player::Instance().GetPosition();

			ItemManager& itemManager = ItemManager::Instance();

			Item* item = itemManager.GetItem(hitCloneIndex);
			Light* light = dynamic_cast<Light*>(item);
			//lightの位置を保存
			XMFLOAT3 lightPos = light->GetPosition();

			if (mouseCursor.GetButtonDown() & Mouse::BTN_LEFT)
			{
				//入れ替え
				Player::Instance().SetPosition(lightPos);
				light->SetPosition(playerPos);

			}
		}
	}

	hasReflectHit = hit2;
	reflectedHitPoint = hit2 ? hitPoint2 : hitPoint2;
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

//mirrorに対するレイキャストを共通化
bool Player::RaycastToMirrors(
	const DirectX::XMFLOAT3& rayOrigin,
	const DirectX::XMFLOAT3& rayDir,
	DirectX::XMFLOAT3& outHitPoint,
	DirectX::XMFLOAT3& outHitNormal,
	int& mirrorHitIndex)
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
		XMFLOAT3 mirrorSize = mirror->GetScale();
		mirrorSize.x = 1.0f;
		mirrorSize.y = 1.1f;
		mirrorSize.z = 0.01f;
		mirrorPos.y += mirrorSize.y / 2;



		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsBox(
			rayOrigin, rayDir,
			mirrorPos,mirrorSize,
			 hitDistance,hitPoint))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeBoxNormal(hitPoint, mirrorPos, mirrorSize, rayDir);
				anyHit = true;
				mirrorHitIndex = i;
			}
		}
	}

	return anyHit;
}

//fanに対するレイキャストを共通化
bool Player::RaycastToFans(
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

		Fan* fan = dynamic_cast<Fan*>(item);
		if (!fan) continue;

		XMFLOAT3 fanPos = fan->GetPosition();
		float fan_radius = fan->GetRadius();
		float fan_height = fan->GetHeight();

		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsCylinder(
			rayOrigin, rayDir,
			fanPos, fan_radius, fan_height,
			hitPoint, hitDistance))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeCylinderNormal(hitPoint, fanPos);
				anyHit = true;
				lightHitIndex = i;
			}
		}
	}

	return anyHit;
}

//panelに対するレイキャストを共通化
bool Player::RaycastToPanels(
	const DirectX::XMFLOAT3& rayOrigin,
	const DirectX::XMFLOAT3& rayDir,
	DirectX::XMFLOAT3& outHitPoint,
	DirectX::XMFLOAT3& outHitNormal,
	int& panelHitIndex)
{
	using namespace DirectX;

	ItemManager& itemManager = ItemManager::Instance();
	int itemCount = itemManager.GetItemCount();

	float closestDistance = FLT_MAX;
	bool anyHit = false;

	for (int i = 0; i < itemCount; i++)
	{
		Item* item = itemManager.GetItem(i);

		Panel* panel = dynamic_cast<Panel*>(item);
		if (!panel) continue;

		XMFLOAT3 panelPos = panel->GetPosition();
		float panel_radius = panel->GetRadius();
		float panel_height = panel->GetHeight();

		XMFLOAT3 hitPoint;
		float hitDistance;

		if (Collision::IntersectRayVsCylinder(
			rayOrigin, rayDir,
			panelPos, panel_radius, panel_height,
			hitPoint, hitDistance))
		{
			if (hitDistance < closestDistance)
			{
				closestDistance = hitDistance;
				outHitPoint = hitPoint;
				outHitNormal = ComputeCylinderNormal(hitPoint, panelPos);
				anyHit = true;
				panelHitIndex = i;
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

// Boxからの法線を出す (New function)
DirectX::XMFLOAT3 Player::ComputeBoxNormal(
	const DirectX::XMFLOAT3& hitPoint,
	const DirectX::XMFLOAT3& boxCenter,
	const DirectX::XMFLOAT3& boxSize,
	const DirectX::XMFLOAT3& rayDirection)
{
	using namespace DirectX;

	XMVECTOR hit = XMLoadFloat3(&hitPoint);
	XMVECTOR center = XMLoadFloat3(&boxCenter);
	XMVECTOR halfSize = XMVectorScale(XMLoadFloat3(&boxSize), 0.5f);

	XMVECTOR boxMin = XMVectorSubtract(center, halfSize);
	XMVECTOR boxMax = XMVectorAdd(center, halfSize);

	XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };

	// ヒットした点がボックスのどの面に最も近いかをチェックして法線を決定します。
	// これは簡易的なアプローチであり、複雑なケースや回転したボックスではより洗練された処理が必要になる場合があります。
	if (fabs(hitPoint.x - XMVectorGetX(boxMin)) < FLT_EPSILON) normal.x = -1.0f;
	else if (fabs(hitPoint.x - XMVectorGetX(boxMax)) < FLT_EPSILON) normal.x = 1.0f;
	else if (fabs(hitPoint.y - XMVectorGetY(boxMin)) < FLT_EPSILON) normal.y = -1.0f;
	else if (fabs(hitPoint.y - XMVectorGetY(boxMax)) < FLT_EPSILON) normal.y = 1.0f;
	else if (fabs(hitPoint.z - XMVectorGetZ(boxMin)) < FLT_EPSILON) normal.z = -1.0f;
	else if (fabs(hitPoint.z - XMVectorGetZ(boxMax)) < FLT_EPSILON) normal.z = 1.0f;


	// 法線を正規化
	XMVECTOR nVec = XMVector3Normalize(XMLoadFloat3(&normal));
	XMStoreFloat3(&normal, nVec);

	return normal;
}

//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, local_transform, model, ShaderId::Lambert);
	renderer->Render(rc, parent_transform, model1, ShaderId::Lambert);

	//弾丸描画処理
	projectileManager.Render(rc, renderer);
}

//レイの描画処理
void Player::RayRender(const RenderContext& rc, ShapeRenderer* renderer)
{
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

		// プレイヤーからの1本目（白）
		renderer->RenderLine(rc, rayOrigin, rayHitPoint, { 1.0f, 1.0f, 1.0f, 1.0f });

		
		if (hit3)
		{
			// 反射レイ(2本目)（青など）
			if (hasReflectHit)
			{
				renderer->RenderLine(rc, rayHitPoint, reflectedHitPoint, { 1.0f, 1.0f, 0.0f, 1.0f });
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
		renderer->RenderSphere(rc, rayHitPoint, 0.2f, { 1.0f, 0.0f, 0.0f, 1.0f }); // 赤い円、半径0.2f
	}
	if (hasReflectHit)
	{
		// 衝突点に緑い円を描画
		renderer->RenderSphere(rc, reflectedHitPoint, 0.2f, { 1.0f, 1.0f, 0.0f, 1.0f });
	}
}

//デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	//弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);

	
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

// リセット用関数
void Player::PanelReset()
{
	panelcount = 0;
	panelcount1 = 0;

}