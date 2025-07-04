#include "Misc.h"
#include "GpuResourceUtils.h"
#include "ShapeRenderer.h"
#include <algorithm>

// コンストラクタ
ShapeRenderer::ShapeRenderer(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/ShapeRendererVS.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf());

	// ピクセルシェーダー
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/ShapeRendererPS.cso",
		pixelShader.GetAddressOf());

	// 定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		constantBuffer.GetAddressOf());

	// 箱メッシュ生成
	CreateBoxMesh(device, 1.0f, 1.0f, 1.0f);

	// 球メッシュ生成
	CreateSphereMesh(device, 1.0f, 32);

	// 半球メッシュ生成
	CreateHalfSphereMesh(device, 1.0f, 32);

	// 円柱メッシュ生成
	CreateCylinderMesh(device, 1.0f, 1.0f, -0.5f, 1.0f, 32);

	//線メッシュ生成
	CreateLineMesh(device);
}

// 箱描画
void ShapeRenderer::RenderBox(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& angle,
	const DirectX::XMFLOAT3& size,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * R * T);

	Render(rc, boxMesh, transform, color);
}

// 球描画
void ShapeRenderer::RenderSphere(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	float radius,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * T);

	Render(rc, sphereMesh, transform, color);
}

// 円柱描画
void ShapeRenderer::RenderCylinder(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	float radius,
	float height,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, height, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y + height * 0.5f, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * T);

	Render(rc, cylinderMesh, transform, color);
}

// カプセル描画
void ShapeRenderer::RenderCapsule(
	const RenderContext& rc,
	const DirectX::XMFLOAT4X4& transform,
	float radius,
	float height,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	// 上半球
	{
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, height * 0.5f, 0, 0), Transform);
		DirectX::XMMATRIX World = DirectX::XMMatrixScaling(radius, radius, radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, halfSphereMesh, world, color);
	}
	// 円柱
	{
		DirectX::XMMATRIX World;
		World.r[0] = DirectX::XMVectorScale(Transform.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(Transform.r[1], height);
		World.r[2] = DirectX::XMVectorScale(Transform.r[2], radius);
		World.r[3] = Transform.r[3];
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, cylinderMesh, world, color);
	}
	// 下半球
	{
		DirectX::XMMATRIX World = DirectX::XMMatrixRotationX(DirectX::XM_PI);
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, -height * 0.5f, 0, 0), Transform);
		Transform.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		World = DirectX::XMMatrixMultiply(World, Transform);
		World.r[0] = DirectX::XMVectorScale(World.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(World.r[1], radius);
		World.r[2] = DirectX::XMVectorScale(World.r[2], radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, halfSphereMesh, world, color);
	}
}

void ShapeRenderer::RenderLine(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4& color) const
{
	// 単位ベクトル（0,0,1）から (start→end) ベクトルへの変換を行う
	DirectX::XMVECTOR dir = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&end),
		DirectX::XMLoadFloat3(&start));
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(dir));

	if (length < 1e-5f)
		return;

	// 単位方向ベクトル
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(dir);
	DirectX::XMVECTOR defaultDir = DirectX::XMVectorSet(0, 0, 1, 0); // 初期線の方向

	// 回転軸と角度を求める
	DirectX::XMVECTOR axis = DirectX::XMVector3Cross(defaultDir, direction);
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(defaultDir, direction));
	float angle = acosf(std::clamp(dot, -1.0f, 1.0f)); // 安全なacos

	DirectX::XMMATRIX R;
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(axis)) < 1e-5f)
	{
		R = (dot > 0.0f) ? DirectX::XMMatrixIdentity() : DirectX::XMMatrixRotationY(DirectX::XM_PI);
	}
	else
	{
		R = DirectX::XMMatrixRotationAxis(axis, angle);
	}

	// スケーリングと移動
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, length);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&start));
	DirectX::XMMATRIX World = S * R * T;

	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, World);

	Render(rc, lineMesh, transform, color);
}


