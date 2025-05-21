#include "CUi_BeatMarker.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "Beat_Manager.h"


wstring CUi_BeatMarker::ObjID = L"CUi_BeatMarker";

CUi_BeatMarker::CUi_BeatMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_BeatMarker::CUi_BeatMarker(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_BeatMarker::~CUi_BeatMarker()
{
}

HRESULT CUi_BeatMarker::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_BeatMarker",
			L"../Bin/Resources/Textures/Ui/BeatMarker/BeatMarker%d.png", 2));

	return S_OK;
}

HRESULT CUi_BeatMarker::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);

	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_bRight = Arg->bRight;

	if (m_bRight)
	{
		Set_TextureNum(1);
	}
	else
	{
		Set_TextureNum(0);
	}

	return S_OK;
}

void CUi_BeatMarker::PriorityTick(_float fTimeDelta)
{
}

void CUi_BeatMarker::Tick(_float fTimeDelta)
{

}

void CUi_BeatMarker::LateTick(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
}


shared_ptr<CUi_BeatMarker> CUi_BeatMarker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_BeatMarker
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_BeatMarker(pDevice, pContext) { }
	};

	shared_ptr<CUi_BeatMarker> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_BeatMarker::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_BeatMarker
	{
		MakeSharedEnabler(const CUi_BeatMarker& rhs) : CUi_BeatMarker(rhs) { }
	};

	shared_ptr<CUi_BeatMarker> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
