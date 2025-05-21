#include "CUi_PlayerBar.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

#include "Beat_Manager.h"
#include "CUi_PlayerGuiter.h"
#include "CUi_PlayerHP.h"
#include "CUi_PlayerReverb.h"
#include "CUi_PlayerPartner.h"


wstring CUi_PlayerBar::ObjID = L"CUi_PlayerBar";

CUi_PlayerBar::CUi_PlayerBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_PlayerBar::CUi_PlayerBar(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_PlayerBar::~CUi_PlayerBar()
{
}

HRESULT CUi_PlayerBar::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_PlayerGuiter::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_PlayerHP::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_PlayerReverb::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_PlayerPartner::ObjID);

	return S_OK;
}

HRESULT CUi_PlayerBar::Initialize(void* pArg)
{
	Initialize_Transform();

	Create_PlayerReverb();
	Create_PlayerHP();
	Create_PlayerPartner();
	Create_PlayerGuiter();

	return S_OK;
}

void CUi_PlayerBar::PriorityTick(_float fTimeDelta)
{
}

void CUi_PlayerBar::Tick(_float fTimeDelta)
{
	Move_Shadow(fTimeDelta);
}

void CUi_PlayerBar::LateTick(_float fTimeDelta)
{

}

HRESULT CUi_PlayerBar::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		assert(false);

	return S_OK;
}

void CUi_PlayerBar::Create_PlayerGuiter()
{
	CUi_PlayerGuiter::Desc GuiterDesc{};
	GuiterDesc.fX = -550.f;
	GuiterDesc.fY = 300.f;
	GuiterDesc.fZ = 0.7f;
	GuiterDesc.fSizeX = 125.f;
	GuiterDesc.fSizeY = 125.f;
	GuiterDesc.fSizeZ = 1.f;
	GuiterDesc.fSpeed = 0.f;
	GuiterDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiter";
	GuiterDesc.TextureLevel = LEVEL_STATIC;
	m_pGuiter = static_pointer_cast<CUi_PlayerGuiter>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerGuiter::ObjID, &GuiterDesc));

	CUi_PlayerGuiter::Desc GuiterBackDesc{};
	GuiterBackDesc.fX = -550.f;
	GuiterBackDesc.fY = 300.f;
	GuiterBackDesc.fZ = 0.71f;
	GuiterBackDesc.fSizeX = 125.f;
	GuiterBackDesc.fSizeY = 125.f;
	GuiterBackDesc.fSizeZ = 1.f;
	GuiterBackDesc.fSpeed = 0.f;
	GuiterBackDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterBackDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiterBackground";
	GuiterBackDesc.TextureLevel = LEVEL_STATIC;
	m_pGuiterBackground = static_pointer_cast<CUi_PlayerGuiter>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerGuiter::ObjID, &GuiterBackDesc));

	CUi_PlayerGuiter::Desc GuiterShadowDesc{};
	GuiterShadowDesc.fX = -550.f;
	GuiterShadowDesc.fY = 296.f;
	GuiterShadowDesc.fZ = 0.72f;
	GuiterShadowDesc.fSizeX = 125.f;
	GuiterShadowDesc.fSizeY = 125.f;
	GuiterShadowDesc.fSizeZ = 1.f;
	GuiterShadowDesc.fSpeed = 0.f;
	GuiterShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	GuiterShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerGuiterShadow";
	GuiterShadowDesc.TextureLevel = LEVEL_STATIC;
	m_pGuiterShadow = static_pointer_cast<CUi_PlayerGuiter>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerGuiter::ObjID, &GuiterShadowDesc));


	CUi_PlayerGuiter::Desc LockDesc{};
	LockDesc.fX = -553.f;
	LockDesc.fY = 302.f;
	LockDesc.fZ = 0.69f;
	LockDesc.fSizeX = 45.f;
	LockDesc.fSizeY = 45.f;
	LockDesc.fSizeZ = 1.f;
	LockDesc.fSpeed = 0.f;
	LockDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	LockDesc.TextureFileName = L"Prototype_Component_Texture_PlayerBarLock";
	LockDesc.TextureLevel = LEVEL_STATIC;
	m_pLock = static_pointer_cast<CUi_PlayerGuiter>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerGuiter::ObjID, &LockDesc));
}

