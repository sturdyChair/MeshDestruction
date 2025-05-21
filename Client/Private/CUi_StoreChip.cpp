#include "CUi_StoreChip.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_StoreChip::ObjID = L"CUi_StoreChip";

CUi_StoreChip::CUi_StoreChip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreChip::CUi_StoreChip(const CUi_2D& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreChip::~CUi_StoreChip()
{
}

HRESULT CUi_StoreChip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreChip::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	
	return S_OK;
}

void CUi_StoreChip::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreChip::Tick(_float fTimeDelta)
{
}

void CUi_StoreChip::LateTick(_float fTimeDelta)
{
}

shared_ptr<CUi_StoreChip> CUi_StoreChip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreChip
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreChip(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreChip> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreChip::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreChip
	{
		MakeSharedEnabler(const CUi_StoreChip& rhs) : CUi_StoreChip(rhs) { }
	};

	shared_ptr<CUi_StoreChip> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
