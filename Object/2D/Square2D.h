#pragma once
#include "Object2D.h"

class Square2D :
    public Object2D
{
public:
    Square2D() = default;
    ~Square2D();

    /// <summary>
    /// オブジェクトの初期化
    /// </summary>
    /// <param name="center_x">中心座標</param>
    /// <param name="center_y">中心座標</param>
    /// <param name="size_x">X方向の大きさ</param>
    /// <param name="size_y">Y方向の大きさ</param>
    /// <param name="color">色</param>
    /// <param name="rotation">回転</param>
    void CreateObject(float center_x, float center_y, float size_x, float size_y,
        const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 0.0f, 1.0f },
        const DirectX::XMMATRIX& rotation = DirectX::XMMatrixIdentity());

    /// <summary>
    /// オブジェクトの初期化
    /// </summary>
    /// <param name="filename">画像のパス</param>
    /// <param name="center_x">中心座標</param>
    /// <param name="center_y">中心座標</param>
    /// <param name="size_x">X方向の大きさ</param>
    /// <param name="size_y">Y方向の大きさ</param>
    /// <param name="color">色</param>
    /// <param name="rotation">回転</param>
    void CreateObject(std::string filename,
        float center_x, float center_y, float size_x, float size_y,
        const DirectX::XMMATRIX& rotation = DirectX::XMMatrixIdentity());

    void resetSize(float size_x, float size_y);

    void SetUV(float top, float left, float bottom, float right)override;
private:
	static constexpr unsigned int vertexCount_ = 4;
	static constexpr unsigned int indexCount_ = 6;
	static constexpr unsigned short indexData_[indexCount_] =
	{
        0, 2, 1, 2, 3, 1
	};
};

typedef Square2D Image2D;
