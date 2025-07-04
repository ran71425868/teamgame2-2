#pragma once

#include "System/Model.h" // Modelクラスの定義があるヘッダーファイル
#include <DirectXMath.h>  // DirectXMathを使用するため

// 前方宣言 (RenderContext が他のヘッダーで定義されている場合)
struct RenderContext;
class ModelRenderer;
//
//// シーンに配置する汎用的なモデルオブジェクト
//class Prop
//{
//public:
//    // コンストラクタ
//    // modelPath: ロードするMDLファイルのパス
//    // position: オブジェクトの初期位置
//    // scale: オブジェクトのスケール (デフォルトは1.0f)
//    Prop(const char* modelPath, const DirectX::XMFLOAT3& position, float scale = 1.0f);
//
//    // デストラクタ
//    ~Prop();
//
//    // 更新処理 (必要に応じて追加)
//    void Update(float elapsedTime);
//
//    // 描画処理
//    void Render(const RenderContext& rc, ModelRenderer* renderer);
//
//    // 位置設定
//    void SetPosition(const DirectX::XMFLOAT3& position) { m_position = position; }
//
//    // スケール設定
//    void SetScale(float scale) { m_scale = { scale, scale, scale }; }
//    void SetScale(const DirectX::XMFLOAT3& scale) { m_scale = scale; }
//
//    // 位置取得
//    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }
//
//private:
//    Model* m_model;                   // ロードするモデル
//    DirectX::XMFLOAT3 m_position;     // 位置
//    DirectX::XMFLOAT3 m_scale;        // スケール
//    DirectX::XMFLOAT3 m_rotation;     // 回転 (必要に応じて使用)
//
//    // コピーコンストラクタと代入演算子の禁止 (シングルトンではないため、必要に応じて実装)
//    Prop(const Prop&) = delete;
//    Prop& operator=(const Prop&) = delete;
//};
