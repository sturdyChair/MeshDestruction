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
	Desc.StringFont = L"에코 스플래시";

	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 10);
	Desc.Price = 6000;
	Desc.StringFont = L"튠 업";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 20);
	Desc.Price = 10000;
	Desc.StringFont = L"스톰프 박스";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 30);
	Desc.Price = 8000;
	Desc.StringFont = L"픽업 크래시";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));


	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 40);
	Desc.Price = 10000;
	Desc.StringFont = L"하모닉 빔";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	Desc.fY = 70 - _float(m_StoreBarVec.size() * 60 + 50);
	Desc.Price = 16000;
	Desc.StringFont = L"컴프레서 슬램";
	m_StoreBarVec.emplace_back(static_pointer_cast<CUi_StoreBar>
		(m_pGameInstance.lock()->Add_Clone_Return(m_iLevelIndex, L"CUi", CUi_StoreBar::ObjID, &Desc)));

	m_StoreBarVec[0]->Set_Focus(true);
	Initialize_AttackBack();
}

void CUi_StoreAttack::Initialize_AttackMap()
{
	CUi_StoreDesc::AttackDesc Desc;
	Desc.PriceFont = to_wstring(8000);
	Desc.TitleFont = L"에코 스플래시";
	Desc.ExplainFont = L"공중 콤보로 이어지는 지상 콤보가 있다고요? &말도 안 돼!&&그게 바로 에코 스플래시입니다. 비트 히트를 적중시켜 &차이 전방에 있는 적들을 공중으로 띄운 다음, &대시나 자석으로 적에게 다시 접근해서 공중콤보로 연계하세요.&&믿고 쓰는 기술이죠!";
	Desc.iEnterCount = 7;
	m_AttackMap.emplace(L"에코 스플래시", Desc);

	Desc.PriceFont = to_wstring(6000);
	Desc.TitleFont = L"튠 업";
	Desc.ExplainFont = L"&&차이 앞에 있는 적을 직접 공격한 다음, 광범위 공격으로 다수의 적을&가격해 날려 보내는 레스트 콤보입니다. 높은 점수를 획득하면서 가까이에&있는 적과 거리를 벌리고 싶을 때, 다른 기술들과 혼용하기 좋습니다.";
	Desc.iEnterCount = 4;
	m_AttackMap.emplace(L"튠 업", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"스톰프 박스";
	Desc.ExplainFont = L"공중으로 점프해 비트 히트를 적중시켜 360도 범위 공격을 수행합니다.&이때 약한 적들은 뒤로 날려 보낼 수 있습니다. 적에게 둘러싸였을 때&사용하기 좋은 공격입니다!";
	Desc.iEnterCount = 2;
	m_AttackMap.emplace(L"스톰프 박스", Desc);

	Desc.PriceFont = to_wstring(8000);
	Desc.TitleFont = L"픽업 크래시";
	Desc.ExplainFont = L"차이가 강공격으로 에너지를 충전해, 쓰레기...로 기타를 강화합니다.&그다음 강력한 참격으로 적들을 쓰레기와 함께 뒤로 날려 보냅니다. 뒤이어&자성 그랩을 사용하면,콤보를 이어 가고 마그넷 링크 보너스를 획득할 수&있습니다.";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"픽업 크래시", Desc);

	Desc.PriceFont = to_wstring(10000);
	Desc.TitleFont = L"하모닉 빔";
	Desc.ExplainFont = L"고급 공중 콤보 공격입니다. 약공격과 강공격을 연계해 공중에 있는&적에게 피해를 줄 뿐만 아니라, 적을 땅으로 내려보낸 다음 차이가&공중에서 강력한 빔을 발사해 적을 끝장냅니다. 빔은 주변에 있는 적에게도&피해를 줍니다!";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"하모닉 빔", Desc);

	Desc.PriceFont = to_wstring(16000);
	Desc.TitleFont = L"컴프레서 슬램";
	Desc.ExplainFont = L"여러 번 공격한 뒤 마지막에는 적을 땅에 내리꽂는 공중 콤보입니다.&&까다로운 콤보 중 하나입니다. 처음 두 번의 공격을 수행한 다음,&차이가 회전 공격을 마치면 콤보의 나머지 절반을 입력하세요!";
	Desc.iEnterCount = 3;
	m_AttackMap.emplace(L"컴프레서 슬램", Desc);
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
