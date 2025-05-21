#include "CUi_TalkSystem.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

#include "Beat_Manager.h"
#include "CUi_TalkCharacter.h"
#include "CUi_TalkWindow.h"


wstring CUi_TalkSystem::ObjID = L"CUi_TalkSystem";

CUi_TalkSystem::CUi_TalkSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_TalkSystem::CUi_TalkSystem(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_TalkSystem::~CUi_TalkSystem()
{
}

HRESULT CUi_TalkSystem::Initialize_Prototype()
{
	m_List_Owning_ObjectID.push_back(CUi_TalkCharacter::ObjID);
	m_List_Owning_ObjectID.push_back(CUi_TalkWindow::ObjID);

	return S_OK;
}

HRESULT CUi_TalkSystem::Initialize(void* pArg)
{
	Initialize_Transform();

	Create_TalkRobotA();

	return S_OK;
}

void CUi_TalkSystem::PriorityTick(_float fTimeDelta)
{
}

void CUi_TalkSystem::Tick(_float fTimeDelta)
{
}

void CUi_TalkSystem::LateTick(_float fTimeDelta)
{

}

HRESULT CUi_TalkSystem::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		assert(false);

	return S_OK;
}

void CUi_TalkSystem::Create_TalkRobotA()
{
	CUi_TalkCharacter::Desc RobotADesc{};
	RobotADesc.fX = -440.f;
	RobotADesc.fY = -230.f;
	RobotADesc.fZ = 0.7f;
	RobotADesc.fSizeX = 550.f;
	RobotADesc.fSizeY = 550.f;
	RobotADesc.fSizeZ = 1.f;
	RobotADesc.fSpeed = 0.f;
	RobotADesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	RobotADesc.TextureFileName = L"Prototype_Component_Texture_TalkRobotA";
	RobotADesc.TextureLevel = LEVEL_STATIC;
	m_pRobotA = static_pointer_cast<CUi_TalkCharacter>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkCharacter::ObjID, &RobotADesc));

	FontDescInfo FontDesc;
	FontDesc.DirectPos = { -273.f, -224.f };
	FontDesc.String = L"SCR-UB";
	FontDesc.Size = 0.3f;
	FontDesc.Color = { 1.f, 1.f, 1.f, 1.f };
	CUi_TalkWindow::Desc BoardDesc{};
	BoardDesc.fX = -210.f;
	BoardDesc.fY = -230.f;
	BoardDesc.fZ = 0.69f;
	BoardDesc.fSizeX = 155.f;
	BoardDesc.fSizeY = 100.f;
	BoardDesc.fSizeZ = 1.f;
	BoardDesc.fSpeed = 0.f;
	BoardDesc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	BoardDesc.TextureFileName = L"Prototype_Component_Texture_TalkBlackBoard";
	BoardDesc.TextureLevel = LEVEL_STATIC;
	BoardDesc.Font = FontDesc;
	m_pBlackBoard = static_pointer_cast<CUi_TalkWindow>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkWindow::ObjID, &BoardDesc));

	CUi_TalkWindow::Desc Win0Desc{};
	Win0Desc.fX = -322.f;
	Win0Desc.fY = -282.f;
	Win0Desc.fZ = 0.67f;
	Win0Desc.fSizeX = 140.f;
	Win0Desc.fSizeY = 140.f;
	Win0Desc.fSizeZ = 1.f;
	Win0Desc.fSpeed = 0.f;
	Win0Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Win0Desc.TextureFileName = L"Prototype_Component_Texture_TalkWindow0";
	Win0Desc.TextureLevel = LEVEL_STATIC;
	m_pWindow0 = static_pointer_cast<CUi_TalkWindow>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkWindow::ObjID, &Win0Desc));

	CUi_TalkWindow::Desc Win1Desc{};
	Win1Desc.fX = -310.f;
	Win1Desc.fY = -282.f;
	Win1Desc.fZ = 0.68f;
	Win1Desc.fSizeX = 140.f;
	Win1Desc.fSizeY = 140.f;
	Win1Desc.fSizeZ = 1.f;
	Win1Desc.fSpeed = 0.f;
	Win1Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Win1Desc.TextureFileName = L"Prototype_Component_Texture_TalkWindow1";
	Win1Desc.TextureLevel = LEVEL_STATIC;
	m_pWindow1 = static_pointer_cast<CUi_TalkWindow>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkWindow::ObjID, &Win1Desc));

	FontDesc.DirectPos = { -285.f, -273.f };
	FontDesc.String = L"이런, 청소하려고 한 건데 더 지저분하게 만들고 말았네.";
	FontDesc.Size = 0.5f;
	FontDesc.Color = { 0.f, 0.f, 0.f, 1.f };
	CUi_TalkWindow::Desc Win2Desc{};
	Win2Desc.fX = 0.f;
	Win2Desc.fY = -282.f;
	Win2Desc.fZ = 0.69f;
	Win2Desc.fSizeX = 1000.f;
	Win2Desc.fSizeY = 140.f;
	Win2Desc.fSizeZ = 1.f;
	Win2Desc.fSpeed = 0.f;
	Win2Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Win2Desc.TextureFileName = L"Prototype_Component_Texture_TalkWindow2";
	Win2Desc.TextureLevel = LEVEL_STATIC;
	Win2Desc.Font = FontDesc;
	m_pWindow2 = static_pointer_cast<CUi_TalkWindow>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkWindow::ObjID, &Win2Desc));

	CUi_TalkWindow::Desc Win3Desc{};
	Win3Desc.fX = 310.f;
	Win3Desc.fY = -282.f;
	Win3Desc.fZ = 0.68f;
	Win3Desc.fSizeX = 140.f;
	Win3Desc.fSizeY = 140.f;
	Win3Desc.fSizeZ = 1.f;
	Win3Desc.fSpeed = 0.f;
	Win3Desc.ShaderFileName = L"Prototype_Component_Shader_VtxPosTex";
	Win3Desc.TextureFileName = L"Prototype_Component_Texture_TalkWindow3";
	Win3Desc.TextureLevel = LEVEL_STATIC;
	m_pWindow3 = static_pointer_cast<CUi_TalkWindow>(m_pGameInstance.lock()->Add_Clone_Return(LEVEL_TEST, L"CUi", CUi_TalkWindow::ObjID, &Win3Desc));
}

shared_ptr<CUi_TalkSystem> CUi_TalkSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_TalkSystem
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_TalkSystem(pDevice, pContext) { }
	};

	shared_ptr<CUi_TalkSystem> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_TalkSystem::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_TalkSystem
	{
		MakeSharedEnabler(const CUi_TalkSystem& rhs) : CUi_TalkSystem(rhs) { }
	};

	shared_ptr<CUi_TalkSystem> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
