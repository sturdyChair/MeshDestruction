#include "CUi_ComboNum.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "PlayerManager.h"


wstring CUi_ComboNum::ObjID = L"CUi_ComboNum";

CUi_ComboNum::CUi_ComboNum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_ComboNum::CUi_ComboNum(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_ComboNum::~CUi_ComboNum()
{
}

HRESULT CUi_ComboNum::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_ComboNum",
			L"../Bin/Resources/Textures/Ui/ComboNum/ComboNum%d.png", 10));

	return S_OK;
}

HRESULT CUi_ComboNum::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	return S_OK;
}

void CUi_ComboNum::PriorityTick(_float fTimeDelta)
{
}

void CUi_ComboNum::Tick(_float fTimeDelta)
{

}

void CUi_ComboNum::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}


shared_ptr<CUi_ComboNum> CUi_ComboNum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_ComboNum
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_ComboNum(pDevice, pContext) { }
	};

	shared_ptr<CUi_ComboNum> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_ComboNum::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_ComboNum
	{
		MakeSharedEnabler(const CUi_ComboNum& rhs) : CUi_ComboNum(rhs) { }
	};

	shared_ptr<CUi_ComboNum> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
