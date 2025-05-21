#include "CUi_TalkCharacter.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_TalkCharacter::ObjID = L"CUi_TalkCharacter";

CUi_TalkCharacter::CUi_TalkCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_TalkSystem(pDevice, pContext)
{
}

CUi_TalkCharacter::CUi_TalkCharacter(const CUi_TalkSystem& rhs)
	:CUi_TalkSystem(rhs)
{
}

CUi_TalkCharacter::~CUi_TalkCharacter()
{
}

HRESULT CUi_TalkCharacter::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_TalkRobotA", L"../Bin/Resources/Textures/Ui/Talk/Robot/T_talk_robotA_normal.png")
	);

	return S_OK;
}

HRESULT CUi_TalkCharacter::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_iTextureNum = Arg->iTextureNum;

	return S_OK;
}

void CUi_TalkCharacter::PriorityTick(_float fTimeDelta)
{

}

void CUi_TalkCharacter::Tick(_float fTimeDelta)
{
}

void CUi_TalkCharacter::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}

shared_ptr<CUi_TalkCharacter> CUi_TalkCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TalkCharacter
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TalkCharacter(pDevice, pContext) { }
	};

	shared_ptr<CUi_TalkCharacter> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TalkCharacter::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TalkCharacter
	{
		MakeSharedEnabler(const CUi_TalkCharacter& rhs) : CUi_TalkCharacter(rhs) { }
	};

	shared_ptr<CUi_TalkCharacter> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
