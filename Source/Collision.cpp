#include "Collision.h"
#include <DirectXMath.h>
#include <algorithm> // For std::min, std::max
#include <limits>    // For FLT_MAX

// レイと円柱の交差判定 (Y軸に沿った円柱を想定)
bool Collision::IntersectRayVsCylinder(
    const DirectX::XMFLOAT3& rayOrigin,
    const DirectX::XMFLOAT3& rayDirection, // 想定として正規化済み
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outHitPoint,
    float& outHitDistance)
{
    using namespace DirectX;

    XMVECTOR O = XMLoadFloat3(&rayOrigin);
    XMVECTOR D = XMLoadFloat3(&rayDirection);
    XMVECTOR C = XMLoadFloat3(&cylinderPosition);

    float R = cylinderRadius;
    float H = cylinderHeight;

    float halfH = H;
    float cylinderMinY = cylinderPosition.y;
    float cylinderMaxY = cylinderPosition.y + halfH;

    float t_closest = FLT_MAX;
    bool hit_found = false;

    // レイと円柱の側面との交差判定 (無限円柱)
    XMVECTOR O_prime = XMVectorSubtract(O, C); // レイの原点を円柱の中心からの相対位置に
    XMVECTOR D_xz = XMVectorSetY(D, 0.0f); // レイの方向ベクトルをXZ平面に投影
    XMVECTOR O_prime_xz = XMVectorSetY(O_prime, 0.0f); // レイの相対原点をXZ平面に投影

    float a = XMVectorGetX(XMVector3LengthSq(D_xz)); // D_x^2 + D_z^2
    float b = 2.0f * XMVectorGetX(XMVector3Dot(O_prime_xz, D_xz)); // 2 * (O'_x * D_x + O'_z * D_z)
    float c = XMVectorGetX(XMVector3LengthSq(O_prime_xz)) - (R * R); // O'_x^2 + O'_z^2 - R^2

    // レイが円柱の中心軸に平行でない場合 (水平方向の動きがある場合)
    if (a > FLT_EPSILON) // aが0に近い場合は、レイがほぼ垂直であることを意味する
    {
        float discriminant = b * b - 4 * a * c;
        if (discriminant >= 0)
        {
            float sqrtDisc = sqrtf(discriminant);
            float t0 = (-b - sqrtDisc) / (2.0f * a);
            float t1 = (-b + sqrtDisc) / (2.0f * a);

            // 小さい方のtからチェックし、Y範囲内にあるか確認
            if (t0 > 0.0f)
            {
                XMVECTOR intersectPoint = XMVectorAdd(O, XMVectorScale(D, t0));
                float hitY = XMVectorGetY(intersectPoint);
                if (hitY >= cylinderMinY && hitY <= cylinderMaxY)
                {
                    t_closest = t0;
                    hit_found = true;
                }
            }
            if (t1 > 0.0f && t1 < t_closest) // t1がより近くで、かつ有効なY範囲内
            {
                XMVECTOR intersectPoint = XMVectorAdd(O, XMVectorScale(D, t1));
                float hitY = XMVectorGetY(intersectPoint);
                if (hitY >= cylinderMinY && hitY <= cylinderMaxY)
                {
                    t_closest = t1;
                    hit_found = true;
                }
            }
        }
    }
    // レイがほぼ垂直な場合は、側面衝突はキャップ衝突によって処理される

    // レイと円柱のキャップ（蓋と底面）との交差判定
    float Dy = XMVectorGetY(D);
    if (fabs(Dy) > FLT_EPSILON) // レイがキャップ平面に平行でない場合
    {
        // 上面 (N = (0, 1, 0), P0 = (C.x, cylinderMaxY, C.z))
        float t_cap_top = (cylinderMaxY - XMVectorGetY(O)) / Dy;
        if (t_cap_top > 0.0f && t_cap_top < t_closest)
        {
            XMVECTOR intersectPoint = XMVectorAdd(O, XMVectorScale(D, t_cap_top));
            XMVECTOR hitPointXZ = XMVectorSetY(intersectPoint, 0.0f); // Y成分を0にしてXZ平面上の位置として扱う
            XMVECTOR cylinderCenterXZ = XMVectorSetY(C, 0.0f); // 同様に円柱の中心もXZ平面上の位置として扱う
            float distSqToCenter = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(hitPointXZ, cylinderCenterXZ)));
            if (distSqToCenter <= (R * R))
            {
                t_closest = t_cap_top;
                hit_found = true;
            }
        }

        // 底面 (N = (0, -1, 0), P0 = (C.x, cylinderMinY, C.z))
        float t_cap_bottom = (cylinderMinY - XMVectorGetY(O)) / Dy;
        if (t_cap_bottom > 0.0f && t_cap_bottom < t_closest)
        {
            XMVECTOR intersectPoint = XMVectorAdd(O, XMVectorScale(D, t_cap_bottom));
            XMVECTOR hitPointXZ = XMVectorSetY(intersectPoint, 0.0f);
            XMVECTOR cylinderCenterXZ = XMVectorSetY(C, 0.0f);
            float distSqToCenter = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(hitPointXZ, cylinderCenterXZ)));
            if (distSqToCenter <= (R * R))
            {
                t_closest = t_cap_bottom;
                hit_found = true;
            }
        }
    }
    // レイがキャップ平面に平行な場合 (Dy == 0):
    // この場合、キャップとの交差は発生しない。側面衝突のみ考慮すればよいが、
    // それは既に上の quadratic で処理されている。

    if (hit_found)
    {
        XMVECTOR finalHitPoint = XMVectorAdd(O, XMVectorScale(D, t_closest));
        XMStoreFloat3(&outHitPoint, finalHitPoint);
        outHitDistance = t_closest;
        return true;
    }

    return false;
}

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB) 
{
	//A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);//XMFLOAT3をXMVECTORに変換
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB,PositionA);//pB-pA
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//三平方の定理でVecの長さを計算
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range) 
	{
		return false;
	}

	//AがBを押し出す
	Vec = DirectX::XMVector3Normalize(Vec);//pBへの方向にベクトルを単位ベクトルに直す
	Vec = DirectX::XMVectorScale(Vec, range);//単位ベクトル方向に半径分引き伸ばす
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);//PositionA+半径分引き伸ばした位置を押し出し位置に設定して
	DirectX::XMStoreFloat3(&outPositionB, PositionB);//XMFLOAT3に戻す

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, 
	float radiusA, 
	float heightA,
	const DirectX::XMFLOAT3& positionB, 
	float radiusB, 
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	//Aの足元がBの頭より上なら当たってない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}

	//Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	//XZ平面での範囲チェック
	//x成分を引き算
	float vx = positionB.x - positionA.x;
	//z成分を引き算
	float vz = positionB.z - positionA.z;
	//半径の合計を計算
	float range = radiusA + radiusB;
	//AからBへのベクトルを求める
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;


	//AがBを押し出す
	outPositionB.x = positionA.x + (range * vx);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (range * vz);

	return true;
}

//球と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition, 
	float sphereRadius, 
	const DirectX::XMFLOAT3& cylinderPosition, 
	float cylinderRadius, 
	float cylinderHeight, 
	DirectX::XMFLOAT3& outCylinderPosition)
{


	//球の足元（位置-半径）が円柱の頭より上なら当たってない
	if (spherePosition.y-sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}

	//球の頭（位置+半径）が円柱の足元より下なら当たっていない
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	//XZ平面での範囲チェック
	//x成分を引き算
	float vx = cylinderPosition.x - spherePosition.x;
	//z成分を引き算
	float vz = cylinderPosition.z - spherePosition.z;
	//半径の合計を計算
	float range = sphereRadius + cylinderRadius;
	//AからBへのベクトルを求める
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;


	//AがBを押し出す
	outCylinderPosition.x = spherePosition.x + (range * vx);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (range * vz);

	return true;
}
