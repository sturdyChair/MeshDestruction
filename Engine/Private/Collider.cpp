#include "..\Public\Collider.h"
#include "GameInstance.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

_uint CCollider::s_iNumCollider = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_iColliderID = s_iNumCollider;
	++s_iNumCollider;
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent{ rhs }
	, m_eType{ rhs.m_eType }
	, m_pBounding{ rhs.m_pBounding }
	, m_pBatch{ rhs.m_pBatch }
	, m_pEffect{ rhs.m_pEffect }
	, m_pInputLayout{ rhs.m_pInputLayout }
{
	m_iColliderID = s_iNumCollider;
	++s_iNumCollider;
	Safe_AddRef(m_pInputLayout);

}

CCollider::~CCollider()
{
	Free();
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;


	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;



	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	}

	return S_OK;
}
void CCollider::Update(_fmatrix WorldMatrix)
{
	m_pBounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(shared_ptr<CCollider> pTargetCollider)
{
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

_bool CCollider::RayIntersect(XMVECTOR vRayPos, XMVECTOR vRayDir, _float& Dist)
{
	return m_pBounding->RayIntersect(vRayPos, vRayDir, Dist);
}

void CCollider::Collision_Enter(shared_ptr<CCollider> pTargetCollider)
{

}


HRESULT CCollider::Render()
{
	if (/*GAMEINSTANCE->Get_DebugMode()*/true) {
		m_pContext->GSSetShader(nullptr, nullptr, 0);

		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance.lock()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

		m_pContext->IASetInputLayout(m_pInputLayout);
		m_pEffect->Apply(m_pContext);

		m_pBatch->Begin();

		m_pBounding->Render(m_pBatch);

		m_pBatch->End();
	}
	return S_OK;

}

_bool CCollider::IsCollided()
{
	return m_pBounding->GetIsCollision();
}

_bool CCollider::IsPicked()
{
	return m_pBounding->GetIsRayCast();
}


shared_ptr<CCollider> CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	struct MakeSharedEnabler : public CCollider
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCollider(pDevice,pContext)
		{}
	};


	shared_ptr<CCollider> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider"));
		assert(false);
		//Safe_Release(pInstance);
	}

	return pInstance;
}


shared_ptr<CComponent> CCollider::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CCollider
	{
		MakeSharedEnabler(const CCollider& rhs) : CCollider(rhs)
		{}
	};

	shared_ptr<CCollider> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollider"));
		assert(false);
	}

	return pInstance;
}

void CCollider::Free()
{
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
}