void CUi_PlayerBar::Create_PlayerHP()
{
	CUi_PlayerHP::Desc  HPDesc;
	HPDesc.fX = -475;
	HPDesc.fY = 300;
	HPDesc.fZ = 0.7f;
	HPDesc.fSizeX = 250;
	HPDesc.fSizeY = 300;
	HPDesc.fSizeZ = 1.f;
	HPDesc.fSpeed = 0.f;
	HPDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPGauge";
	HPDesc.TextureLevel = LEVEL_STATIC;
	m_pHP = static_pointer_cast<CUi_PlayerHP>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerHP::ObjID, &HPDesc));
	m_pHP->Rotation(XMConvertToRadians(6.5f));

	CUi_PlayerHP::Desc HPBarDesc{};
	HPBarDesc.fX = -475.f;
	HPBarDesc.fY = 300.f;
	HPBarDesc.fZ = 0.71f;
	HPBarDesc.fSizeX = 250.f;
	HPBarDesc.fSizeY = 300.f;
	HPBarDesc.fSizeZ = 1.f;
	HPBarDesc.fSpeed = 0.f;
	HPBarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPBarDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBar";
	HPBarDesc.TextureLevel = LEVEL_STATIC;
	m_pHPBar = static_pointer_cast<CUi_PlayerHP>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerHP::ObjID, &HPBarDesc));
	m_pHPBar->Rotation(XMConvertToRadians(6.5f));

	CUi_PlayerHP::Desc HPShadowDesc{};
	HPShadowDesc.fX = -475.f;
	HPShadowDesc.fY = 296.f;
	HPShadowDesc.fZ = 0.72f;
	HPShadowDesc.fSizeX = 250.f;
	HPShadowDesc.fSizeY = 300.f;
	HPShadowDesc.fSizeZ = 1.f;
	HPShadowDesc.fSpeed = 0.f;
	HPShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	HPShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerHPBarShadow";
	HPShadowDesc.TextureLevel = LEVEL_STATIC;
	m_pHPShadow = static_pointer_cast<CUi_PlayerHP>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerHP::ObjID, &HPShadowDesc));
	m_pHPShadow->Rotation(XMConvertToRadians(6.5f));
}

void CUi_PlayerBar::Create_PlayerReverb()
{
	CUi_PlayerReverb::Desc  ReverbDesc;
	ReverbDesc.fX = -495.f;
	ReverbDesc.fY = 321.f;
	ReverbDesc.fZ = 0.7f;
	ReverbDesc.fSizeX = 125.f;
	ReverbDesc.fSizeY = 142.f;
	ReverbDesc.fSizeZ = 1.f;
	ReverbDesc.fSpeed = 0.f;
	ReverbDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbGauge";
	ReverbDesc.TextureLevel = LEVEL_STATIC;
	ReverbDesc.iTextureNum = 0;
	m_pReverb1 = static_pointer_cast<CUi_PlayerReverb>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc));
	m_pReverb1->Rotation(XMConvertToRadians(7.4f));

	ReverbDesc.fX = -431.f;
	ReverbDesc.fY = 324.f;
	ReverbDesc.fSizeX = 125.f;
	ReverbDesc.fSizeY = 140.f;
	ReverbDesc.iTextureNum = 1;
	m_pReverb2 = static_pointer_cast<CUi_PlayerReverb>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerReverb::ObjID, &ReverbDesc));
	m_pReverb2->Rotation(XMConvertToRadians(7.4f));

	CUi_PlayerReverb::Desc ReverbBarDesc{};
	ReverbBarDesc.fX = -495.f;
	ReverbBarDesc.fY = 321.f;
	ReverbBarDesc.fZ = 0.71f;
	ReverbBarDesc.fSizeX = 125.f;
	ReverbBarDesc.fSizeY = 142.f;
	ReverbBarDesc.fSizeZ = 1.f;
	ReverbBarDesc.fSpeed = 0.f;
	ReverbBarDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbBarDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbBar";
	ReverbBarDesc.TextureLevel = LEVEL_STATIC;
	ReverbBarDesc.iTextureNum = 0;
	m_pReverbBar1 = static_pointer_cast<CUi_PlayerReverb>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerReverb::ObjID, &ReverbBarDesc));
	m_pReverbBar1->Rotation(XMConvertToRadians(7.4f));

	ReverbBarDesc.fX = -431.f;
	ReverbBarDesc.fY = 324.f;
	ReverbBarDesc.fSizeX = 125.f;
	ReverbBarDesc.fSizeY = 140.f;
	ReverbBarDesc.iTextureNum = 1;
	m_pReverbBar2 = static_pointer_cast<CUi_PlayerReverb>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerReverb::ObjID, &ReverbBarDesc));
	m_pReverbBar2->Rotation(XMConvertToRadians(7.4f));

	CUi_PlayerReverb::Desc  ReverbShadowDesc;
	ReverbShadowDesc.fX = -462.f;
	ReverbShadowDesc.fY = 308.f;
	ReverbShadowDesc.fZ = 0.72f;
	ReverbShadowDesc.fSizeX = 250.f;
	ReverbShadowDesc.fSizeY = 250.f;
	ReverbShadowDesc.fSizeZ = 1.f;
	ReverbShadowDesc.fSpeed = 0.f;
	ReverbShadowDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	ReverbShadowDesc.TextureFileName = L"Prototype_Component_Texture_PlayerReverbShadow";
	ReverbShadowDesc.TextureLevel = LEVEL_STATIC;
	ReverbShadowDesc.iTextureNum = 0;
	m_pReverbShadow = static_pointer_cast<CUi_PlayerReverb>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerReverb::ObjID, &ReverbShadowDesc));
	m_pReverbShadow->Rotation(XMConvertToRadians(7.4f));

}

