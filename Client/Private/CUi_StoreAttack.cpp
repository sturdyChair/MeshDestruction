#include "CUi_StoreAttack.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "CUi_StoreBar.h"
#include "CUi_Default.h"


wstring CUi_StoreAttack::ObjID = L"CUi_StoreAttack";

CUi_StoreAttack::CUi_StoreAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_Store(pDevice, pContext)
{
}

CUi_StoreAttack::CUi_StoreAttack(const CUi_2D& rhs)
	:CUi_Store(rhs)
{
}

CUi_StoreAttack::~CUi_StoreAttack()
{
}

HRESULT CUi_StoreAttack::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUi_StoreAttack::Initialize(void* pArg)
{
	Initialize_Transform();
	Initialize_StoreDesc();
	Initialize_Attack();
	Initialize_AttackBack();
	Initialize_AttackMap();

	return S_OK;
}

void CUi_StoreAttack::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreAttack::Tick(_float fTimeDelta)
{
	Progress_Store(fTimeDelta);
	Adjust_Alpha(fTimeDelta);
	wstring SkillName = m_StoreBarVec[m_iFocusIndex]->Get_SkillName();
	m_StoreDesc->Set_Desc((m_AttackMap.find(SkillName)->second));

	//_float PosY = m_StoreBarVec[m_iFocusIndex]->Get_Pos().y;
	//_float ScaleY = m_StoreBarVec[m_iFocusIndex]->Get_Scale().y;
	//if (true)
	//{
	//	if (PosY - ScaleY * 0.5f < -360 + m_fOriginAttackY)
	//	{
	//		for (auto& iter : m_StoreBarVec)
	//			iter->Move(MoveDir::Up, fTimeDelta);
	//	}
	//	else if (PosY + ScaleY * 0.5f > m_fOriginAttackY)
	//	{
	//		for (auto& iter : m_StoreBarVec)
	//			iter->Move(MoveDir::Down, fTimeDelta);
	//	}
	//}
}

void CUi_StoreAttack::LateTick(_float fTimeDelta)
{
}

void CUi_StoreAttack::Initialize_Attack()
{
	m_StoreBarVec.reserve(6);

	CUi_StoreBar::Desc Desc;
	Desc.fSizeX = 450;
	Desc.fSizeY = 50;
	Desc.fSpeed = 300;
	Desc.fX = -Desc.fSizeX * 0.5f;
	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60);
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.TextureFileName = L"Prototype_Component_Texture_StoreBar";
	Desc.TextureLevel = LEVEL_STATIC;
	Desc.Price = 8000;
	Desc.StringFont = L"���� ���÷���";

	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 10);
	Desc.Price = 6000;
	Desc.StringFont = L"ƪ ��";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 20);
	Desc.Price = 10000;
	Desc.StringFont = L"������ �ڽ�";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 30);
	Desc.Price = 8000;
	Desc.StringFont = L"�Ⱦ� ũ����";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 40);
	Desc.Price = 10000;
	Desc.StringFont = L"�ϸ�� ��";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 50);
	Desc.Price = 16000;
	Desc.StringFont = L"�������� ����";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	m_StoreBarVec[0]->Set_Focus(true);
	Initialize_AttackBack();
}

