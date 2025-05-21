#pragma once
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	virtual ~CBounding();

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(CCollider::TYPE eColliderType, shared_ptr<CBounding> pTargetBounding) PURE;
	virtual _bool RayIntersect(FXMVECTOR Origin, FXMVECTOR Direction, float& Dist) PURE;

public:
	_bool GetIsCollision() const { return m_isCollision; }
	_bool GetIsRayCast() const { return m_isRayCast; }


	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);


protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_bool						m_isCollision = { false };
	_bool						m_isRayCast = { false };

public:
	void Free();
};

END