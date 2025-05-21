#include "CUi_BeatJudge.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_BeatJudge::ObjID = L"CUi_BeatJudge";

CUi_BeatJudge::CUi_BeatJudge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_BeatJudge::CUi_BeatJudge(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_BeatJudge::~CUi_BeatJudge()
{
}

HRESULT CUi_BeatJudge::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
	make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatJudge",
		L"../Bin/Resources/Textures/Ui/BeatJudge/BeatJudge%d.png", 3));


	return S_OK;
}

HRESULT CUi_BeatJudge::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	switch (Arg->Type)
	{
	case CUi_BeatMarkerSystem::JudgeType::Perfect:
		Set_TextureNum(0);
		break;
	case CUi_BeatMarkerSystem::JudgeType::Good:
		Set_TextureNum(1);
		break;
	case CUi_BeatMarkerSystem::JudgeType::Miss:
		Set_TextureNum(2);
		break;
	default:
		assert(false);
		break;
	}

	return S_OK;
}

void CUi_BeatJudge::PriorityTick(_float fTimeDelta)
{
}

void CUi_BeatJudge::Tick(_float fTimeDelta)
{
	Move(MoveDir::Up, fTimeDelta);
	Add_Color(ColorType::A, -fTimeDelta * 2);
	if (m_fColor.w < 0)
		Set_Dead();
}

void CUi_BeatJudge::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_BeatJudge> CUi_BeatJudge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_BeatJudge
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_BeatJudge(pDevice, pContext) { }
	};

	shared_ptr<CUi_BeatJudge> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_BeatJudge::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_BeatJudge
	{
		MakeSharedEnabler(const CUi_BeatJudge& rhs) : CUi_BeatJudge(rhs) { }
	};

	shared_ptr<CUi_BeatJudge> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
