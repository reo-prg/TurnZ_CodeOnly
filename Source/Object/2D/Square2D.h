#pragma once
#include "Object2D.h"

class Square2D :
    public Object2D
{
public:
    Square2D() = default;
    ~Square2D();

    /// <summary>
    /// �I�u�W�F�N�g�̏�����
    /// </summary>
    /// <param name="center_x">���S���W</param>
    /// <param name="center_y">���S���W</param>
    /// <param name="size_x">X�����̑傫��</param>
    /// <param name="size_y">Y�����̑傫��</param>
    /// <param name="color">�F</param>
    /// <param name="rotation">��]</param>
    void CreateObject(float center_x, float center_y, float size_x, float size_y,
        const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 0.0f, 1.0f },
        const DirectX::XMMATRIX& rotation = DirectX::XMMatrixIdentity());

    /// <summary>
    /// �I�u�W�F�N�g�̏�����
    /// </summary>
    /// <param name="filename">�摜�̃p�X</param>
    /// <param name="center_x">���S���W</param>
    /// <param name="center_y">���S���W</param>
    /// <param name="size_x">X�����̑傫��</param>
    /// <param name="size_y">Y�����̑傫��</param>
    /// <param name="color">�F</param>
    /// <param name="rotation">��]</param>
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
