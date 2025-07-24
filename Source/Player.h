#pragma once

#include "System/ModelRenderer.h"
#include "Character.h" 
#include "ProjectileManager.h"
#include "Effect.h"
#include "System/AudioSource.h"
#include <DirectXMath.h>

//プレイやー
class Player : public Character
{
private:
	Player() {};
	~Player() override {};

public:
	//インスタンス取得
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	//初期化
	void Initializa();

	//終了化
	void Finalize();

	//更新処理
	void Update(float elapsedTime);

	//プレイヤー用行列計算
	void ModelUpdateTransform();

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//プレイヤーの角度にセット
	void SetAngle(const DirectX::XMFLOAT3 angle) { this->angle = angle; };

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// ベクトルの反射
	DirectX::XMFLOAT3 Reflect(const DirectX::XMFLOAT3& incident, const DirectX::XMFLOAT3& normal);

	//移動入力処理
	void InputMove(float elapsedTime);

	// カメラの向きとプレイヤーの角度を同期
	void SyncPlayerAngleWithCamera();


	void PerformRaycastToLight();





private:
	Model* model = nullptr;
	Model* model1 = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 12.0f;

	int jumpCount = 0;
	int jumpLimit = 2;

	int panelcount = 0;


	DirectX::XMFLOAT3 hitPoint2;
	DirectX::XMFLOAT3 reflectedDir;
	bool hit1;
	bool hit2;
	bool hit3;
	bool hitFan;
	bool hit_panel;


	bool prevMouseBtnState = false;

	ProjectileManager projectileManager;//弾丸管理

	Effect* hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

	// レイキャスト結果を保持するメンバ変数 (追加)
	bool hasRayHit = false;
	DirectX::XMFLOAT3 rayHitPoint = { 0, 0, 0 };

	// レイキャスト結果を保持するメンバ変数(反射後)
	bool hasReflectHit = false;
	DirectX::XMFLOAT3 reflectedHitPoint = { 0, 0, 0 };

	// Cylinderからの法線
	DirectX::XMFLOAT3 ComputeCylinderNormal(
		const DirectX::XMFLOAT3& hitPoint,
		const DirectX::XMFLOAT3& cylinderCenter);

	// Boxからの法線
	DirectX::XMFLOAT3 ComputeBoxNormal(
		const DirectX::XMFLOAT3& hitPoint,
		const DirectX::XMFLOAT3& boxCenter,
		const DirectX::XMFLOAT3& boxSize,
		const DirectX::XMFLOAT3& rayDirection);

	DirectX::XMFLOAT3 ComputeOBBNormal(
		const DirectX::XMFLOAT3& hitPoint,
		const DirectX::XMMATRIX& worldMatrix,
		const DirectX::XMFLOAT3& boxSize,
		const DirectX::XMFLOAT3& rayDir);

	//ライトに対するレイキャスト
	bool Player::RaycastToLights(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDir,
		DirectX::XMFLOAT3& outHitPoint,
		DirectX::XMFLOAT3& outHitNormal,
		int& lightHitIndex);

	//ミラーに対するレイキャスト
	bool RaycastToMirrors(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDir,
		DirectX::XMFLOAT3& outHitPoint,
		DirectX::XMFLOAT3& outHitNormal,
		int& mirrorHitIndex);

	//扇風機に対するレイキャスト
	bool Player::RaycastToFans(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDir,
		DirectX::XMFLOAT3& outHitPoint,
		DirectX::XMFLOAT3& outHitNormal,
		int& lightHitIndex);

	//パネルに対するレイキャスト
	bool Player::RaycastToPanels(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDir,
		DirectX::XMFLOAT3& outHitPoint,
		DirectX::XMFLOAT3& outHitNormal,
		int& panelHitIndex);
};