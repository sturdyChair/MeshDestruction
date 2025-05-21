#include "Engine_Defines.h"

Engine::_float4x4 Engine::Matrix_Inverse(const Engine::_float4x4& Matrix)
{
	auto LoadedMatrix = XMLoadFloat4x4(&Matrix);
	LoadedMatrix = XMMatrixInverse(nullptr, LoadedMatrix);
	Engine::_float4x4 MatRet;
	XMStoreFloat4x4(&MatRet, LoadedMatrix);
	return MatRet;
}

_float3 Engine::Get_Cursor_Pos(HWND hWnd)
{
	POINT pPt = {};
	GetCursorPos(&pPt);
	ScreenToClient(hWnd, &pPt);
	return _float3{ (float)pPt.x,(float)pPt.y, 0.f };

}

void Engine::XMFloat4x4ToFloatArray(const XMFLOAT4X4& mat, float out[16])
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			out[i * 4 + j] = mat.m[i][j];
		}
	}
}

void Engine::FloatArrayToXMFloat4x4(const float in[16], XMFLOAT4X4& mat)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat.m[i][j] = in[i * 4 + j];
		}
	}
}

PxMat44 Engine::XMMatrixToPxMat(_fmatrix matrix)
{
	PxMat44 mat;
	mat.column0.x = matrix.r[0].m128_f32[0];
	mat.column0.y = matrix.r[0].m128_f32[1];
	mat.column0.z = matrix.r[0].m128_f32[2];
	mat.column0.w = matrix.r[0].m128_f32[3];
	mat.column1.x = matrix.r[1].m128_f32[0];
	mat.column1.y = matrix.r[1].m128_f32[1];
	mat.column1.z = matrix.r[1].m128_f32[2];
	mat.column1.w = matrix.r[1].m128_f32[3];
	mat.column2.x = matrix.r[2].m128_f32[0];
	mat.column2.y = matrix.r[2].m128_f32[1];
	mat.column2.z = matrix.r[2].m128_f32[2];
	mat.column2.w = matrix.r[2].m128_f32[3];
	mat.column3.x = matrix.r[3].m128_f32[0];
	mat.column3.y = matrix.r[3].m128_f32[1];
	mat.column3.z = matrix.r[3].m128_f32[2];
	mat.column3.w = matrix.r[3].m128_f32[3];
	return mat;
}

_matrix Engine::PxMatToXMMatrix(PxMat44 matrix)
{
	_matrix mat;
	mat.r[0].m128_f32[0] = matrix.column0.x;
	mat.r[0].m128_f32[1] = matrix.column0.y;
	mat.r[0].m128_f32[2] = matrix.column0.z;
	mat.r[0].m128_f32[3] = matrix.column0.w;
	mat.r[1].m128_f32[0] = matrix.column1.x;
	mat.r[1].m128_f32[1] = matrix.column1.y;
	mat.r[1].m128_f32[2] = matrix.column1.z;
	mat.r[1].m128_f32[3] = matrix.column1.w;
	mat.r[2].m128_f32[0] = matrix.column2.x;
	mat.r[2].m128_f32[1] = matrix.column2.y;
	mat.r[2].m128_f32[2] = matrix.column2.z;
	mat.r[2].m128_f32[3] = matrix.column2.w;
	mat.r[3].m128_f32[0] = matrix.column3.x;
	mat.r[3].m128_f32[1] = matrix.column3.y;
	mat.r[3].m128_f32[2] = matrix.column3.z;
	mat.r[3].m128_f32[3] = matrix.column3.w;
	return mat;
}

_matrix Engine::XMMatrixNormalize(_fmatrix matrix)
{
	_matrix rt{};
	rt.r[0] = XMVector3Normalize(matrix.r[0]);
	rt.r[1] = XMVector3Normalize(matrix.r[1]);
	rt.r[2] = XMVector3Normalize(matrix.r[2]);
	rt.r[3] = matrix.r[3];
	return rt;
}

_float Engine::fRand()
{
	return rand() % 1000 * 0.001f;
}

_float Engine::fRand(_float min, _float max)
{
	return min + fRand() * (max - min);
}
