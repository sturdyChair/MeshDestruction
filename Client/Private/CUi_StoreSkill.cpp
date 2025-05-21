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
	Desc.StringFont = L"�� �� ��!";
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
	Desc.StringFont = L"��������̺� ������";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));
}

void CUi_StoreSkill::Initialize_SkillMap()
{
	CUi_StoreDesc::SkillDesc Desc;
	Desc.PriceFont = to_wstring(17500);
	Desc.TitleFont = L"�� �� ��!";
	Desc.ExplainFont = L"���̴� ���� ���߿� ������ ������ ���� ������ �帵ũ�� ���� �ڽ���&ü���� ȸ���մϴ�. ������ �帵ũ�� ���� �� ��Ʈ ��Ʈ�� �ߵ���Ű�� ü��&ȸ������ �����մϴ�!";
	Desc.iEnterCount = 2;
	m_SkillMap.emplace(L"�� �� ��!", Desc);

	Desc.PriceFont = to_wstring(25000);
	Desc.TitleFont = L"��������̺� ������";
	Desc.ExplainFont = L"���̰� ���뿡 ���� �������� ������ ���� ���� ���� ������ �����ϴ�.&��Ʈ ��Ʈ�� ������ �������ϸ� ���濡 ������ ���� ���뿡 �� �����&����� ����! �������� ����� �����ؾ� �ִ� ���ظ� �� �� ������&�����ϼ���!";
	Desc.iEnterCount = 3;
	m_SkillMap.emplace(L"��������̺� ������", Desc);

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
