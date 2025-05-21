#include "CUi_StoreSkill.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_StoreBar.h"


wstring CUi_StoreSkill::ObjID = L"CUi_StoreSkill";

CUi_StoreSkill::CUi_StoreSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreSkill::CUi_StoreSkill(const CUi_2D& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreSkill::~CUi_StoreSkill()
{
}

HRESULT CUi_StoreSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUi_StoreSkill::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	Initialize_Skill();
	Initialize_SkillMap();

	return S_OK;
}

void CUi_StoreSkill::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreSkill::Tick(_float fTimeDelta)
{
	Progress_Store(fTimeDelta);
	Adjust_Alpha(fTimeDelta);
	//if (PosY - ScaleY * 0.5f < -360 + m_fOriginSkillY)
	//{
	//	for (auto& iter : m_StoreBarVec)
	//		iter->Move(MoveDir::Up, fTimeDelta);
	//}
	//else if (PosY + ScaleY * 0.5f > m_fOriginSkillY)
	//{
	//	for (auto& iter : m_StoreBarVec)
	//		iter->Move(MoveDir::Down, fTimeDelta);
	//}

	wstring SkillName = m_StoreBarVec[m_iFocusIndex]->Get_SkillName();
	m_StoreDesc->Set_Desc((m_SkillMap.find(SkillName)->second));
}

void CUi_StoreSkill::LateTick(_float fTimeDelta)
{
}

void CUi_StoreSkill::Initialize_Skill()
{
	CUi_StoreBar::Desc Desc;
	Desc.fSizeX = 450;
	Desc.fSizeY = 50;
	Desc.fSpeed = 300;
	Desc.fX = -Desc.fSizeX * 0.5f;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Price = 17500;
	Desc.StringFont = L"픽 미 업!";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fSizeX = 450;
	Desc.fSizeY = 50;
	Desc.fSpeed = 300;
	Desc.fX = -Desc.fSizeX * 0.5f;
	Desc.fY = 200 - _float(m_StoreBarVec.size() * 60);
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Price = 25000;
	Desc.StringFont = L"오버드라이브 슬래시";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));
}

void CUi_StoreSkill::Initialize_SkillMap()
{
	CUi_StoreDesc::SkillDesc Desc;
	Desc.PriceFont = to_wstring(17500);
	Desc.TitleFont = L"픽 미 업!";
	Desc.ExplainFont = L"차이는 전투 도중에 궁지에 몰리면 예비 에너지 드링크를 꺼내 자신의&체력을 회복합니다. 에너지 드링크를 마실 때 비트 히트를 발동시키면 체력&회복량이 증가합니다!";
	Desc.iEnterCount = 2;
	m_SkillMap.emplace(L"픽 미 업!", Desc);

	Desc.PriceFont = to_wstring(25000);
	Desc.TitleFont = L"오버드라이브 슬래시";
	Desc.ExplainFont = L"차이가 리듬에 맞춰 에너지를 충전한 다음 여러 차례 참격을 날립니다.&비트 히트로 공격을 마무리하면 전방에 참격을 가해 고통에 찬 비명을&만들어 내죠! 에너지를 충분히 충전해야 최대 피해를 줄 수 있으니&주의하세요!";
	Desc.iEnterCount = 3;
	m_SkillMap.emplace(L"오버드라이브 슬래시", Desc);

	m_StoreBarVec[0]->Set_Focus(true);
}

shared_ptr<CUi_StoreSkill> CUi_StoreSkill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreSkill
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreSkill(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreSkill> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreSkill::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreSkill
	{
		MakeSharedEnabler(const CUi_StoreSkill& rhs) : CUi_StoreSkill(rhs) { }
	};

	shared_ptr<CUi_StoreSkill> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
