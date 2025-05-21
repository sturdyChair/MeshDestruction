#include "CUi_StoreChipEquip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_StoreChipEquip::ObjID = L"CUi_StoreChipEquip";

CUi_StoreChipEquip::CUi_StoreChipEquip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreChipEquip::CUi_StoreChipEquip(const CUi_2D& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreChipEquip::~CUi_StoreChipEquip()
{
}

HRESULT CUi_StoreChipEquip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreChipEquip::Initialize(void* pArg)
{
	Initialize_Transform();

	return S_OK;
}

void CUi_StoreChipEquip::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreChipEquip::Tick(_float fTimeDelta)
{
}

void CUi_StoreChipEquip::LateTick(_float fTimeDelta)
{
}

shared_ptr<CUi_StoreChipEquip> CUi_StoreChipEquip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreChipEquip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreChipEquip(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreChipEquip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreChipEquip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreChipEquip
	{
		MakeSharedEnabler(const CUi_StoreChipEquip& rhs) : CUi_StoreChipEquip(rhs) { }
	};

	shared_ptr<CUi_StoreChipEquip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
