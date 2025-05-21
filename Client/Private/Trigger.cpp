#include "Trigger.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_OBB.h"

wstring CTrigger::ObjID = TEXT("CTrigger");

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CGameObject{rhs}
{
}

CTrigger::~CTrigger()
{
    Free();
}

HRESULT CTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC obbDesc{};
	obbDesc.vCenter = _float3{ 0.f, 0.f, 0.f };
	obbDesc.vExtents = _float3{ 1.f, 1.f, 1.f };
	obbDesc.vRotation = _float3{ 0.f, 0.f, 0.f };

	m_pColliderCom = static_pointer_cast<CCollider>(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), &obbDesc));

	if (!m_pColliderCom)
		assert(false);

    return S_OK;
}

void CTrigger::PriorityTick(_float fTimeDelta)
{
}

void CTrigger::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_XMMat());
}

void CTrigger::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_NONBLEND, shared_from_this());
#endif
}

#ifdef _DEBUG
HRESULT CTrigger::Render()
{
	return m_pColliderCom->Render();
}
#endif

void CTrigger::SetUp_WorldMatrix(_fmatrix ParentMatrix)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_originMat) * ParentMatrix);
}

shared_ptr<CTrigger> CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CTrigger
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CTrigger(pDevice, pContext) { }
	};

	shared_ptr<CTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTrigger"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CTrigger::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CTrigger
	{
		MakeSharedEnabler(const CTrigger& rhs) : CTrigger(rhs) { }
	};

	shared_ptr<CTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTrigger"));
		assert(false);
	}

	return pInstance;
}

void CTrigger::Free()
{
}
