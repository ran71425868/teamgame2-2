#include "Collision.h"

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