void CUi_StoreAttack::Initialize_AttackMap()
{
	CUi_StoreDesc::AttackDesc Desc;
	Desc.PriceFont = to_wstring(8000);
	Desc.TitleFont = L"���� ���÷���";
	Desc.ExplainFont = L"���� �޺��� �̾����� ���� �޺��� �ִٰ��? &���� �� ��!&&�װ� �ٷ� ���� ���÷����Դϴ�. ��Ʈ ��Ʈ�� ���߽��� &���� ���濡 �ִ� ������ �������� ��� ����, &��ó� �ڼ����� ������ �ٽ� �����ؼ� �����޺��� �����ϼ���.&&�ϰ� ���� �������!";
	Desc.iEnterCount = 7;
	m_AttackMap.emplace(L"���� ���÷���", Desc);

	Desc.PriceFont = to_wstring(6000);
	Desc.TitleFont = L"ƪ ��";
	Desc.ExplainFont = L"&&���� �տ� �ִ� ���� ���� ������ ����, ������ �������� �ټ��� ����&������ ���� ������ ����Ʈ �޺��Դϴ�. ���� ������ ȹ���ϸ鼭 �����̿�&�ִ� ���� �Ÿ��� ������ ���� ��, �ٸ� ������ ȥ���ϱ� �����ϴ�.";
	Desc.iEnterCount = 4;
	m_AttackMap.emplace(L"ƪ ��", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"������ �ڽ�";
	Desc.ExplainFont = L"�������� ������ ��Ʈ ��Ʈ�� ���߽��� 360�� ���� ������ �����մϴ�.&�̶� ���� ������ �ڷ� ���� ���� �� �ֽ��ϴ�. ������ �ѷ��ο��� ��&����ϱ� ���� �����Դϴ�!";
	Desc.iEnterCount = 2;
	m_AttackMap.emplace(L"������ �ڽ�", Desc);

	Desc.PriceFont = to_wstring(8000);
	Desc.TitleFont = L"�Ⱦ� ũ����";
	Desc.ExplainFont = L"���̰� ���������� �������� ������, ������...�� ��Ÿ�� ��ȭ�մϴ�.&�״��� ������ �������� ������ ������� �Բ� �ڷ� ���� �����ϴ�. ���̾�&�ڼ� �׷��� ����ϸ�,�޺��� �̾� ���� ���׳� ��ũ ���ʽ��� ȹ���� ��&�ֽ��ϴ�.";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"�Ⱦ� ũ����", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"�ϸ�� ��";
	Desc.ExplainFont = L"��� ���� �޺� �����Դϴ�. ����ݰ� �������� ������ ���߿� �ִ�&������ ���ظ� �� �Ӹ� �ƴ϶�, ���� ������ �������� ���� ���̰�&���߿��� ������ ���� �߻��� ���� ������ϴ�. ���� �ֺ��� �ִ� �����Ե�&���ظ� �ݴϴ�!";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"�ϸ�� ��", Desc);

	Desc.PriceFont = to_wstring(16000);
	Desc.TitleFont = L"�������� ����";
	Desc.ExplainFont = L"���� �� ������ �� ���������� ���� ���� �����ȴ� ���� �޺��Դϴ�.&&��ٷο� �޺� �� �ϳ��Դϴ�. ó�� �� ���� ������ ������ ����,&���̰� ȸ�� ������ ��ġ�� �޺��� ������ ������ �Է��ϼ���!";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"�������� ����", Desc);
}

void CUi_StoreAttack::Initialize_AttackBack()
{
	CUi_Default::Desc Desc;
	Desc.fSizeX = 400;
	Desc.fSizeY = g_iWinSizeY * 0.8f;
	Desc.fX = -_float(g_iWinSizeX * 0.5f) + Desc.fSizeX * 0.4f;
	Desc.fY = g_iWinSizeY * 0.1f;
	Desc.fZ = 0.93f;
	Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Desc.ShaderPass = ShaderPass::Blend;
	Desc.TextureFileName = L"Prototype_Component_Texture_SelectAttackStore";
	Desc.TextureLevel = LEVEL_STATIC;
	m_BackVec.emplace_back(static_pointer_cast<CUi_Default>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_Default::ObjID, &Desc)));

	for (auto& iter : m_BackVec)
		iter->Set_Active(false);
}

shared_ptr<CUi_StoreAttack> CUi_StoreAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreAttack
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreAttack(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreAttack> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreAttack::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreAttack
	{
		MakeSharedEnabler(const CUi_StoreAttack& rhs) : CUi_StoreAttack(rhs) { }
	};

	shared_ptr<CUi_StoreAttack> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
