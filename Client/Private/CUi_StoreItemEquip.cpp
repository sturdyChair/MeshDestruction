#include "CUi_StoreItemEquip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_StoreItemEquip::ObjID = L"CUi_StoreItemEquip";

CUi_StoreItemEquip::CUi_StoreItemEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreItemEquip::CUi_StoreItemEquip(const CUi_2D& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreItemEquip::~CUi_StoreItemEquip()
{
}

HRESULT CUi_StoreItemEquip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreItemEquip::Initialize(void* pArg)
{
	Initialize_Transform();

	return S_OK;
}

void CUi_StoreItemEquip::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreItemEquip::Tick(_float fTimeDelta)
{
}

void CUi_StoreItemEquip::LateTick(_float fTimeDelta)
{
}

shared_ptr<CUi_StoreItemEquip> CUi_StoreItemEquip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreItemEquip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreItemEquip(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreItemEquip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreItemEquip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreItemEquip
	{
		MakeSharedEnabler(const CUi_StoreItemEquip& rhs) : CUi_StoreItemEquip(rhs) { }
	};

	shared_ptr<CUi_StoreItemEquip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
