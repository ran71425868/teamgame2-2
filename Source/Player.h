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

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//ジャンプ入力処理
	void InputJump();

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//移動入力処理
	void InputMove(float elapsedTime);

	// カメラの向きとプレイヤーの角度を同期
	void SyncPlayerAngleWithCamera();

	//プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// レイキャスト処理 (追加)
	void PerformRaycastToSlime();

	//弾丸入力処理
	void InputProjectile();

	//弾丸と敵の衝突判定
	void CollisionProjectilesVsEnemies();

protected:
	//着地した時に呼ばれる
	void OnLanding() override;


private:
	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 12.0f;

	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;//弾丸管理

	Effect* hitEffect = nullptr;

	AudioSource* hitSE = nullptr;

	// レイキャスト結果を保持するメンバ変数 (追加)
	bool hasRayHit = false;
	DirectX::XMFLOAT3 rayHitPoint = { 0, 0, 0 };
};