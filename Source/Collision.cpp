#include "Collision.h"
#include <DirectXMath.h>
#include <algorithm> // For std::min, std::max
#include <limits>    // For FLT_MAX

// ���C�Ɖ~���̌������� (Y���ɉ������~����z��)
bool Collision::IntersectRayVsCylinder(
    const DirectX::XMFLOAT3& rayOrigin,
    const DirectX::XMFLOAT3& rayDirection, // �z��Ƃ��Đ��K���ς�
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

    // ���C�Ɖ~���̑��ʂƂ̌������� (�����~��)
    XMVECTOR O_prime = XMVectorSubtract(O, C); // ���C�̌��_���~���̒��S����̑��Έʒu��
    XMVECTOR D_xz = XMVectorSetY(D, 0.0f); // ���C�̕����x�N�g����XZ���ʂɓ��e
    XMVECTOR O_prime_xz = XMVectorSetY(O_prime, 0.0f); // ���C�̑��Ό��_��XZ���ʂɓ��e

    float a = XMVectorGetX(XMVector3LengthSq(D_xz)); // D_x^2 + D_z^2
    float b = 2.0f * XMVectorGetX(XMVector3Dot(O_prime_xz, D_xz)); // 2 * (O'_x * D_x + O'_z * D_z)
    float c = XMVectorGetX(XMVector3LengthSq(O_prime_xz)) - (R * R); // O'_x^2 + O'_z^2 - R^2

    // ���C���~���̒��S���ɕ��s�łȂ��ꍇ (���������̓���������ꍇ)
    if (a > FLT_EPSILON) // a��0�ɋ߂��ꍇ�́A���C���قڐ����ł��邱�Ƃ��Ӗ�����
    {
        float discriminant = b * b - 4 * a * c;
        if (discriminant >= 0)
        {
            float sqrtDisc = sqrtf(discriminant);
            float t0 = (-b - sqrtDisc) / (2.0f * a);
            float t1 = (-b + sqrtDisc) / (2.0f * a);

            // ����������t����`�F�b�N���AY�͈͓��ɂ��邩�m�F
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
            if (t1 > 0.0f && t1 < t_closest) // t1�����߂��ŁA���L����Y�͈͓�
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
    // ���C���قڐ����ȏꍇ�́A���ʏՓ˂̓L���b�v�Փ˂ɂ���ď��������

    // ���C�Ɖ~���̃L���b�v�i�W�ƒ�ʁj�Ƃ̌�������
    float Dy = XMVectorGetY(D);
    if (fabs(Dy) > FLT_EPSILON) // ���C���L���b�v���ʂɕ��s�łȂ��ꍇ
    {
        // ��� (N = (0, 1, 0), P0 = (C.x, cylinderMaxY, C.z))
        float t_cap_top = (cylinderMaxY - XMVectorGetY(O)) / Dy;
        if (t_cap_top > 0.0f && t_cap_top < t_closest)
        {
            XMVECTOR intersectPoint = XMVectorAdd(O, XMVectorScale(D, t_cap_top));
            XMVECTOR hitPointXZ = XMVectorSetY(intersectPoint, 0.0f); // Y������0�ɂ���XZ���ʏ�̈ʒu�Ƃ��Ĉ���
            XMVECTOR cylinderCenterXZ = XMVectorSetY(C, 0.0f); // ���l�ɉ~���̒��S��XZ���ʏ�̈ʒu�Ƃ��Ĉ���
            float distSqToCenter = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(hitPointXZ, cylinderCenterXZ)));
            if (distSqToCenter <= (R * R))
            {
                t_closest = t_cap_top;
                hit_found = true;
            }
        }

        // ��� (N = (0, -1, 0), P0 = (C.x, cylinderMinY, C.z))
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
    // ���C���L���b�v���ʂɕ��s�ȏꍇ (Dy == 0):
    // ���̏ꍇ�A�L���b�v�Ƃ̌����͔������Ȃ��B���ʏՓ˂̂ݍl������΂悢���A
    // ����͊��ɏ�� quadratic �ŏ�������Ă���B

    if (hit_found)
    {
        XMVECTOR finalHitPoint = XMVectorAdd(O, XMVectorScale(D, t_closest));
        XMStoreFloat3(&outHitPoint, finalHitPoint);
        outHitDistance = t_closest;
        return true;
    }

    return false;
}

//���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB) 
{
	//A��B�̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);//XMFLOAT3��XMVECTOR�ɕϊ�
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB,PositionA);//pB-pA
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//�O�����̒藝��Vec�̒������v�Z
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//��������
	float range = radiusA + radiusB;
	if (lengthSq > range * range) 
	{
		return false;
	}

	//A��B�������o��
	Vec = DirectX::XMVector3Normalize(Vec);//pB�ւ̕����Ƀx�N�g����P�ʃx�N�g���ɒ���
	Vec = DirectX::XMVectorScale(Vec, range);//�P�ʃx�N�g�������ɔ��a�������L�΂�
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);//PositionA+���a�������L�΂����ʒu�������o���ʒu�ɐݒ肵��
	DirectX::XMStoreFloat3(&outPositionB, PositionB);//XMFLOAT3�ɖ߂�

	return true;
}

//�~���Ɖ~���̌�������
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, 
	float radiusA, 
	float heightA,
	const DirectX::XMFLOAT3& positionB, 
	float radiusB, 
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	//A�̑�����B�̓�����Ȃ瓖�����ĂȂ�
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}

	//A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	//XZ���ʂł͈̔̓`�F�b�N
	//x�����������Z
	float vx = positionB.x - positionA.x;
	//z�����������Z
	float vz = positionB.z - positionA.z;
	//���a�̍��v���v�Z
	float range = radiusA + radiusB;
	//A����B�ւ̃x�N�g�������߂�
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//�P�ʃx�N�g����
	vx /= distXZ;
	vz /= distXZ;


	//A��B�������o��
	outPositionB.x = positionA.x + (range * vx);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (range * vz);

	return true;
}

//���Ɖ~���̌�������
bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition, 
	float sphereRadius, 
	const DirectX::XMFLOAT3& cylinderPosition, 
	float cylinderRadius, 
	float cylinderHeight, 
	DirectX::XMFLOAT3& outCylinderPosition)
{


	//���̑����i�ʒu-���a�j���~���̓�����Ȃ瓖�����ĂȂ�
	if (spherePosition.y-sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}

	//���̓��i�ʒu+���a�j���~���̑�����艺�Ȃ瓖�����Ă��Ȃ�
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	//XZ���ʂł͈̔̓`�F�b�N
	//x�����������Z
	float vx = cylinderPosition.x - spherePosition.x;
	//z�����������Z
	float vz = cylinderPosition.z - spherePosition.z;
	//���a�̍��v���v�Z
	float range = sphereRadius + cylinderRadius;
	//A����B�ւ̃x�N�g�������߂�
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//�P�ʃx�N�g����
	vx /= distXZ;
	vz /= distXZ;


	//A��B�������o��
	outCylinderPosition.x = spherePosition.x + (range * vx);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (range * vz);

	return true;
}
