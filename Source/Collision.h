#pragma once

#include <DirectXMath.h>

class Collision
{
public:
	//‹…‚Æ‹…‚ÌŒğ·”»’è
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	//‰~’Œ‚Æ‰~’Œ‚ÌŒğ·”»’è
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	//‹…‚Æ‰~’Œ‚ÌŒğ·”»’è
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	// ƒŒƒC‚Æ‰~’Œ‚ÌŒğ·”»’è (Y²‚É‰ˆ‚Á‚½‰~’Œ‚ğ‘z’è)
	static bool IntersectRayVsCylinder(
		const DirectX::XMFLOAT3& rayOrigin,
		const DirectX::XMFLOAT3& rayDirection, // ‘z’è‚Æ‚µ‚Ä³‹K‰»Ï‚İ
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outHitPoint,
		float& outHitDistance
	);
};