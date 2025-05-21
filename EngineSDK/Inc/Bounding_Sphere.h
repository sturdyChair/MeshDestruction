#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere : public CBounding
{
public:
	typedef struct sphere_desc : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	}BOUNDING_SPHERE_DESC;
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CBounding_Sphere();

public:
	const BoundingSphere* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eColliderType, shared_ptr<CBounding> pTargetBounding) override;
	virtual _bool RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) override;


	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

private:
	BoundingSphere* m_pBoundingDesc_Original = { nullptr };
	BoundingSphere* m_pBoundingDesc = { nullptr };

public:
	static shared_ptr<CBounding_Sphere> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	void Free();
};

END