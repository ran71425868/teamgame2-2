//#include "Prop.h"
//#include "System/Model.h"   // Modelクラスの定義があるヘッダーファイル
//#include "System/Graphics.h" // RenderContextやModelRendererが関連する可能性のあるヘッダー
//
//// コンストラクタ
//Prop::Prop(const char* modelPath, const DirectX::XMFLOAT3& position, float scale)
//    : m_position(position),
//    m_scale({ scale, scale, scale }), // 初期スケールを設定
//    m_rotation({ 0.0f, 0.0f, 0.0f })   // 初期回転を設定 (必要に応じて変更)
//{
//    m_model = new Model(modelPath); // MDLファイルをロード
//}
//
//// デストラクタ
//Prop::~Prop()
//{
//    if (m_model)
//    {
//        delete m_model;
//        m_model = nullptr;
//    }
//}
//
//// 更新処理 (必要に応じて追加、現状は空で問題ありません)
//void Prop::Update(float elapsedTime)
//{
//    // 必要に応じて、位置や回転の更新ロジックをここに記述
//}
//
//// 描画処理
//void Prop::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//    if (!m_model || !renderer)
//    {
//        return; // モデルまたはレンダラーが無効なら何もしない
//    }
//
//    // ワールド行列の作成
//    // スケール -> 回転 -> 移動 の順に適用するのが一般的
//    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
//    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
//    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//
//    DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
//
//    // RenderContext にワールド行列を設定
//    RenderContext currentRc = rc; // RenderContext はコピーして使用
//    DirectX::XMStoreFloat4x4(&currentRc.world, worldMatrix); // XMFLOAT4x4に変換して保存
//
//    // モデルを描画
//    m_model->Render(currentRc, renderer);
//}