void CUi_PlayerBar::Create_PlayerPartner()
{
	CUi_PlayerPartner::Desc PepperDesc{};
	PepperDesc.fX = -528.f;
	PepperDesc.fY = 235.f;
	PepperDesc.fZ = 0.7f;
	PepperDesc.fSizeX = 200.f;
	PepperDesc.fSizeY = 200.f;
	PepperDesc.fSizeZ = 1.f;
	PepperDesc.fSpeed = 0.f;
	PepperDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	PepperDesc.TextureFileName = L"Prototype_Component_Texture_PlayerPartnerPeppermint";
	PepperDesc.TextureLevel = LEVEL_STATIC;
	PepperDesc.iTextureNum = 0;
	m_pPartnerPeppermint = static_pointer_cast<CUi_PlayerPartner>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_PlayerPartner::ObjID, &PepperDesc));
}

void CUi_PlayerBar::Move_Shadow(_float fTimeDelta)
{
	m_fShadowCurTime += fTimeDelta;

	if (m_fShadowCurTime >= 0.45f)
	{
		m_fShadowCurTime = 0.0f;
		m_bReturning = false;
	}

	_float halfCycleTime = 0.5f / 3.0f;
	_float returnCycleTime = 1.f / 3.0f;

	if (m_fShadowCurTime < halfCycleTime)
	{
		_float progress = m_fShadowCurTime / halfCycleTime;
		m_pGuiterShadow->Set_Pos(-550.f + progress * 4.5f, 296.f - progress * 9.f, 0.72f);
		m_pHPShadow->Set_Pos(-475.f + progress * 4.5f, 296.f - progress * 9.f, 0.72f);
		m_pReverbShadow->Set_Pos(-462.f + progress * 4.5f, 308.f - progress * 9.f, 0.72f);
	}
	else
	{
		_float progress = (m_fShadowCurTime - halfCycleTime) / returnCycleTime;
		m_pGuiterShadow->Set_Pos(-550.f + 4.5f * (1.0f - progress), 296.f - 9.f * (1.0f - progress), 0.72f);
		m_pHPShadow->Set_Pos(-475.f + 4.5f * (1.0f - progress), 296.f - 9.f * (1.0f - progress), 0.72f);
		m_pReverbShadow->Set_Pos(-462.f + 4.5f * (1.0f - progress), 308.f - 9.f * (1.0f - progress), 0.72f);
	}
}

shared_ptr<CUi_PlayerBar> CUi_PlayerBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_PlayerBar
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_PlayerBar(pDevice, pContext) { }
	};

	shared_ptr<CUi_PlayerBar> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_PlayerBar::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_PlayerBar
	{
		MakeSharedEnabler(const CUi_PlayerBar& rhs) : CUi_PlayerBar(rhs) { }
	};

	shared_ptr<CUi_PlayerBar> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
