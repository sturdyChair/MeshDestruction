#include "Math_Manager.h"

CMath_Manager::CMath_Manager()
{

}

CMath_Manager::~CMath_Manager()
{
	Free();
}

void CMath_Manager::Initialize()
{

}

void CMath_Manager::Tick(_float fTimeDelta)
{

}

void CMath_Manager::Clear()
{

}

void CMath_Manager::Start()
{

}

void CMath_Manager::Render()
{

}

_float2 CMath_Manager::Lissajous_Curve(IN _float _fTimeDelta, IN OUT _float& _fLissajousTime, IN _float _fWitth, IN _float _fHeight, IN _float _fLagrangianX, IN _float _fLagrangianY, IN _float _fPhaseDelta, IN _float _fLissajousSpeed)
{
	_fLissajousTime += _fTimeDelta * _fLissajousSpeed;

	_float _fPosX = _fWitth * sin(_fLagrangianX * _fLissajousTime + 3.14f / _fPhaseDelta);
	_float _fPosY = _fHeight * cos(_fLagrangianY * _fLissajousTime);

	return { _fPosX,_fPosY };
}

_float CMath_Manager::Random_Float(_float _fMin, _float _fMax)
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_real_distribution<_float> dis(_fMin, _fMax);

	return dis(gen);
}

_float2 CMath_Manager::Random_Float2(_float2 _f2Min, _float2 _f2Max)
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_real_distribution<_float> disX(_f2Min.x, _f2Max.x);
	std::uniform_real_distribution<_float> disY(_f2Min.y, _f2Max.y);

	return { disX(gen),disY(gen) };
}

_float3 CMath_Manager::Random_Float3(_float3 _f3Min, _float3 _f3Max)
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_real_distribution<_float> disX(_f3Min.x, _f3Max.x);
	std::uniform_real_distribution<_float> disY(_f3Min.y, _f3Max.y);
	std::uniform_real_distribution<_float> disZ(_f3Min.z, _f3Max.z);

	return { disX(gen),disY(gen),disZ(gen) };
}

_int CMath_Manager::Random_Int(_int _iMin, _int _iMax)
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_int_distribution<_int> dis(_iMin, _iMax);

	return dis(gen);
}

_int CMath_Manager::Random_Binary()
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_int_distribution<_int> dis(0, 1);

	return dis(gen);
}

_bool CMath_Manager::Random_Bool()
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_int_distribution<_int> dis(0, 1);

	if (dis(gen) == 0)
		return false;
	else
		return true;

}

_float3 CMath_Manager::Vector3x3_BezierCurve(IN _float3 _vStart, IN _float3 _vControl, IN _float3 _vEnd, IN OUT _float& _fTimeControl, IN _float _fTimeDelta, IN _float _fSpeed)
{
	_float3 vResult;
	_float fDetail = _fTimeControl;

	vResult.x = _vEnd.x * BezierB1(fDetail) + _vControl.x * BezierB2(fDetail) + _vStart.x * BezierB3(fDetail);
	vResult.y = _vEnd.y * BezierB1(fDetail) + _vControl.y * BezierB2(fDetail) + _vStart.y * BezierB3(fDetail);
	vResult.z = _vEnd.z * BezierB1(fDetail) + _vControl.z * BezierB2(fDetail) + _vStart.z * BezierB3(fDetail);

	_fTimeControl += _fTimeDelta * _fSpeed;

	return vResult;
}

_vector CMath_Manager::XMQuaternionRotationBetweenVectors(XMVECTOR start, XMVECTOR end)
{
	start = XMVector3Normalize(start);
	end = XMVector3Normalize(end);

	float cosTheta = XMVectorGetX(XMVector3Dot(start, end));
	XMVECTOR rotationAxis;

	if (cosTheta < -1.0f + 1e-6f) {
		// 180도 회전, 임의의 축을 선택해야 함
		rotationAxis = XMVector3Cross(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), start);
		if (XMVectorGetX(XMVector3LengthSq(rotationAxis)) < 1e-6f) {
			// start와 z축이 거의 평행한 경우, x축 사용
			rotationAxis = XMVector3Cross(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), start);
		}
		rotationAxis = XMVector3Normalize(rotationAxis);
		return XMQuaternionRotationAxis(rotationAxis, XM_PI);
	}

	rotationAxis = XMVector3Cross(start, end);

	float s = sqrt((1.0f + cosTheta) * 2.0f);
	float invs = 1.0f / s;

	return XMVectorSet(rotationAxis.m128_f32[0] * invs, rotationAxis.m128_f32[1] * invs, rotationAxis.m128_f32[2] * invs, s * 0.5f);
}

void CMath_Manager::QuaternionToEulerAngles(const XMVECTOR& quaternion, float& yaw, float& pitch, float& roll)
{
	// Quaternion을 오일러 각으로 변환

	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

	// 행렬에서 오일러 각도를 계산 (왼손 좌표계)
	yaw = std::atan2(rotationMatrix.r[2].m128_f32[1], rotationMatrix.r[2].m128_f32[2]);
	pitch = std::atan2(-rotationMatrix.r[2].m128_f32[0], std::sqrt(rotationMatrix.r[2].m128_f32[1] * rotationMatrix.r[2].m128_f32[1] + rotationMatrix.r[2].m128_f32[2] * rotationMatrix.r[2].m128_f32[2]));
	roll = std::atan2(rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
}

_float CMath_Manager::BezierB1(_float t)
{
	return t * t;
}

_float CMath_Manager::BezierB2(_float t)
{
	return 2 * t * (1 - t);
}

_float CMath_Manager::BezierB3(_float t)
{
	return (1 - t) * (1 - t);
}

PxVec3 CMath_Manager::Add_PxVec3toXMVECTOR(PxVec3 _vec3, _vector _XMVEC)
{
	PxVec3 tempVec3;
	XMFLOAT3 tempXMFloat3;
	XMStoreFloat3(&tempXMFloat3, _XMVEC);

	tempVec3 = _vec3;

	tempVec3.x += tempXMFloat3.x;
	tempVec3.y += tempXMFloat3.y;
	tempVec3.z += tempXMFloat3.z;

	return tempVec3;
}

shared_ptr<CMath_Manager> CMath_Manager::Create()
{
	MAKE_SHARED_ENABLER(CMath_Manager);
	shared_ptr<CMath_Manager> pInstance = make_shared<MakeSharedEnabler>();

	pInstance->Initialize();

	return pInstance;
}

void CMath_Manager::Free()
{
	
}
