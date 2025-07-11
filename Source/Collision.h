#pragma once

#include <DirectXMath.h>

class Collision
{
public:
	//球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	//円柱と円柱の交差判定
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	//球と円柱の交差判定
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	// レイと円柱の交差判定 (Y軸に沿った円柱を想定)
	static bool IntersectRayVsCylinder(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDirection, // 想定として正規化済み
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outHitPoint,
		float& outHitDistance
	);

	// レイと直方体の交差判定（交点と距離も出力）
	static bool IntersectRayVsBox(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDirection,
		const DirectX::XMFLOAT3& boxCenter,
		const DirectX::XMFLOAT3& boxSize,
		float& outTMin,
		DirectX::XMFLOAT3& outHitPoint  // ← 追加：ヒットした座標
	);
};