#include "CUi_StoreEntry.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

wstring CUi_StoreEntry::ObjID = L"CUi_StoreEntry";

CUi_StoreEntry::CUi_StoreEntry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUi_2D(pDevice, pContext)
{
}

CUi_StoreEntry::CUi_StoreEntry(const CUi_2D& rhs)
	:CUi_2D(rhs)
{
}

CUi_StoreEntry::~CUi_StoreEntry()
{
}

HRESULT CUi_StoreEntry::Initialize_Prototype()
{
	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreMenuAttack",
			L"../Bin/Resources/Textures/Ui/Store/AttackStore%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreMenuCustom",
			L"../Bin/Resources/Textures/Ui/Store/CustomStore%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreMenuItem",
			L"../Bin/Resources/Textures/Ui/Store/ItemStore%d.png", 2));

	m_List_ResourceInfo.push_back(
		make_shared<RES_TEXTURE>(L"Prototype_Component_Texture_StoreMenuSkill",
			L"../Bin/Resources/Textures/Ui/Store/SkillStore%d.png", 2));

	return S_OK;
}

HRESULT CUi_StoreEntry::Initialize(void* pArg)
{
	Initialize_Transform();

	if (FAILED(Ready_Components(pArg)))
		assert(false);

	Desc* Arg = reinterpret_cast<Desc*>(pArg);
	
	InitializeUi(Arg->fX, Arg->fY, Arg->fZ, Arg->fSizeX, Arg->fSizeY, Arg->fSizeZ, Arg->fSpeed);

	m_OriginPos.x = Get_Pos().x;
	m_OriginPos.y = Get_Pos().y;
	Set_Font(Arg->Font);

	return S_OK;
}

void CUi_StoreEntry::PriorityTick(_float fTimeDelta)
{
}

void CUi_StoreEntry::Tick(_float fTimeDelta)
{
	Check_Focus(fTimeDelta);
}

void CUi_StoreEntry::LateTick(_float fTimeDelta)
{
	if (m_bActive)
	{
		_float3 Pos = Get_Pos();
		m_FontDesc.DirectPos.x = Pos.x + Font_Distance.x;
		m_FontDesc.DirectPos.y = Pos.y + Font_Distance.y;

		if (m_bFocused)
			m_iTextureNum = 1;
		else
			m_iTextureNum = 0;

			m_pGameInstance.lock()->Add_RenderObjects(CRenderer::RENDER_UI, shared_from_this());
	}
}

HRESULT CUi_StoreEntry::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &Identity)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		assert(false);

	if (FAILED(m_pShaderCom->Bind_Vector4("g_Color", &m_fColor)))
		assert(false);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		assert(false);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		assert(false);

	if (FAILED(m_pShaderCom->Begin((_uint)m_eShaderPass)))
		assert(false);

	if (FAILED(m_pVIBufferCom->Bind_Buffer()))
		assert(false);

	if (FAILED(m_pVIBufferCom->Render()))
		assert(false);

	RenderFont(m_FontDesc);

	return S_OK;
}

void CUi_StoreEntry::Set_Font(FontDescInfo Info)
{
	m_FontDesc = Info;
	Font_Distance = Info.DirectPos;
}

void CUi_StoreEntry::Check_Focus(_float fTimeDelta)
{
	if (m_bFocused)
	{
		if (Get_Pos().x < m_OriginPos.x + 15)
		{
			Move(MoveDir::Right, fTimeDelta);
			Move(MoveDir::Up, fTimeDelta);
		}
		else
		{
			Set_Pos(m_OriginPos.x + 15, m_OriginPos.y + 15);
		}
	}
	else
	{
		if (m_OriginPos.x < Get_Pos().x)
		{
			Move(MoveDir::Left, fTimeDelta);
			Move(MoveDir::Down, fTimeDelta);
		}
		else
		{
			Set_Pos(m_OriginPos.x, m_OriginPos.y);
		}
	}
}

shared_ptr<CUi_StoreEntry> CUi_StoreEntry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	struct MakeSharedEnabler : public CUi_StoreEntry
	{
		MakeSharedEnabler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUi_StoreEntry(pDevice, pContext) { }
	};

	shared_ptr<CUi_StoreEntry> pInstance = make_shared<MakeSharedEnabler>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		assert(false);

	return pInstance;
}

shared_ptr<CGameObject> CUi_StoreEntry::Clone(void* pArg)
{
	struct MakeSharedEnabler : public CUi_StoreEntry
	{
		MakeSharedEnabler(const CUi_StoreEntry& rhs) : CUi_StoreEntry(rhs) { }
	};

	shared_ptr<CUi_StoreEntry> pInstance = make_shared<MakeSharedEnabler>(*this);


	if (FAILED(pInstance->Initialize(pArg)))
		assert(false);


	return pInstance;
}
