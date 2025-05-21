#include "DoorTrigger.h"
#include "GameInstance.h"
#include "Collider.h"

CDoorTrigger::CDoorTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger{pDevice, pContext}
{
}

CDoorTrigger::CDoorTrigger(const CDoorTrigger& rhs)
	: CTrigger{rhs}
{
}

CDoorTrigger::~CDoorTrigger()
{
	Free();
}

HRESULT CDoorTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoorTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	DOORTRIGGER_DESC* pDesc = static_cast<DOORTRIGGER_DESC*>(pArg);
	m_pDoorObject = pDesc->pDoorObject;

	return S_OK;
}

void CDoorTrigger::PriorityTick(_float fTimeDelta)
{
}

void CDoorTrigger::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDoorTrigger::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

#ifdef _DEBUG
HRESULT CDoorTrigger::Render()
{
	return __super::Render();
}
#endif

void CDoorTrigger::Collision_Enter(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CDoorTrigger::Collision(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

void CDoorTrigger::Collision_Exit(shared_ptr<class CCollider> pOther, shared_ptr<class CCollider> pCaller)
{
}

shared_ptr<CDoorTrigger> CDoorTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CDoorTrigger
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CDoorTrigger(pDevice, pContext) { }
	};

	shared_ptr<CDoorTrigger> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDoorTrigger"));
		assert(false);
	}

	return pInstance;
}

shared_ptr<CGameObject> CDoorTrigger::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CDoorTrigger
	{
		MakeSharedEnabler(const CDoorTrigger& rhs) : CDoorTrigger(rhs) { }
	};

	shared_ptr<CDoorTrigger> pInstance = make_shared<MakeSharedEnabler>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDoorTrigger"));
		assert(false);
	}

	return pInstance;
}

void CDoorTrigger::Free()
{
}
