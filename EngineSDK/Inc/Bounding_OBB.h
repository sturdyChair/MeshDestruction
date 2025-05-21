#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB  : public CBounding
{
public:
	typedef struct obbDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;

	typedef struct
	{
		_float3		vCenter;
		_float3		vCenterDir[3];
		_float3		vAlignDir[3];
	}OBB_DESC;
private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CBounding_OBB();

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eColliderType, shared_ptr<CBounding> pTargetBounding) override;
	virtual _bool RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) override;


	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

private:
	BoundingOrientedBox* m_pBoundingDesc_Original = { nullptr };
	BoundingOrientedBox* m_pBoundingDesc = { nullptr };

private:
	_bool Intersect_ToOBB(shared_ptr<CBounding_OBB> pTargetBounding);
	OBB_DESC Compute_OBB() const;

public:
	static shared_ptr<CBounding_OBB> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	void Free();
};

END