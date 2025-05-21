#include "CUi_ComboWordHits.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"


wstring CUi_ComboWordHits::ObjID = L"CUi_ComboWordHits";

CUi_ComboWordHits::CUi_ComboWordHits(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_ComboWordHits::CUi_ComboWordHits(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_ComboWordHits::~CUi_ComboWordHits()
{
}

HRESULT CUi_ComboWordHits::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboWordHits",
			L"../Bin/Resources/Textures/Ui/Word/Hits.png"));


	return S_OK;
}

HRESULT CUi_ComboWordHits::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_OriginScale.x = Arg->fSizeX;
	m_OriginScale.y = Arg->fSizeY;

	return S_OK;
}

void CUi_ComboWordHits::PriorityTick(_float fTimeDelta)
{
}

void CUi_ComboWordHits::Tick(_float fTimeDelta)
{
	Check_Change();
	if (m_bChange)
	{
		Scaling(fTimeDelta);
	}
}

void CUi_ComboWordHits::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

void CUi_ComboWordHits::Check_Change()
{
	if (m_iOriginCombo != PLAYERMANAGER->Get_PlayerCombo())
	{
		m_iOriginCombo = PLAYERMANAGER->Get_PlayerCombo();
		m_bChange = true;
	}
}

void CUi_ComboWordHits::Scaling(_float fTimeDelta)
{
	m_fScaleTime += fTimeDelta;
	_float ChangeScaleRate = fTimeDelta * 250;
	if (m_fScaleTime < 0.15f)
	{
		Add_Scale(ChangeScaleRate, ChangeScaleRate);
	}
	else if (0.15f <= m_fScaleTime && m_fScaleTime <= 0.3f)
	{
		Add_Scale(-ChangeScaleRate, -ChangeScaleRate);
	}
	else
	{
		Set_Scale(m_OriginScale.x, m_OriginScale.y);
		m_fScaleTime = 0;
		m_bChange = false;
	}
}

shared_ptr<CUi_ComboWordHits> CUi_ComboWordHits::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_ComboWordHits
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_ComboWordHits(pDevice, pContext) { }
	};

	shared_ptr<CUi_ComboWordHits> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_ComboWordHits::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_ComboWordHits
	{
		MakeSharedEnabler(const CUi_ComboWordHits& rhs) : CUi_ComboWordHits(rhs) { }
	};

	shared_ptr<CUi_ComboWordHits> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