// メッシュ生成
void ShapeRenderer::CreateMesh(ID3D11Device* device, const std::vector<DirectX::XMFLOAT3>& vertices, Mesh& mesh)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	mesh.vertexCount = static_cast<UINT>(vertices.size());
}

// 箱メッシュ作成
void ShapeRenderer::CreateBoxMesh(ID3D11Device* device, float width, float height, float depth)
{
	DirectX::XMFLOAT3 positions[8] =
	{
		// top
		{ -width,  height, -depth},
		{  width,  height, -depth},
		{  width,  height,  depth},
		{ -width,  height,  depth},
		// bottom
		{ -width, -height, -depth},
		{  width, -height, -depth},
		{  width, -height,  depth},
		{ -width, -height,  depth},
	};

	std::vector<DirectX::XMFLOAT3> vertices;
	vertices.resize(32);

	// top
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[0]);
	// bottom
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[4]);
	// side
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[7]);

	// メッシュ生成
	CreateMesh(device, vertices, boxMesh);
}

// 球メッシュ作成
void ShapeRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	float step = DirectX::XM_2PI / subdivisions;

	std::vector<DirectX::XMFLOAT3> vertices;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = 0.0f;
			p.z = cosf(theta) * radius;
		}
	}
	// XY平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = cosf(theta) * radius;
			p.z = 0.0f;
		}
	}
	// YZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = 0.0f;
			p.y = sinf(theta) * radius;
			p.z = cosf(theta) * radius;
		}
	}

	// メッシュ生成
	CreateMesh(device, vertices, sphereMesh);
}

// 半球メッシュ作成
void ShapeRenderer::CreateHalfSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	std::vector<DirectX::XMFLOAT3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = 0.0f;
			v.z = cosf(theta) * radius;
		}
	}
	// XY平面
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions) - DirectX::XM_PIDIV2;

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = cosf(theta) * radius;
			v.z = 0.0f;
		}
	}
	// YZ平面
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = 0.0f;
			v.y = sinf(theta) * radius;
			v.z = cosf(theta) * radius;
		}
	}

	// メッシュ生成
	CreateMesh(device, vertices, halfSphereMesh);
}

// 円柱
void ShapeRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int subdivisions)
{
	std::vector<DirectX::XMFLOAT3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ平面
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius1;
			v.y = start;
			v.z = cosf(theta) * radius1;
		}
	}
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius2;
			v.y = start + height;
			v.z = cosf(theta) * radius2;
		}
	}
	// XY平面
	{
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start, radius1));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start + height, radius2));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start, -radius1));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start + height, -radius2));
	}
	// YZ平面
	{
		vertices.emplace_back(DirectX::XMFLOAT3(radius1, start, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(radius2, start + height, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(-radius1, start, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(-radius2, start + height, 0.0f));
	}

	// メッシュ生成
	CreateMesh(device, vertices, cylinderMesh);
}

void ShapeRenderer::CreateLineMesh(ID3D11Device* device)
{
	std::vector<DirectX::XMFLOAT3> vertices(2);
	// 初期化時はダミー位置でOK（描画時にtransformで座標設定する）
	vertices[0] = { 0, 0, 0 };
	vertices[1] = { 0, 0, 1 };

	CreateMesh(device, vertices, lineMesh);
}

// 描画実行
void ShapeRenderer::Render(const RenderContext& rc, const Mesh& mesh, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color) const
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// 定数バッファ設定
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// ビュープロジェクション行列作成
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX VP = V * P;

	// プリミティブ設定
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 頂点バッファ設定
	dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

	// ワールドビュープロジェクション行列作成
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX WVP = W * VP;

	// 定数バッファ更新
	CbMesh cbMesh;
	DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
	cbMesh.color = color;

	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	// 描画
	dc->Draw(mesh.vertexCount, 0);
}
