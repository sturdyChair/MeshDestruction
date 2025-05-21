#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB : public CBounding
{
public:
	typedef struct boundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CBounding_AABB();

public:
	const BoundingBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eColliderType, shared_ptr<CBounding> pTargetBounding) override;
	virtual _bool RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) override;


	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

private:
	BoundingBox* m_pBoundingDesc_Original = { nullptr };
	BoundingBox* m_pBoundingDesc = { nullptr };

private:
	_bool Intersect_ToAABB(shared_ptr<CBounding_AABB> pTargetBounding);
	_float3 Compute_Min() const;
	_float3 Compute_Max() const;

public:
	static shared_ptr<CBounding_AABB> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	void Free();
};

END