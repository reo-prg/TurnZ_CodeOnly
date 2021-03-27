#pragma once
#include <cmath>

struct Matrix3
{
	float matrix_[9];

	Matrix3(float v00, float v01, float v02,
		float v10, float v11, float v12,
		float v20, float v21, float v22)
	{
		matrix_[0] = v00;
		matrix_[1] = v01;
		matrix_[2] = v02;
		matrix_[3] = v10;
		matrix_[4] = v11;
		matrix_[5] = v12;
		matrix_[6] = v20;
		matrix_[7] = v21;
		matrix_[8] = v22;
	}

	~Matrix3() = default;

	Matrix3 operator*(const Matrix3& m)
	{
		return Matrix3(matrix_[0] * m.matrix_[0] + matrix_[1] * m.matrix_[3] + matrix_[2] * m.matrix_[6],
			matrix_[0] * m.matrix_[1] + matrix_[1] * m.matrix_[4] + matrix_[2] * m.matrix_[7],
			matrix_[0] * m.matrix_[2] + matrix_[1] * m.matrix_[5] + matrix_[2] * m.matrix_[8],
			matrix_[3] * m.matrix_[0] + matrix_[4] * m.matrix_[3] + matrix_[5] * m.matrix_[6],
			matrix_[3] * m.matrix_[1] + matrix_[4] * m.matrix_[4] + matrix_[5] * m.matrix_[7],
			matrix_[3] * m.matrix_[2] + matrix_[4] * m.matrix_[5] + matrix_[5] * m.matrix_[8],
			matrix_[6] * m.matrix_[0] + matrix_[7] * m.matrix_[3] + matrix_[8] * m.matrix_[6],
			matrix_[6] * m.matrix_[1] + matrix_[7] * m.matrix_[4] + matrix_[8] * m.matrix_[7],
			matrix_[6] * m.matrix_[2] + matrix_[7] * m.matrix_[5] + matrix_[8] * m.matrix_[8]);
	}
};

Matrix3 RotationMatrix(float angle)
{
	return Matrix3(cos(angle), -sin(angle), 0.0f,
		sin(angle), cos(angle), 0.0f,
		0.0f, 0.0f, 1.0f);
}

Matrix3 TranslationMatrix(float x, float y)
{
	return Matrix3(1.0f, 0.0f, x,
		0.0f, 1.0f, y,
		0.0f, 0.0f, 1.0f);
}

Matrix3 IdentityMatrix(void)
{
	return Matrix3(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

